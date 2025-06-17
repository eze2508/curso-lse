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
#define RED_LED_PORT 1         /* Puerto del LED rojo */
#define RED_LED_PIN 2          /* Pin del LED rojo */

#define USER_BUTTON_PORT 0     /* Puerto del botón USER */
#define USER_BUTTON_PIN 4      /* Pin del botón USER */

#define DEBOUNCE_DELAY 50      /* Tiempo de debounce en ms */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void GPIO_Init(void);
bool ReadButtonDebounced(void);
void DelayMS(uint32_t ms);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

/* Inicialización de los pines GPIO */
void GPIO_Init(void)
{
    /* Configurar el pin del LED rojo como salida */
    GPIO->DIR[RED_LED_PORT] |= (1 << RED_LED_PIN);
    
    /* Apagar el LED rojo al inicio */
    GPIO->SET[RED_LED_PORT] = (1 << RED_LED_PIN);
    
    /* Configurar el pin del botón USER como entrada */
    GPIO->DIR[USER_BUTTON_PORT] &= ~(1 << USER_BUTTON_PIN);
}

/* Función simple para delay en ms */
void DelayMS(uint32_t ms)
{
    volatile uint32_t i;
    for (i = 0; i < ms * 2000; i++)
    {
        __NOP();
    }
}

/* Función para leer el botón con debounce por software */
bool ReadButtonDebounced(void)
{
    bool initialState;
    
    /* Leer estado inicial del botón */
    initialState = !(GPIO->PIN[USER_BUTTON_PORT] & (1 << USER_BUTTON_PIN));
    
    if (initialState)
    {
        /* Si el botón parece presionado, esperar para confirmar */
        DelayMS(DEBOUNCE_DELAY);
        
        /* Verificar si el botón sigue presionado */
        return !(GPIO->PIN[USER_BUTTON_PORT] & (1 << USER_BUTTON_PIN));
    }
    
    return false;
}

int main(void)
{
    bool buttonPressed = false;

    /* Init board hardware. */
    BOARD_InitHardware();
    
    /* Inicializar GPIO */
    GPIO_Init();

    /* Add user custom codes below */
    while (1)
    {
        /* Verificar si el botón está presionado */
        buttonPressed = ReadButtonDebounced();
        
        if (buttonPressed)
        {
            /* Encender LED rojo (nivel bajo) */
            GPIO->CLR[RED_LED_PORT] = (1 << RED_LED_PIN);
        }
        else
        {
            /* Apagar LED rojo (nivel alto) */
            GPIO->SET[RED_LED_PORT] = (1 << RED_LED_PIN);
        }
        
        /* Pequeño delay para no sobrecargar el CPU */
        DelayMS(10);
    }
}
