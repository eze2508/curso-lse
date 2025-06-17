

#include "fsl_device_registers.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"

#include "board.h"
#include "app.h"


#define DELAY_COUNT 500000

/* Prototypes
 ******************************************************************************/
void delay(uint32_t count);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */


void delay(uint32_t count)
{
    volatile uint32_t i = 0;
    for (i = 0; i < count; ++i)
    {
        __asm("NOP");
    }
}

int main(void)
{


    BOARD_InitHardware();


    GPIO->DIR[1] |= (1 << 1);


    while (1)
    {

        GPIO->NOT[1] = (1 << 1);

        delay(DELAY_COUNT);
    }
}
