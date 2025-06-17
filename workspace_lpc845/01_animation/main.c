/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"

#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RED_LED_PORT       0  // Puerto del LED rojo
#define RED_LED_PIN        29 // Pin del LED rojo (PIO0_29)
#define USER_BUTTON_PORT   0  // Puerto del botón USER
#define USER_BUTTON_PIN    4  // Pin del botón USER (PIO0_4)

#define DEBOUNCE_TIME_MS   20 // Tiempo de antirebote en ms

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay_ms(uint32_t ms);
void ConfigLED(void);
void ConfigButton(void);
bool ReadButton(void);
void SetLED(bool state);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void delay_ms(uint32_t ms)
{
    uint32_t i;
    for (i = 0; i < ms * 12000; i++) // Aproximación para un retraso de 1ms
    {
        __asm("NOP");
    }
}

void ConfigLED(void)
{
    // Configurar puerto del LED rojo como salida
    GPIO->DIR[RED_LED_PORT] |= (1 << RED_LED_PIN);
    // Inicializar en estado apagado (lógica invertida)
    GPIO->SET[RED_LED_PORT] = (1 << RED_LED_PIN);
}

void ConfigButton(void)
{
    // Configurar puerto del botón USER como entrada
    GPIO->DIR[USER_BUTTON_PORT] &= ~(1 << USER_BUTTON_PIN);
}

bool ReadButton(void)
{
    // Leer el estado del botón USER (activo bajo)
    return !(GPIO->PIN[USER_BUTTON_PORT] & (1 << USER_BUTTON_PIN));
}

void SetLED(bool state)
{
    // Controlar el LED rojo (lógica invertida)
    if (state)
        GPIO->CLR[RED_LED_PORT] = (1 << RED_LED_PIN); // Encender
    else
        GPIO->SET[RED_LED_PORT] = (1 << RED_LED_PIN); // Apagar
}

int main(void)
{
    /* Init board hardware. */
    BOARD_InitHardware();

    // Variables para el antirebote
    bool buttonState = false;
    bool lastButtonState = false;
    bool ledState = false;
    uint32_t lastDebounceTime = 0;
    uint32_t currentTime = 0;

    // Configuraciones
    ConfigLED();
    ConfigButton();

    /* Add user custom codes below */
    while (1)
    {
        // Leer el estado actual del botón
        bool reading = ReadButton();

        // Actualizar el contador de tiempo
        currentTime++;

        // Si el estado del botón cambió, reiniciar el contador de antirebote
        if (reading != lastButtonState)
        {
            lastDebounceTime = currentTime;
        }

        // Verificar si ha pasado suficiente tiempo desde el último cambio
        if ((currentTime - lastDebounceTime) > DEBOUNCE_TIME_MS)
        {
            // Si el estado del botón ha cambiado (después del antirebote)
            if (reading != buttonState)
            {
                buttonState = reading;
                
                // Actualizar el estado del LED según el estado del botón
                ledState = buttonState;
                SetLED(ledState);
            }
        }

        // Guardar el estado actual como el último estado conocido
        lastButtonState = reading;

        // Pequeño retraso para mejorar la eficiencia
        delay_ms(1);
    }
}
