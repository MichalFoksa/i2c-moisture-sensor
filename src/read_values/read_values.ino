/*
** This example uses F Malpartida's NewLiquidCrystal library. Obtain from:
** https://bitbucket.org/fmalpartida/new-liquidcrystal
** Modified - Ian Brennan ianbren at hotmail.com 23-10-2012 to support Tutorial posted to Arduino.cc
** Written for and tested with Arduino 1.0
**
** NOTE: Tested on Arduino Uno whose I2C pins are A4==SDA, A5==SCL

*/
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LCD_I2C_ADDR    0x3F // LCD display I2C address.
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C	lcd(LCD_I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

int moisture_sensor_count = 0 ;
// Add all your moisture sensors i2c addresses into moisture_i2c_addrs array
int moisture_i2c_addrs[] = { 0x20 , 0x21 } ;

// Print buffer
char buffer [32] ;
int line = 0 ;

void writeI2CRegister8bit(int addr, int value) {
	Wire.beginTransmission(addr);
	Wire.write(value);
	Wire.endTransmission();
}

unsigned int readI2CRegister16bit(int addr, int reg) {
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.endTransmission();
	delay(20);
	Wire.requestFrom(addr, 2);
	unsigned int t = Wire.read() << 8;
	t = t | Wire.read();
	return t;
}

void setup() {
	Wire.begin();
	Serial.begin(9600);
	
	lcd.begin (20,4); //  <<----- My LCD was 20x4

	// Switch on the backlight
	lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
	lcd.setBacklight(HIGH);
	lcd.home (); // go home

	moisture_sensor_count = sizeof (moisture_i2c_addrs) / sizeof(moisture_sensor_count) ;
	
	
	Serial.println ( moisture_sensor_count ) ;

	for ( int i = 0 ; i < moisture_sensor_count ; i++) {
		writeI2CRegister8bit(moisture_i2c_addrs[i], 6); //reset	
	}
	
	delay(500) ;
}

void loop() {
	
	for ( int i = 0 ; i < moisture_sensor_count ; i++) {
		int cap = readI2CRegister16bit(moisture_i2c_addrs[i], 0); //read capacitance register
		int temp = readI2CRegister16bit(moisture_i2c_addrs[i], 5); //temperature register
		writeI2CRegister8bit(moisture_i2c_addrs[i], 3); //request light measurement
		int light = readI2CRegister16bit(moisture_i2c_addrs[i], 4); //read light register

		sprintf( buffer , "C:%3d T:%3d L:%4d " , cap , temp , light ) ;

		Serial.print("0x") ;
		Serial.print(moisture_i2c_addrs[i] ,  HEX);
		Serial.print(": ");
		Serial.println(buffer);

		if ( moisture_sensor_count == 1 ) {
			lcd.setCursor (0,(line++%4));			
			sprintf( buffer , "C:%3d T:%3d L:%4d " , cap , temp , light ) ;			
		} else {
			lcd.setCursor (0,(i));
			sprintf( buffer , "%d C:%3d T:%3d L:%4d " , i+1 ,cap , temp , light / 10 ) ;
		}
		lcd.print( buffer ) ;
		delay (200) ;
	}

	delay (500) ;
}
