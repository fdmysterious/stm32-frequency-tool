=====================================================================================
Simple 3 channel frequency generator / frequency meter using an STM32 microcontroller
=====================================================================================

:Authors:  - Florian Dupeyron <florian.dupeyron@mugcat.fr>
:Date:     April 2022

Build requirements
==================

- A linux build environment
- Docker
- stlink-tools

Build procedure
===============

1. Init and fetch external dependencies:

.. code:: bash

    git submodule update --init 

2. Launch the build

.. code:: bash
    
    ./build.sh

That should be all. You can flash on the target board with the ̀`./flash.sh` script.

Pinout and stuff
================

Please see the `project/src/io/gpio.c` file for te pins.

Serial interface
================

The used serial interface is based on PRPC_ and has the following methods (quick and dirty description):

.. table::

    +------------------------+----------------------------------------------------------+-------------------------------+
    | Command name           | Description                                              | Example                       |
    +========================+==========================================================+===============================+
    | `pwm1/frequency/set`   | Sets the frequency for PWM channel 1                     | `0:pwm1/frequency/set 2000.0` |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm1/duty/set`        | Sets the duty cycle for PWM channel 1 (0..1)             | `0:pwm1/duty/set 0.5`         |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm1/polarity/set`    | Sets the polarity (+ or -)                               | `0:pwm1/polarity/set "+"`     |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm1/started/set`     | Start/stops the generator                                | `0:pwm1/started/set yes`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm1/started/get`     | Gets the status of generation                            | `0:pwm1/started/get`          |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm2/frequency/set`   | Sets the frequency for PWM channel 1                     | `0:pwm1/frequency/set 2000.0` |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm2/duty/set`        | Sets the duty cycle for PWM channel 1 (0..1)             | `0:pwm1/duty/set 0.5`         |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm2/polarity/set`    | Sets the polarity (+ or -)                               | `0:pwm1/polarity/set "+"`     |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm2/started/set`     | Start/stops the generator                                | `0:pwm1/started/set yes`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm2/started/get`     | Gets the status of generation                            | `0:pwm1/started/get`          |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm3/frequency/set`   | Sets the frequency for PWM channel 1                     | `0:pwm1/frequency/set 2000.0` |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm3/duty/set`        | Sets the duty cycle for PWM channel 1 (0..1)             | `0:pwm1/duty/set 0.5`         |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm3/polarity/set`    | Sets the polarity (+ or -)                               | `0:pwm1/polarity/set "+"`     |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm3/started/set`     | Start/stops the generator                                | `0:pwm1/started/set yes`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `pwm3/started/get`     | Gets the status of generation                            | `0:pwm1/started/get`          |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter1/negative/get` | Get the negative time measurement from frequency meter 1 | `0:fmeter1/negative/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter1/positive/get` | Get the positive time measurement from frequency meter 1 | `0:fmeter1/positive/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter2/negative/get` | Get the negative time measurement from frequency meter 2 | `0:fmeter2/negative/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter2/positive/get` | Get the positive time measurement from frequency meter 2 | `0:fmeter2/positive/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter3/negative/get` | Get the negative time measurement from frequency meter 3 | `0:fmeter3/negative/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+
    | `fmeter3/positive/get` | Get the positive time measurement from frequency meter 3 | `0:fmeter3/positive/get`      |
    +------------------------+----------------------------------------------------------+-------------------------------+

.. _PRPC: https://github.com/fdmysterious/PRPC
