# Blinky

Investigate further, using the processor manual:

● What are the hardware registers that cause the LED to turn on and off? (From the
processor manual, don’t worry about initialization.)

ODR: output data register


● What are the registers that you read in order to find out the state of the button?

IDR: input data register


● Can you read the register directly and see the button change in a debugger or by
● printing out thes value of the memory at the register’s address?

Button NotPressed IDR Value: 0x8010 

Pin = 0x1, IDR bit that corresponds to the pin = 0 when button released



Button Pressed IDR Value: 0x8011

Pin = 0x1, IDR bit that corresponds to the pin = 1 when button pressed

