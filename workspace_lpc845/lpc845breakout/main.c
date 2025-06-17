
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"
#include "board.h"
#include "app.h"

int main(void)
{

    gpio_pin_config_t out_config = {
        kGPIO_DigitalOutput, 0,
    };
    GPIO_PortInit(GPIO, 1);
	GPIO_PinInit(GPIO, 1, 0, &out_config);

    /* Add user custom codes below */
    while (1)
    {
        GPIO_PinWrite(GPIO, 1, 0, !GPIO_PinRead(GPIO, 1, 0));

        for(uint32_t i=0; i<1000000; i++)
        {
            __asm("nop");
        }
    }
    return 0;
}