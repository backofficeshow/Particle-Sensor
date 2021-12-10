//TBOS Particle Sensor Code for video!

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*!
 * @file gainConcentration.ino
 * @brief 获取此时空气中颗粒物的浓度
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      PengKaixing(kaixing.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-11-23
 * @url https://github.com/dfrobot/DFRobot_AirQualitySensor
 */
#include "DFRobot_AirQualitySensor.h"

#define I2C_ADDRESS    0x19
  DFRobot_AirQualitySensor particle(&Wire ,I2C_ADDRESS);

void setup(){
  Serial.begin(115200);

  //传感器初始化，用作初始化IIC，由此时使用的通信方式来决定
  while(!particle.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  }
  Serial.println("sensor begin success!");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
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
  Serial.println();

  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("PM1.0:" + String(PM1_0) + " ug/m3");
  display.setCursor(10, 10);
  display.println("PM2.5:" + String(PM2_5) + " ug/m3");
  display.setCursor(10, 20);
  display.println("PM 10:" + String(PM10) + " ug/m3");    
  display.display();      // Show text
  delay(1000);
}
