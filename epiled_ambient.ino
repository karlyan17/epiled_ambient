#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht

#include <FHT.h>

//////////////////// define operation variables ///////////////

int const red_led = 11;          // - digital pins for your leds
int const green_led = 10;        //   need to be PWM able
int const blue_led = 9;
int const readjust_time = 10000; // - readjustment time in ms   
       
//int max_volt = 200;         // - estimated maximal voltage in A.U.
int amp = 0;                  // - maximal amplitude in one cycle
int temp_max_amp;             // - store amp for readjust
int max_amp = 200;            // - maximum amplitude over readjustment time
int readjust_counter = 0;    // - counts ms to 10s in ms
int real_input[256];
//  int board_volt = 5000;    // - arduino board voltage in mV

void setup() {
  ///////////////// ADC reading vodoo /////////////////////////
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  //////////////// LED operation //////////////////////////////
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  //////////////// Debugging over serial //////////////////////
  Serial.begin(115200);
  Serial.println("Serial port open on 9600 with ln");
}

void loop() {
  while(1) {
    ////////////// Data acquisition and processing ////////////
    
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
      real_input[i] = k;
      //Serial.println(k);
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
//    noInterrupts();
//    amp = 0;
//    for(int i = 0; i < (main_intv*10); i++) {
//      // each sampling step takes approx. 100 microseconds
//      // real voltage is approx. U_real = (5*amp)mV
//      amp = max(analogRead(audio_r),amp);
//    }
//    max_volt_collect = max(amp,max_volt_collect);
//    Serial.print(max_volt);
//    Serial.print("|");
//    Serial.print(max_volt_collect);
//    Serial.print("| amp: ");
//    for (int t = 0; t < amp; t++) {
//      Serial.print("#");
//    }
//    Serial.print("\n");
  //  digitalWrite(blue_led, LOW);
  //  digitalWrite(red_led, LOW);
  //  digitalWrite(green_led, LOW);
  //  if (amp*(board_volt/max_volt) > 750) {
  //    digitalWrite(blue_led, HIGH);
  //    //delay(200 - 2*main_intv);
  //  }
  //  else if (amp*(board_volt/max_volt) > 600) {
  //    digitalWrite(green_led, HIGH);
  //    //delay(200 - 2*main_intv);
  //  }
  //  else if (amp*(board_volt/max_volt) > 200) {
  //    digitalWrite(red_led,HIGH);
  //    //delay(200 - 2*main_intv);
  //  }
    /////////// Data interpretation and visualisation /////////
    //--------- Amplitude part ------------------------------//
    for (int j = 0; j < 256; j++) {
      amp = max(amp, real_input[j] + 32768);
      //Serial.println(real_input[j]);
    }
    temp_max_amp = max(amp, temp_max_amp);
    analogWrite(red_led, map(amp,0,max_amp,0,255));
    analogWrite(green_led,map(amp,0,max_amp,0,255));
    analogWrite(blue_led, map(amp,0,max_amp,0,255));
    if (millis()>= readjust_counter) {
      readjust_counter += readjust_time;
      max_amp = max(10,temp_max_amp);
      temp_max_amp = 0;
    }
  }
}
