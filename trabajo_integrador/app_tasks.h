#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

// ============================================================================
// INCLUDES
// ============================================================================

#include <stdio.h>
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "wrappers.h"
#include "isr.h"

// ============================================================================
// DEFINICIONES DE PRIORIDADES DE TAREAS
// ============================================================================

// Prioridades de inicialización
#define tskSYSTEM_INIT_PRIORITY (tskIDLE_PRIORITY + 3UL)

// Prioridades de control crítico
#define tskSETPOINT_BUTTONS_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define tskBUZZER_CONTROL_PRIORITY (tskIDLE_PRIORITY + 2UL)

// Prioridades de tareas principales
#define tskADC_TRIGGER_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskDISPLAY_MANAGER_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskDISPLAY_TOGGLE_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskPWM_CONTROL_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskLIGHT_SENSOR_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskUSER_BUTTON_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskRGB_CONTROLLER_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskSYSTEM_MONITOR_PRIORITY (tskIDLE_PRIORITY + 1UL)

// ============================================================================
// DEFINICIONES DE STACK SIZE PARA TAREAS
// ============================================================================

// Stack para inicialización
#define tskSYSTEM_INIT_STACK (3 * configMINIMAL_STACK_SIZE)

// Stack para tareas de adquisición de datos
#define tskADC_TRIGGER_STACK (configMINIMAL_STACK_SIZE)
#define tskLIGHT_SENSOR_STACK (2 * configMINIMAL_STACK_SIZE)

// Stack para tareas de control
#define tskPWM_CONTROL_STACK (3 * configMINIMAL_STACK_SIZE / 2)
#define tskRGB_CONTROLLER_STACK (3 * configMINIMAL_STACK_SIZE)

// Stack para tareas de interfaz de usuario
#define tskDISPLAY_MANAGER_STACK (3 * configMINIMAL_STACK_SIZE / 2)
#define tskDISPLAY_TOGGLE_STACK (3 * configMINIMAL_STACK_SIZE / 2)

// Stack para tareas de entrada
#define tskUSER_BUTTON_STACK (configMINIMAL_STACK_SIZE)
#define tskSETPOINT_BUTTONS_STACK (configMINIMAL_STACK_SIZE)
#define tskBUZZER_CONTROL_STACK (configMINIMAL_STACK_SIZE)

// Stack para tareas de monitoreo
#define tskSYSTEM_MONITOR_STACK (3 * configMINIMAL_STACK_SIZE)

// ============================================================================
// VARIABLES GLOBALES EXTERNAS
// ============================================================================

// Handler para la tarea de display manager
extern TaskHandle_t handle_display;

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE INICIALIZACIÓN
// ============================================================================

/**
 * @brief Inicializa todos los periféricos, colas y semáforos del sistema
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_system_init(void *params);

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE ADQUISICIÓN DE DATOS
// ============================================================================

/**
 * @brief Activa conversiones del ADC cada 250ms
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_adc_trigger(void *params);

/**
 * @brief Lee periódicamente el valor de intensidad lumínica del sensor BH1750
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_light_sensor(void *params);

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE CONTROL Y ACTUADORES
// ============================================================================

/**
 * @brief Controla el PWM del LED principal basado en el potenciómetro
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_pwm_control(void *params);

/**
 * @brief Controla el LED RGB basado en el error entre setpoint y valor actual
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_rgb_controller(void *params);

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE INTERFAZ DE USUARIO
// ============================================================================

/**
 * @brief Maneja la visualización multiplexada en el display de 7 segmentos
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_display_manager(void *params);

/**
 * @brief Alterna entre mostrar setpoint y valor de luz en el display
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_display_toggle(void *params);

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE ENTRADA
// ============================================================================

/**
 * @brief Detecta pulsaciones del botón de usuario con debounce
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_user_button(void *params);

/**
 * @brief Controla el buzzer basado en el sensor infrarrojo CNY70
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_buzzer_control(void *params);

/**
 * @brief Maneja los botones S1 y S2 para control del setpoint
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_setpoint_buttons(void *params);

// ============================================================================
// PROTOTIPOS DE FUNCIONES - TAREAS DE MONITOREO
// ============================================================================

/**
 * @brief Imprime información de estado del sistema por consola
 * @param params Parámetros de la tarea (no utilizados)
 */
void task_system_monitor(void *params);

// ============================================================================
// FUNCIONES INLINE DE UTILIDAD
// ============================================================================

/**
 * @brief Wrapper que verifica el estado de un pulsador con pull-up
 *        aplicando un antirebote de 20ms
 * @param btn Estructura del GPIO del pulsador
 * @return true si el pulsador fue presionado, false en caso contrario
 */
static inline bool wrapper_btn_get_with_debouncing_with_pull_up(gpio_t btn)
{
	// Verificar si se presionó el pulsador (lógica negativa por pull-up)
	if (!wrapper_btn_get(btn))
	{
		// Aplicar antirebote de 20ms
		vTaskDelay(pdMS_TO_TICKS(20));
		// Confirmar que el pulsador sigue presionado
		if (!wrapper_btn_get(btn))
		{
			return true;
		}
	}
	return false;
}

#endif /* _APP_TASKS_H_ */