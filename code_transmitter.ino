int count = 0, countaux = 0;
unsigned char arr_elem[64] = {
  0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ISR(TIM0_COMPA_vect)
{
  count++;
  if (count == 15){ // At ~37.6 KHz, 11 cycles give about 300 us pulses, which is compatible with 600 and 900 us pulses
    PORTB = arr_elem[countaux];
    countaux = (countaux + 1) & 0x0000003F; //This makes it module 32 easily
    count = 0;
  }
}

void setup() {
  // Clock frequency can be changed by changing the prescaler value in CLKPSR (by default at /8)
  // Clock can even be calibrated to ensure 8 MHz frequency is achieved
  // Write the signature D8 in the CCP to unprotect CLKPSR
  CCP = 0xD8;
  CLKPSR = 0;
  // put your setup code here, to run once:
  DDRB = 3;                       // PB0 AND PB1 as an output
  TCCR0A = 0 << COM0A1 | 0 << COM0A0 | 1 << COM0B0 | 0 << WGM00;  // Toggle OC0A, CTC mode
  TCCR0B = 1<<WGM02 | 2<<CS00;    // CTC mode; use OCR0A or OCR0B; /8
  OCR0A = 12;                  // 12 gives a frequency of roughly 37.6 kHz
  OCR0B = 12;
  //Configure interrupts
  
  //Enable interrupts globally
  asm("sei");
  //Enable interrupt on compare match in channel A  
  TIMSK0 = 1 << OCIE0A;
}

void loop() {
  // put your main code here, to run repeatedly:

}
