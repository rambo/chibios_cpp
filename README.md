# ChibiOS C++ experiments

Running on Ruuvitracker devel HW (and if this goes well I will suggest moving the main RT firmware to C++ as well).

Any other STM32F4 board ought to work too.

## Coding standards

### Indentation

Hard-tabs, 4 spaces. Logical blocks are always indented. for example

    switch(foo)
    {
        case 0x1:
            bar = 1;
            break;
    }

Different conditions in same if-statement go on separate lines

    if (   !shelltp
        && (SDU.config->usbp->state == USB_ACTIVE))
    {
        board_green_led(PAL_HIGH);
        shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    }



### Curly brackets go to their own line

Like so:

    if (foo)
    {
        bar = 1;
    }

This also demonstrates that even simple ifs with single statements will have their curly braces.



### Ternary operator is forbidden

It *really* hurts code readibility and breaks the rules about indentation.

    if (foo)
    {
        bar = 1;
    }
    else
    {
        bar = 2;
    }



### Naming

We use the underscore style. Not CamelCase or javaCase (I know ChibiOS uses Java style, we do not, this also serves
the purpose of easily distinguishing our own stuff from ChibiOS stuff).

For thread classes add `_thd` to the end of the name. For shell commands add `cmd_` to the beginning of the name.



### Types

Use the smallest type that is suitable, use the correct `xxx_t` typedef. Especially do not use `char` when you mean signed 8-bit integer, use `int8_t`. 

Specify unsigned type if you are not going to use negative numbers, even if you do ne need the extra range.



### Object-Oriented

Use classes, do not be afraid to use inheritance (for example the accelerometer class should inherit from a generic I2C device class the common helper methods that each device is going to need).

There are cases where a single procedural function is a better choice, but if you find that you're passing pointer to commond data around you are probably better served by OOP-approach.

