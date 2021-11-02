#define COUNT_LIMIT 4000000
int count = 0, countaux = 0, count_button = 0, enable_out = 0;

unsigned char arr_elem[64] = {
  0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ISR(TIM0_COMPA_vect)
{
  count++;
  if (count == 15){ // At ~37.6 KHz, 11 cycles give about 300 us pulses, which is compatible with 600 and 900 us pulses
    PORTB = (PORTB & (3u << 0))| enable_out & arr_elem[countaux] | enable_out << 1;
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


  // Configure both carrier output counter (counter B) and modulating signal output counter (counter A)
  DDRB = 7;                       // PB0 AND PB1 as an output
  TCCR0A = 0 << COM0A1 | 0 << COM0A0 | 1 << COM0B0 | 0 << WGM00;  // Toggle OC0B (channel B) in CTC mode, channel A works normally (as GPIO)
  TCCR0B = 1<<WGM02 | 2<<CS00;    // CTC mode; use OCR0A or OCR0B; Use /8 prescaler
  OCR0B = 12;                  // 12 gives a frequency of roughly 37.6 kHz for the carrier (channel B).
  OCR0A = 12;                  // Note that as the counter is RESET each time counter B reaches 12, the compare value for channel A
                               // cannot be higher than 12 or it shall never be reached. A value of 12 means interrupting at 37.6 KHz
                               // which is pretty high. Note that as the counter must reach 12 and the instructions are 8 times faster
                               // than the counter (because of the /8 value for the prescaler), 96 instructions shall execute in between
                               // interrupts
  //Configure interrupts
  //Enable interrupts globally
  asm("sei");
  //Enable interrupt on compare match in channel A  
  TIMSK0 = 1 << OCIE0A;
}

void loop() {
  // When the button (third port) is pushed, increase counter at a certain rate
  if(PORTB & (1u << 3)){
    enable_out = 1;
    if(count_button != COUNT_LIMIT) // If maximum value is reached, saturate
      count_button += 2;
  }
  // When not pushed, decrease counter at half rate
  else{
    enable_out = 0;
    if (count_button != 0) // If minimum value is reached, saturate
      count_button--;
  }

  // If the limit is reached, shoouting is disabled
  // for cooldown time
  if(count_button == COUNT_LIMIT){
    enable_out = 0;
    PORTB = 1u << 2;
    // Locked here until count limit is reached
    for(int i = 0; i < COUNT_LIMIT; i++);
    PORTB = 0;
    count_button = 0;
  }
}
