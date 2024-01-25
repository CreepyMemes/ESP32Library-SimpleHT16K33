#pragma once

#include <Arduino.h>
#include <Wire.h>

#define HT16K33_BLINK_CMD 		0x80 // I2C register for BLINK setting
#define HT16K33_TURN_OSCILLATOR 0x21 // I2C value to turn on oscillator
#define HT16K33_BLINK_DISPLAYON 0x01 // I2C value for steady on
#define HT16K33_CMD_BRIGHTNESS  0xE0 // I2C register for BRIGHTNESS setting
#define HT16K33_BLINK_OFF       0	 // I2C value for steady off
#define HT16K33_BLINK_2HZ       1	 // I2C value for 2 Hz blink
#define HT16K33_BLINK_1HZ       2	 // I2C value for 1 Hz blink
#define HT16K33_BLINK_HALFHZ    3	 // I2C value for 0.5 Hz blink

/*!
	@brief  Class driver for an 8x16 Led matrix with HT16K33 chip.
*/
class HT16K33{

	public:	
		HT16K33(const uint8_t address, TwoWire *wire = &Wire);

		bool 	 begin();

		void 	 setAnimation(const uint16_t (*animation)[8], uint8_t frames);
		void 	 setFramerate(uint8_t framerate);
		void 	 loop();

		void 	 drawPixel(int16_t x, int16_t y, uint16_t state);		
		void 	 drawBitmap(const uint16_t  bitmap[]);
		void 	 clear();

		void 	 setDisplayState(bool state);
		void 	 setBrightness(uint8_t brightness);
		void 	 setBlinkRate(uint8_t rate);
		
		void 	 writeDisplay();

		bool     isRunning() {return _running;}
		
	private:
		bool 	 isConnected(); 
		void 	 reset();
		
		void 	 writeData(const uint8_t *data, size_t quantity);

		uint8_t  _address;		 // The I2C device's address
		uint16_t _databuffer[8]; // Raw display buffer data
		TwoWire* _wire; 		 // Pointer to the I2C bus interface

		uint8_t  _frames;		 // The amount of frames the animation has
		uint8_t  _currentFrame;  // The current frame index
		uint16_t _frameInterval; // The frame interval between each frame, (calculated from FPS)
		uint32_t _lastTime;	     // The last time the function millis() has ben called here (used to iterate through animation frames)
		bool	 _running;	     // Variable that tells if the display is currently going through an animation

		const uint16_t  (*_animation)[8]; // The main 2D array pointer that points to the selected animation
}; 