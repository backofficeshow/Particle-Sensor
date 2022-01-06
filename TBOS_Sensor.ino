//TBOS Particle Sensor Code for video!

#define hysteresis 3 //The gap at which the fan will turn off, i.e. if trigger is 10 fan will turn on, but needs to be 7 to turn off

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include "RotaryEncoder.h"
#define RTRY_ENC_CLK A2
#define RTRY_ENC_DT A3
RotaryEncoder encoder(RTRY_ENC_CLK, RTRY_ENC_DT);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "DFRobot_AirQualitySensor.h"

#define I2C_ADDRESS    0x19
DFRobot_AirQualitySensor particle(&Wire ,I2C_ADDRESS);

ISR(PCINT1_vect) {
  encoder.tick();
}

static int triggerval = 0;

void setup(){
  Serial.begin(115200);
  Serial.println("Here we go!");

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH); //Fan output pin

  EEPROM.get(0x00, triggerval); //Read the value of the preset trigger from EEPROM

  Serial.println(triggerval);
  encoder.setPosition(triggerval);

  Serial.println("Encoder Set");

  PCICR |= (1 << PCIE1); // enable pin change interrupts
  PCMSK1 |= (1 << PCINT10); // enable PCI for pin A2
  PCMSK1 |= (1 << PCINT11); // enable PCI for pin A3

  Serial.println("Interupt Set");
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("Air Quality Switch");
  display.setCursor(10, 10);
  display.println("@backofficeshow");
  display.setCursor(10, 20);
  display.println("PM2.5 Trg:" + String(triggerval) + " ug/m3");     
  display.display();

  Serial.println("Screen Set");

  while(!particle.begin()){
    Serial.println("NO Devices !");
    delay(1000);
  }
  Serial.println("Air Quality Sensor Running!");

  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  //获取传感器版本号
  uint8_t version = particle.gainVersion();
  Serial.print("version is : ");
  Serial.println(version);
  delay(1000);
}

void loop() {
    static int coolDown = 0; //The Delay on the display
/**
 *@brief : Get concentration of PM1.0 PM2.5 PM10
 *@param :PARTICLE_PM1_0_STANDARD  Standard particle  
          PARTICLE_PM2_5_STANDARD  Standard particle  
          PARTICLE_PM10_STANDARD   Standard particle 
          PARTICLE_PM1_0_ATMOSPHERE  In atmospheric environment
          PARTICLE_PM2_5_ATMOSPHERE  In atmospheric environment
          PARTICLE_PM10_ATMOSPHERE   In atmospheric environment
*/  
  uint16_t PM2_5 = particle.gainParticleConcentration_ugm3(PARTICLE_PM2_5_STANDARD);
  uint16_t PM1_0 = particle.gainParticleConcentration_ugm3(PARTICLE_PM1_0_STANDARD);
  uint16_t PM10 = particle.gainParticleConcentration_ugm3(PARTICLE_PM10_STANDARD);
  Serial.print("PM2.5 concentration:");
  Serial.print(PM2_5);
  Serial.println(" ug/m3");
  Serial.print("PM1.0 concentration:");
  Serial.print(PM1_0);
  Serial.println(" ug/m3");
  Serial.print("PM10 concentration:");
  Serial.print(PM10);
  Serial.println(" ug/m3");
  Serial.print("Trigger Value:");
  Serial.print(triggerval);
  Serial.println(" ug/m3");

  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  int newPos = encoder.getPosition();
  if (triggerval != newPos) {
    triggerval = newPos;

    if (triggerval < 0){
      encoder.setPosition(0);
      triggerval = 0;
    }
    coolDown = 50;
  }

  if (coolDown == 1){
    //Store the requested value for boot time
    EEPROM.put(0x00, triggerval);
  }

  if (coolDown){
    display.setCursor(10, 0);
    display.println("PM2.5 Trg:" + String(triggerval) + " ug/m3");
    coolDown--;
  }else{
    display.setCursor(10, 0);
    display.println("PM1.0:" + String(PM1_0) + " ug/m3");
    display.setCursor(10, 10);
    display.println("PM2.5:" + String(PM2_5) + " ug/m3");
    display.setCursor(10, 20);
    display.println("PM 10:" + String(PM10) + " ug/m3");   
  } // if

  if (PM2_5 >= triggerval){
    digitalWrite(7, LOW);
  }else if (PM2_5 < triggerval - hysteresis || triggerval == 0){
    digitalWrite(7, HIGH);
  }
  

  display.display();      // Show text

}
