# Sparkfun Load Cell Amp Firmware

## Datasheet

https://cdn.sparkfun.com/assets/b/f/5/a/e/hx711F_EN.pdf
This is the Datasheet for the particular IC that is used on the Sparkfun Load-Cell Amp.


## Interface Description

- Supply Voltage: 2.6 - 5.5V

XI = 0: On-Chip Oscillator
RATE = 0: 10Hz Signal

- Data is signed 2's complement 24 bit (TO BE TESTED: Negative numbers?)

- Serial interface:
  - Clock Signal (IN)
    - Minimum frequency is 20 kHz (If 0.5 duty cycle is used)
    - Default frequency is 500 kHz
    - Maximum frequency is 2.5 MHz
    - PD_SCK can onset 0.1us after DOUT has fallen

  - Data Signal (OUT)
    - When DOUT goes low, Data is available.
    - 25 or 27 pulses should be sent on PD_SCK, clocking the serial data out.
    - The data can be read 0.1us after the clock pulse arrived.

  - Powering Down
    - Default PD_SCK = 0
    - if PD_SCK for > 60us: IC powers down
    - if IC powered down and PD_SCK -> 1: Power up again
