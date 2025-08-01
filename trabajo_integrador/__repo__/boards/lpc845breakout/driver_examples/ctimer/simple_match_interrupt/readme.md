# ctimer_match_interrupt_example

## Overview
The Simple Match Interrupt project is to demonstrate usage of the SDK CTimer driver with interrupt callback functions
In this example the upon match and IO pin connected to the LED is toggled and the timer is reset, so it would generate a square wave.
When the number of times of entering the interrupt callback function is greater than matchUpdateCount, matchValue will be divided by 2,
and matchUpdateCount will be multiplied by 2. When matchUpdateCount == 0XFF, matchUpdateCount and matchValue will return to the
original settings.
With an interrupt callback the match value is changed frequently in such a way that the frequency of the output square wave is increased gradually.

## Supported Boards
- [EVK-MIMXRT595](../../../_boards/evkmimxrt595/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [EVK-MIMXRT685](../../../_boards/evkmimxrt685/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXA153](../../../_boards/frdmmcxa153/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXA156](../../../_boards/frdmmcxa156/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXA346](../../../_boards/frdmmcxa346/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXL255](../../../_boards/frdmmcxa346/driver_examples/ctimer/simple_pwm_interrupt/example_board_readme.md)
- [FRDM-MCXN236](../../../_boards/frdmmcxn236/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXN947](../../../_boards/frdmmcxn947/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-MCXW23](../../../_boards/frdmmcxw23/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [FRDM-RW612](../../../_boards/frdmrw612/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPC845BREAKOUT](../../../_boards/lpc845breakout/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso51U68](../../../_boards/lpcxpresso51u68/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso54628](../../../_boards/lpcxpresso54628/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso54S018](../../../_boards/lpcxpresso54s018/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso54S018M](../../../_boards/lpcxpresso54s018m/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso55S06](../../../_boards/lpcxpresso55s06/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso55S16](../../../_boards/lpcxpresso55s16/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso55S28](../../../_boards/lpcxpresso55s28/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso55S36](../../../_boards/lpcxpresso55s36/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso55S69](../../../_boards/lpcxpresso55s69/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso802](../../../_boards/lpcxpresso802/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso804](../../../_boards/lpcxpresso804/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [LPCXpresso845MAX](../../../_boards/lpcxpresso845max/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [MCX-N5XX-EVK](../../../_boards/mcxn5xxevk/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [MCX-N9XX-EVK](../../../_boards/mcxn9xxevk/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [MIMXRT685-AUD-EVK](../../../_boards/mimxrt685audevk/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [MIMXRT700-EVK](../../../_boards/mimxrt700evk/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [RD-RW612-BGA](../../../_boards/rdrw612bga/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
- [MCXW23-EVK](../../../_boards/mcxw23evk/driver_examples/ctimer/simple_match_interrupt/example_board_readme.md)
