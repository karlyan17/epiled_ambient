  int red_led = 2;          // - digital pins for your leds 
  int green_led = 4;        // - don't forget 1kOhm serial resistor!
  int blue_led = 7;         
  int audio_r = 0;          // - analog audio input (right + left)
  int max_volt = 300;       // - estimated maximal voltage in mV
  int amp = 0;              //   will be dynamically adjusted
  int main_intv = 100;      // - duration of wave sampling in ms
  int min_counter = 0;      // - counts ms to a full minute in ms
  int max_volt_collect = 0;
  int board_volt = 5000;    // - arduino board voltage in mV
void setup() {
  // put your setup code here, to run once:
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  analogReference(DEFAULT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  amp = 0;
  for(int i = 0; i < (main_intv*10); i++) {
    // each sampling step takes approx. 100 microseconds
    // real voltage is approx. U_real = (5*amp)mV
    amp = max(analogRead(audio_r),amp);
  }
  max_volt_collect = max(amp,max_volt_collect);
  digitalWrite(blue_led, LOW);
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);
  if (amp*(board_volt/max_volt) > 750) {
    digitalWrite(blue_led, HIGH);
    //delay(200 - 2*main_intv);
  }
  else if (amp*(board_volt/max_volt) > 600) {
    digitalWrite(green_led, HIGH);
    //delay(200 - 2*main_intv);
  }
  else if (amp*(board_volt/max_volt) > 200) {
    digitalWrite(red_led,HIGH);
    //delay(200 - 2*main_intv);
  }
  min_counter += (main_intv);      // - increase min_counter adjusting
  if (min_counter >= 10000) {          //   for the 10ms
    min_counter = 0;
    max_volt = max_volt_collect*5;
    max_volt_collect = 0;
  }
}
