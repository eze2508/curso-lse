#include "app_tasks.h"

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// Colas para comunicación entre tareas
xQueueHandle queue_adc;				// Cola para datos del ADC
xQueueHandle queue_lux;				// Cola para datos de luminosidad
xQueueHandle queue_display;			// Cola para datos del display
xQueueHandle queue_display_control; // Cola para control del display
xQueueHandle queue_setpoint;		// Cola para el valor de setpoint

// Semáforos para sincronización
xSemaphoreHandle semphr_buzz;	 // Semáforo para interrupción del infrarojo
xSemaphoreHandle semphr_counter; // Semáforo contador
xSemaphoreHandle semphr_usr;	 // Semáforo para botón de usuario

// Manejadores de tareas
TaskHandle_t handle_display; // Handler para la tarea de display

// ============================================================================
// TAREAS DE INICIALIZACIÓN
// ============================================================================

/**
 * @brief Inicializa todos los periféricos, colas y semáforos del sistema
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_system_init(void *params)
{
	// Inicialización de semáforos
	semphr_buzz = xSemaphoreCreateBinary();
	semphr_usr = xSemaphoreCreateBinary();
	semphr_counter = xSemaphoreCreateCounting(50, 25);

	// Inicialización de colas
	queue_adc = xQueueCreate(1, sizeof(adc_data_t));
	queue_lux = xQueueCreate(1, sizeof(uint16_t));
	queue_setpoint = xQueueCreate(1, sizeof(uint16_t));
	queue_display = xQueueCreate(1, sizeof(uint8_t));
	queue_display_control = xQueueCreate(1, sizeof(bool));

	// Inicialización de GPIO
	wrapper_gpio_init(0);
	wrapper_gpio_init(1);

	// Inicialización de salidas digitales
	wrapper_output_init((gpio_t){LED}, true);
	wrapper_output_init((gpio_t){BUZZER}, false);
	wrapper_output_init((gpio_t){CNY70_EN}, true);

	// Inicialización de periféricos
	wrapper_adc_init();
	wrapper_display_init();
	wrapper_btn_init();
	wrapper_pwm_init();
	wrapper_i2c_init();
	wrapper_bh1750_init();

	// Eliminar tarea para liberar recursos
	vTaskDelete(NULL);
}

// ============================================================================
// TAREAS DE ADQUISICIÓN DE DATOS
// ============================================================================

/**
 * @brief Activa conversiones del ADC cada 250ms
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_adc_trigger(void *params)
{
	while (1)
	{
		// Iniciar conversión por software
		ADC_DoSoftwareTriggerConvSeqA(ADC0);
		// Esperar 250ms antes de la próxima conversión
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

/**
 * @brief Lee periódicamente el valor de intensidad lumínica del sensor BH1750
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_light_sensor(void *params)
{
	uint16_t lux = 0;

	while (1)
	{
		// Esperar 200ms entre lecturas
		vTaskDelay(pdMS_TO_TICKS(200));

		// Leer valor del sensor de luz
		lux = wrapper_bh1750_read();

		// Convertir a porcentaje (0-30000 lux -> 0-100%)
		uint16_t porcentaje = lux * 100 / 30000;

		// Actualizar cola con el nuevo valor
		xQueueOverwrite(queue_lux, &porcentaje);
	}
}

// ============================================================================
// TAREAS DE CONTROL Y ACTUADORES
// ============================================================================

/**
 * @brief Controla el PWM del LED principal basado en el potenciómetro
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_pwm_control(void *params)
{
	adc_data_t data = {0};

	while (1)
	{
		// Esperar datos del ADC
		xQueuePeek(queue_adc, &data, portMAX_DELAY);

		// Calcular duty cycle del potenciómetro (0-100%)
		uint8_t duty = (100 * data.ref_raw) / 4095;

		// Aplicar duty cycle al LED si está en rango válido
		if (duty < 100 && duty > 0)
		{
			SCTIMER_UpdatePwmDutycycle(SCT0, kSCTIMER_Out_0, duty, 0);
		}

		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

/**
 * @brief Controla el LED RGB basado en el error entre setpoint y valor actual
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_rgb_controller(void *params)
{
	uint16_t lux_actual;
	uint16_t setpoint;

	while (1)
	{
		// Obtener valores actuales
		xQueuePeek(queue_lux, &lux_actual, portMAX_DELAY);
		xQueuePeek(queue_setpoint, &setpoint, portMAX_DELAY);

		// Calcular error
		int16_t error = lux_actual - setpoint;

		// Control basado en el error
		if (error > 0)
		{
			// Valor por encima del setpoint -> LED rojo
			wrapper_pwm_update_bled(0);
			wrapper_pwm_update_rled(error);
		}
		else
		{
			// Valor por debajo del setpoint -> LED azul
			wrapper_pwm_update_rled(0);
			wrapper_pwm_update_bled(-error);
		}

		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

// ============================================================================
// TAREAS DE INTERFAZ DE USUARIO
// ============================================================================

/**
 * @brief Maneja la visualización multiplexada en el display de 7 segmentos
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_display_manager(void *params)
{
	uint16_t data;
	bool show_setpoint = false;

	while (1)
	{
		// Determinar qué mostrar (setpoint o valor de luz)
		xQueuePeek(queue_display_control, &show_setpoint, pdMS_TO_TICKS(100));

		if (show_setpoint)
		{
			xQueuePeek(queue_setpoint, &data, pdMS_TO_TICKS(100));
		}
		else
		{
			xQueuePeek(queue_lux, &data, pdMS_TO_TICKS(100));
		}

		// Mostrar dígito de las decenas
		wrapper_display_off();
		wrapper_display_write((uint8_t)(data / 10), show_setpoint);
		wrapper_display_on((gpio_t){COM_1});
		vTaskDelay(pdMS_TO_TICKS(10));

		// Mostrar dígito de las unidades
		wrapper_display_off();
		wrapper_display_write((uint8_t)(data % 10), show_setpoint);
		wrapper_display_on((gpio_t){COM_2});
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/**
 * @brief Alterna entre mostrar setpoint y valor de luz en el display
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_display_toggle(void *params)
{
	bool current_mode = false;

	while (1)
	{
		// Esperar señal del botón de usuario
		xSemaphoreTake(semphr_usr, portMAX_DELAY);

		// Leer estado actual del display
		xQueuePeek(queue_display_control, &current_mode, pdMS_TO_TICKS(100));

		// Alternar modo de visualización
		bool new_mode = !current_mode;
		xQueueOverwrite(queue_display_control, &new_mode);

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

// ============================================================================
// TAREAS DE ENTRADA (BOTONES Y SENSORES)
// ============================================================================

/**
 * @brief Detecta pulsaciones del botón de usuario con debounce
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_user_button(void *params)
{
	while (1)
	{
		// Detectar flanco descendente (botón presionado)
		if (!GPIO_PinRead(USR_BTN))
		{
			vTaskDelay(pdMS_TO_TICKS(50)); // Debounce

			// Confirmar liberación del botón
			if (GPIO_PinRead(USR_BTN))
			{
				xSemaphoreGive(semphr_usr);
			}
		}
		else
		{
			continue;
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/**
 * @brief Controla el buzzer basado en el sensor infrarrojo CNY70
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_buzzer_control(void *params)
{
	while (1)
	{
		// Detectar flanco ascendente
		if (!GPIO_PinRead(CNY70))
		{
			vTaskDelay(pdMS_TO_TICKS(50));
			if (GPIO_PinRead(CNY70))
			{
				wrapper_output_toggle((gpio_t){BUZZER});
			}
		}
		else
		{
			vTaskDelay(pdMS_TO_TICKS(50));
			// Detectar flanco descendente
			if (!GPIO_PinRead(CNY70))
			{
				wrapper_output_toggle((gpio_t){BUZZER});
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

/**
 * @brief Maneja los botones S1 y S2 para control del setpoint
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_setpoint_buttons(void *params)
{
	while (1)
	{
		// Botón S1: decrementar setpoint
		if (wrapper_btn_get_with_debouncing_with_pull_up((gpio_t){S1_BTN}))
		{
			xSemaphoreTake(semphr_counter, 0);
		}
		// Botón S2: incrementar setpoint
		else if (wrapper_btn_get_with_debouncing_with_pull_up((gpio_t){S2_BTN}))
		{
			xSemaphoreGive(semphr_counter);
		}

		// Actualizar valor del setpoint
		uint16_t setpoint_value = uxSemaphoreGetCount(semphr_counter) + 25;
		xQueueOverwrite(queue_setpoint, &setpoint_value);

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

// ============================================================================
// TAREAS DE MONITOREO Y DEBUG
// ============================================================================

/**
 * @brief Imprime información de estado del sistema por consola
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_system_monitor(void *params)
{
	uint8_t porcentaje_luz = 0;
	adc_data_t data_adc = {0};
	uint16_t setpoint_actual = 0;

	while (1)
	{
		// Obtener datos de las colas
		xQueuePeek(queue_setpoint, &setpoint_actual, pdMS_TO_TICKS(100));
		xQueuePeek(queue_lux, &porcentaje_luz, portMAX_DELAY);
		xQueuePeek(queue_adc, &data_adc, portMAX_DELAY);

		// Calcular tiempo transcurrido
		TickType_t tiempo_actual = xTaskGetTickCount();
		uint32_t tiempo_ms = tiempo_actual * 1000;

		// Calcular duty cycle del LED
		uint16_t duty_led = 100 - (100.0 * data_adc.ref_raw) / 4095.0;

		// Imprimir información del sistema
		PRINTF("Tiempo: %ld ms\n", tiempo_ms);
		PRINTF("Luminosidad actual: %d%%\n", porcentaje_luz);
		PRINTF("Setpoint: %d%%\n", setpoint_actual);
		PRINTF("Brillo LED D1: %d%%\n", duty_led);
		PRINTF("------------------------\n");

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}