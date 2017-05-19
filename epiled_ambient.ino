#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht

#include <FHT.h>
//////////////////// define operation variables ///////////////

int const red_led = 9;          // - digital pins for your leds
int const green_led = 10;        //   need to be PWM able
int const blue_led = 11;
int const readjust_time = 1000; // - readjustment time in ms
int const est_noise = 150;     
       
//int max_volt = 200;         // - estimated maximal voltage in A.U.
int amp = 0;                  // - maximal amplitude in one cycle
int freq = 0;
int amp10[15];
int freq10[20];
int temp_max_amp;             // - store amp for readjust
int min_freq;
int max_amp = 2000;            // - maximum amplitude over readjustment time
int max_freq = 5;
unsigned long readjust_counter = 0;    // - counts ms to 10s in ms
int real_input[256];
uint8_t bg[128];
//  int board_volt = 5000;    // - arduino board voltage in mV

void setup() {
  ///////////////// ADC reading vodoo /////////////////////////
  //TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  //////////////// LED operation //////////////////////////////
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  //////////////// Debugging over serial //////////////////////
//  Serial.begin(115200);
//  Serial.println("Serial port open on 9600 with ln");
}

void loop() {
  while(1) {
    ////////////// Data acquisition and processing ////////////
    amp = 0;
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
      if (k+32768 > est_noise) {               //////
        amp = max(amp, k + 32768);       // subtract nasty background
      }                                  // from power supply
      //Serial.println(amp);
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
    
    /////////// Data interpretation ///////////////////////////
    //--------- Amplitude part ------------------------------//
    for (int i = 14; i > 0; i--) {
      amp10[i] = amp10[i-1];
    }
    amp10[0] = amp;
    temp_max_amp = max(amp, temp_max_amp);
    int amp_sum = 0;
    for (int i = 0; i < 15; i++) {
      amp_sum += amp10[i];
    }
    amp = amp_sum / 15;
    //Serial.println(amp_sum);
    //Serial.println(amp);
    //--------- Frequency part ------------------------------//
    if (amp == 0) {
      //memcpy(bg, fht_log_out, sizeof(bg));
      //Serial.print("BACKGROUND ");
      for (int k = 0; k < 128; k++){
        bg[k] = fht_log_out[k];
      }
      freq = 0;
    }

    int max_magn = 0;
    for (int i = 1; i < 128; i++) {
      if (bg[i] <= fht_log_out[i]) fht_log_out[i] = fht_log_out[i] - bg[i];
      //Serial.println(fht_log_out[i]);
      if (fht_log_out[i] > max_magn) {
        max_magn = fht_log_out[i];
        freq = i;
        //Serial.print(fht_log_out)
        //Serial.println(i);
      }
    }
    for (int i = 19; i > 0; i--) {
      freq10[i] = freq10[i-1];
    }
    freq10[0] = freq;
    int freq_sum = 0;
    for (int i = 0; i < 20; i++) {
      freq_sum += freq10[i];
    }
    freq = freq_sum / 20;
    max_freq = max(freq, max_freq);
    min_freq = min(min_freq, freq);
//    Serial.print(amp);
//    Serial.print("/");
//    Serial.print(max_amp);
//    Serial.print(" ");
//    Serial.print(freq);
//    Serial.print("/");
//    Serial.print(max_freq);
//    Serial.print(" ");
    //////////// Data visualisation ///////////////////////////
    int color = map (freq,0,max_freq+5,0,510);
    int unsigned output[3];
//    Serial.print(color);
    output[0] = max(0,255 - color);
    output[1] = min(color, 510 - color);
    output[2] = max(0, color - 255);
//    Serial.print(" r:");
//    Serial.print(output[0]);
//    Serial.print(" g:");
//    Serial.print(output[1]);
//    Serial.print(" b:");
//    Serial.print(output[2]);
//    Serial.print(" summ:");
//    Serial.print(output[0] + output[1] + output[2]);
//    Serial.print("\n");
    analogWrite(red_led, map(output[0],0,255,0,map(amp,0,max_amp,0,255)));
    analogWrite(green_led, map(output[1],0,255,0,map(amp,0,max_amp,0,255)));
    analogWrite(blue_led, map(output[2],0,255,0,map(amp,0,max_amp,0,255)));
//    analogWrite(red_led, map(amp,0,max_amp,0,255));
//    analogWrite(green_led, map(amp,0,max_amp,0,255));
//    analogWrite(blue_led, map(amp,0,max_amp,0,255));
    //////////// readjustment /////////////////////////////////
    if (millis() >= readjust_counter) {
      //Serial.println("####### THE WEHRMACHT CLOSING IN #######");
      readjust_counter += readjust_time;
      max_amp = max(600,temp_max_amp);
      temp_max_amp = 0;
      min_freq = min(0,max_freq - 5);
      max_freq = max(0,max_freq - 5);
      //max_freq = max(60,temp_max_freq);
      //temp_max_amp = 0;
      //Serial.println(max_amp);
    }
  }
}
