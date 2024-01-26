#include "HT16K33.h"

/*!
*	@brief Class constructor
*	@param address the device's I2C address which can be 0x70 .. 0x77
*	@param wire	   the I2C Wire object instance
*/
HT16K33::HT16K33(const uint8_t address, TwoWire *wire){
	_address      = address;
	_wire         = wire;
	_lastTime     = 0;
	_currentFrame = 0;
	_running      = false;
	memset(_databuffer, 0, sizeof(_databuffer)); // Set all data buffer to 0 
}

/*!
*	@brief  Method that starts the I2C device and initializes some default display state settings
*	@return true if successful, otherwise false
*/
bool HT16K33::begin(){
	if (!isConnected()) return false;
  	reset();
  	return true;
}

/*!
*	@brief Method that sets the current animation from the 2D array pointer passed
*	@param animation The 2D array pointer that points to the animation (each frame is 8x16 bits)
*	@param frames    The amount of frames that the selected animation is composed of
*/
void HT16K33::setAnimation(const uint16_t (*animation)[8],  uint8_t frames){
	_animation    = animation;
	_frames    	  = frames / 8;
	_currentFrame = 0;
	_running      = true;
}

/*!
*	@brief Method that sets the framerate of the animation
*	@param framerate the framerate amount to be set in FPS (Hz)
*/
void HT16K33::setFramerate(uint8_t framerate){
	_frameInterval = (1.0f / framerate) * 1000;
}

/*!
*	@brief Main loop method that iterates through the selected animation, must be called every loop
*/
void HT16K33::loop(){

	if(!_running) return;

	uint32_t currentTime = millis();

	if (currentTime - _lastTime >= _frameInterval){
		_lastTime = currentTime;

		drawImage(_animation[_currentFrame]);

		_currentFrame++;
	}

	if(_currentFrame > _frames) {
		_running = false;
		clear();
		writeDisplay();
	}
}

/*!
*	@brief Method that loads a single pixel to the display buffer
*	@param x     x coordinate
*	@param y     y coordinate
*	@param state led state: true = on, false = off
*/
void HT16K33::drawPixel(int16_t x, int16_t y, uint16_t state){
	if ((y < 0) || (x < 0) || (y >= 16) || (x >= 8)) return;
	if (y >= 8){ x += 8; y -= 8; }

	if (state) _databuffer[y] |=   1 << x;
	else 	   _databuffer[y] &= ~(1 << x);
}

/*!
*	@brief Method that draws a 8x16 bits image, each bit either being on or off (0 or 1)
*	@param bitmap  8 element array with each row being 16 bits
*/
void HT16K33::drawImage(const uint16_t bitmap[]){
	for (int16_t i = 0; i < 8; i++){
		for (int16_t j = 0; j < 16; j++){
			 drawPixel(i, j, bitRead(bitmap[i], 15-j));
		}
	}
	writeDisplay();
}

/*!
*	@brief Clears the display buffer data
*/
void HT16K33::clear(){
	memset(_databuffer, 0, sizeof(_databuffer));
}

/*!
*	@brief Turn display on or off
*	@param state  display state: true = on, false = off
*/
void HT16K33::setDisplayState(bool state){
	uint8_t buffer;
	if (state) buffer = HT16K33_BLINK_CMD | 1;
	else 	   buffer = HT16K33_BLINK_CMD;
	
	writeData(&buffer, 1);
}

/*!
*	@brief Set display brightness.
*	@param brightness  0 (min) to 15 (max).
*/
void HT16K33::setBrightness(uint8_t brightness){
	if (brightness > 15) brightness = 15; // limit to max brightness

	uint8_t buffer = HT16K33_CMD_BRIGHTNESS | brightness;
	writeData(&buffer, 1);
}

/*!
*	@brief Set display blink rate.
*	@param rate  can be either: steady ON / OFF or 2 / 1 / 0.5 HZ
*/
void HT16K33::setBlinkRate(uint8_t rate){
	if (rate > 3) rate = 0; // turn off if illegal value

	uint8_t buffer = HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (rate << 1);
	writeData(&buffer, 1);
}

/*!
*	@brief Loads the buffered data to the I2C device
*/
void HT16K33::writeDisplay(){
	uint8_t buffer[17];
	buffer[0] = 0x00;

	for (uint8_t i = 0; i < 8; i++){
		buffer[1 + 2 * i] = lowByte(_databuffer[i]);
		buffer[2 + 2 * i] = highByte(_databuffer[i]);
	}
	writeData(buffer, 17);
}

// ---------------------------------------------------- P R I V A T E S --------------------------------------------------------------

/*!
*	@brief  Checks if the device is connected
*	@return Returns false if it fails
*/
bool HT16K33::isConnected(){
  _wire->beginTransmission(_address);
  return (_wire->endTransmission() == 0);
}

/*!
*	@brief Initializes the device, clears garbage values and sets some default settings 
*/
void HT16K33::reset(){
	uint8_t buffer[1] = {HT16K33_TURN_OSCILLATOR}; // turns on the oscillator
	writeData(buffer, 1);

	writeDisplay(); // Clears display as all buffer data is set as 0 by default

	setBlinkRate(HT16K33_BLINK_OFF);
	setBrightness(15); // max brightness
}

/*!
*	@brief Sends the data stored in the buffer to the I2C device
*	@param data 	data buffer that stores the data that will get sent
*	@param quantity	the amoung of data that will get sent
*/
void HT16K33::writeData(const uint8_t *data, size_t quantity){
	_wire->beginTransmission(_address);
	_wire->write(data, quantity);
	_wire->endTransmission();
}