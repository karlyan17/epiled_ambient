#include <FHT.h>
  
  int red_led = 11;          // - digital pins for your leds 
  int green_led = 10;        // - don't forget 1kOhm serial resistor!
  int blue_led = 9;         
  int audio_r = 0;          // - analog audio input (right + left)
  int max_volt = 200;       // - estimated maximal voltage in A.U.
  int amp = 0;              //   will be dynamically adjusted
  int main_intv = 50;      // - duration of wave sampling in ms
  int min_counter = 0;      // - counts ms to a full minute in ms
  int max_volt_collect = 0;
//  int board_volt = 5000;    // - arduino board voltage in mV
void setup() {
  // put your setup code here, to run once:
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  analogReference(DEFAULT);
  Serial.begin(9600);
  Serial.println("Serial port open on 9600 with ln");
}

void loop() {
  // time space amplitude part --- use for brightness
  noInterrupts();
  amp = 0;
  for(int i = 0; i < (main_intv*10); i++) {
    // each sampling step takes approx. 100 microseconds
    // real voltage is approx. U_real = (5*amp)mV
    amp = max(analogRead(audio_r),amp);
  }
  max_volt_collect = max(amp,max_volt_collect);
  Serial.print(max_volt);
  Serial.print("|");
  Serial.print(max_volt_collect);
  Serial.print("| amp: ");
  for (int t = 0; t < amp; t++) {
    Serial.print("#");
  }
  Serial.print("\n");
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
  analogWrite(red_led, map(amp,0,max_volt,0,255));
  analogWrite(green_led,map(amp,0,max_volt,0,255));
  analogWrite(blue_led, map(amp,0,max_volt,0,255));
  min_counter += (main_intv);      // - increase min_counter adjusting
  if (min_counter >= 5000) {          //   for the 10ms
    min_counter = 0;
    max_volt = max(10,max_volt_collect);
    max_volt_collect = 0;
  }
}
