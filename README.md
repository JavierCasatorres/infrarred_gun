"# infrarred_gun" 

### IMPORTANT ###
ATTINY10 has only usable I/O by default. It has, however, another pin which is usually used as a /RESET for the TPI programming interface.
The reset can be disabled by disabling the RSTDISBL fuse with AVRDUDE and USBASP (if it has been updated to support TPI):
-Ufuse:w:0xfe:m (see https://www.engbedded.com/fusecalc/ for reference).
However, once disabled the chip can no longer be programmed with the usual programer (USBASP). It is required that 12V are applied to the /RESET pin for the whole programming session. This has been achieved by creating a voltage converter which can be placed as an intermediate between the USBASP and the ATTINY10, converting the usual 0V of "enabling" reset into 12V (and the usual 5V into 0V) via the use of a NMOS:

        12V
       ____
        |
        Z
        Z 10 K
        Z
        |------> Vout
     ___|
     |
---> | NMOS
     |___
         |
         |
       __|__
        ___
        _
         
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