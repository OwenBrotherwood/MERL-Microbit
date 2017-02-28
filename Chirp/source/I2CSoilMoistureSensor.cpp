/*----------------------------------------------------------------------*
 * I2CSoilMoistureSensor.cpp - Arduino library for the Sensor version of*
 * I2C Soil Moisture Sensor version from Chrirp                         *
 * (https://github.com/Miceuz/i2c-moisture-sensor).                     *
 *                                                                      *
 * Ingo Fischer 11Nov2015                                               *
 * https://github.com/Apollon77/I2CSoilMoistureSensor                   *
 *                                                                      *
 * MIT license                                                          *
 *----------------------------------------------------------------------*/ 

#include "I2CSoilMoistureSensor.h"
extern MicroBit uBit;


/*----------------------------------------------------------------------*
 * Constructor.                                                         *
 * Optionally set sensor I2C address if different from default          *
 *----------------------------------------------------------------------*/
I2CSoilMoistureSensor::I2CSoilMoistureSensor(uint8_t addr) : sensorAddress(addr) {
  // nothing to do 
}
  
/*----------------------------------------------------------------------*
 * Initializes anything ... it does a reset.                            *
 * When used without parameter or parameter value is false then a       *
 * waiting time of at least 1 second is expected to give the sensor     *
 * some time to boot up.                                                *
 * Alternatively use true as parameter and the method waits for a       *
 * second and returns after that.                                       *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::begin(bool wait) {
  resetSensor();
  if (wait) {
    uBit.sleep(1000);
  }
}

/*----------------------------------------------------------------------*
 * Return measured Soil Moisture Capacitance                            *
 * Moisture is somewhat linear. More moisture will give you higher      *
 * reading. Normally all sensors give about 290 - 310 as value in free  * 
 * air at 5V supply.                                                    *
 *----------------------------------------------------------------------*/
unsigned int I2CSoilMoistureSensor::getCapacitance() {
  return readI2CRegister16bitUnsigned(sensorAddress, SOILMOISTURESENSOR_GET_CAPACITANCE);
}

/*----------------------------------------------------------------------*
 * Change I2C address of the sensor to the provided address (1..127)    *
 * and do a reset after it in order for the new address to become       *
 * effective if second parameter is true.                               *
 * Method returns true if the new address is set successfully on sensor.*
 *----------------------------------------------------------------------*/
bool I2CSoilMoistureSensor::setAddress(int addr, bool reset) {
  writeI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_SET_ADDRESS, addr);
  if (reset) {
	resetSensor();
    uBit.sleep(1000);
  }
  sensorAddress=addr;
  return (readI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_GET_ADDRESS) == addr);
}

/*----------------------------------------------------------------------*
 * Change the address (1..127) this instance is trying to read from     *
 * and do a reset after to initialize.                                  *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::changeSensor(int addr, bool wait) {
  sensorAddress=addr;
  begin(wait);
}

/*----------------------------------------------------------------------*
 * Return current Address of the Sensor                                 *
 *----------------------------------------------------------------------*/
uint8_t I2CSoilMoistureSensor::getAddress() {
  return sensorAddress;
}

/*----------------------------------------------------------------------*
 * Starts the measurement for the Light sensor. Wait at least 3 seconds *
 * till you call method getLight to get the Light value.                *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::startMeasureLight() {
  writeI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_MEASURE_LIGHT);
}

/*----------------------------------------------------------------------*
 * Read the Light Measurement from the sensor. When used without        *
 * parameter or parameter value is false then a former call to          *
 * method startMeasureLight and a waiting time of at least 3 seconds is *
 * expected.                                                            *
 * Alternatively use true as parameter and the method does the call to  *
 * startMeasureLight and a 3 seconds delay automatically and no former  *
 * call is needed.                                                      *
 * The measurement gives 65535 in a dark room away form desk lamp - so  *
 * more light, lower reading. When it's dark, it takes longer to        *
 * measure light, reading the light register while measurement is in    *
 * progress (e.g. wait time too short) will return the previous reading.*
 * Be aware, light sensor is pretty noisy.                              *
 *----------------------------------------------------------------------*/
