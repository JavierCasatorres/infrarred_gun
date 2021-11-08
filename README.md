# Infrarred_gun
The project is designed to use an ATTINY10 as driver for a (high? power) collimated IR LED (~950 nm, https://es.aliexpress.com/item/1005002310393752.html?spm=a2g0s.9042311.0.0.27e063c02eyOOF) to modulate signals such as it is done in laser tag games.
The receiver is a chinese universal receiver (VS838), which requires pulses of ~600-800 us modulated @ ~38 kHz.

## TRANSMITTER ##
PB1 pin is used to output a 38 kHz signal generated with Attiny timer TC0B (working at 1MHz, with a 8 MHz instruction clock and a /8 prescaler).
An specific PCB is designed to use 2 P channel MOSFETs to effectively __and__ (thus modulating) the carrier with the data signal, which is output from PB0 pin.
PB3 is used as input for a _trigger_ signal (nothing is output if the trigger is not pressed). PB2 is used to output an "overload" signal, which is set when the trigger is pulsed continuously for ~1 second, simulating an overheating of the gun. If overload is reached, a ~3s cooldown period is required

### IMPORTANT ###
ATTINY10 has only usable I/O by default. It has, however, another pin which is usually used as a /RESET for the TPI programming interface.
The reset can be disabled by disabling the RSTDISBL fuse with AVRDUDE and USBASP (if it has been updated to support TPI):
-Ufuse:w:0xfe:m (see https://www.engbedded.com/fusecalc/ for reference).
However, once disabled the chip can no longer be programmed with the usual programer (USBASP). It is required that 12V are applied to the /RESET pin for the whole programming session. This has been achieved by creating a voltage converter which can be placed as an intermediate between the USBASP and the ATTINY10, converting the usual 0V of "enabling" reset into 12V (and the usual 5V into 0V) via the use of a NMOS:

-        12V
-       ____
-        |
-        Z
-        Z 10 K
-        Z
-        |------> Vout
-     ___|
-     |
----> | NMOS
-     |___
-         |
-         |
-       __|__
-        ___
-         _
         
However, note that wven though fuses are easily "programmed" (set to zero) with the AVRDUDE command, they cannot be "reset" (set to 1) with the equivalent AVRDUDE command, the only way to reset them as stated in the datasheet:

Erasing the Configuration Section
The algorithm for erasing the Configuration section is as follows:
1. Write the 0x14 (SECTION_ERASE) to the NVMCMD register
2. Start the erase operation by writing a dummy byte to the high byte of any word location inside the
configuration section
3. Wait until the NVMBSY bit has been cleared

This is (thankfully!) currently supported by AVRDUDE, as pointed out by https://www.avrfreaks.net/forum/svn-avrdude-supports-attiny45910-fuse-reset
Use AVRDUDE with the following command and the fuses shall be reset, so the chip will be responsive to usual programming:
-x section_config -e