unsigned int I2CSoilMoistureSensor::getLight(bool wait) {
  if (wait) {
    startMeasureLight();
    uBit.sleep(3000);
  }
  return readI2CRegister16bitUnsigned(sensorAddress, SOILMOISTURESENSOR_GET_LIGHT);
}

/*----------------------------------------------------------------------*
 * Read the Temperature Measurement. Temperature is measured by the     *
 * thermistor on the tip of the sensor. Calculated absolute measurement *
 * accuracy is better than 2%. The returned value is in degrees Celsius *
 * with factor 10, so need to divide by 10 to get real value            *
 *----------------------------------------------------------------------*/
int I2CSoilMoistureSensor::getTemperature() {
  return readI2CRegister16bitSigned(sensorAddress, SOILMOISTURESENSOR_GET_TEMPERATURE);
}

/*----------------------------------------------------------------------*
 * Resets sensor. Give the sensor 0.5-1 second time to boot up after    *
 * reset.                                                               *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::resetSensor() {
  writeI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_RESET);
}

/*----------------------------------------------------------------------*
 * Get Firmware Version. 0x22 means 2.2                                 *
 *----------------------------------------------------------------------*/
uint8_t I2CSoilMoistureSensor::getVersion() {
  return readI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_GET_VERSION);
}

/*----------------------------------------------------------------------*
 * Sleep sensor. Initiates SLEEP_MODE_PWR_DOWN in the sensor's MCU.     *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::sleep() {
  writeI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_SLEEP);
}

/*----------------------------------------------------------------------*
 * Check if sensor is busy. Returns true if a measurement is running.   *
 *----------------------------------------------------------------------*/
bool I2CSoilMoistureSensor::isBusy() {
  return (readI2CRegister8bit(sensorAddress, SOILMOISTURESENSOR_GET_BUSY) == 1);
}

/*----------------------------------------------------------------------*
 * Helper method to write an 8 bit value to the sensor via I2C          *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::writeI2CRegister8bit(int addr, int value) {
	
	uBit.i2c.write(addr, (char*) &value, 1);

}

/*----------------------------------------------------------------------*
 * Helper method to write an 8 bit value to the sensor via I2C to the   *
 * given register                                                       *
 *----------------------------------------------------------------------*/
void I2CSoilMoistureSensor::writeI2CRegister8bit(int addr, int reg, int value) {
	
	char i2creg[2];
	i2creg[0] = reg;
	i2creg[1] = value; 
	uBit.i2c.write(addr, (char*) &i2creg,2);
}

/*----------------------------------------------------------------------*
 * Helper method to read a 16 bit unsigned value from the given register*
 *----------------------------------------------------------------------*/
unsigned int I2CSoilMoistureSensor::readI2CRegister16bitUnsigned(int addr, int reg) {
	
	char i2creg = reg;
	char i2cval[2];
	uint16_t x;
	uBit.i2c.write(addr, (char*) &i2creg,1, true);
	uBit.i2c.read(addr, (char*) &i2cval, 2);
	x = (i2cval[0] << 8) |  i2cval[1];
	return x;

}

/*----------------------------------------------------------------------*
 * Helper method to read a 16 bit signed value from the given register*
 *----------------------------------------------------------------------*/
int I2CSoilMoistureSensor::readI2CRegister16bitSigned(int addr, int reg) {
	
	
	char i2creg = reg;
	char i2cval[2];
	int16_t x;
	uBit.i2c.write(addr, (char*) &i2creg,1, true);
	uBit.i2c.read(addr, (char*) &i2cval, 2);
	x = (i2cval[0] << 8) |  i2cval[1];
	return x;

}

/*----------------------------------------------------------------------*
 * Helper method to read a 8 bit value from the given register          *
 *----------------------------------------------------------------------*/
uint8_t I2CSoilMoistureSensor::readI2CRegister8bit(int addr, int reg) {
	
	char i2creg = reg;
	char i2cval;
	uBit.i2c.write(addr, (char*) &i2creg,1, true);
	uBit.sleep(100);
	uBit.i2c.read(addr, (char*) &i2cval, 1);
	return i2cval;

}
