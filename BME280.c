/*********************************************************************************
   Original author: Alexandr Pochtovy<alex.mail.prime@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
 	BME280.c
	Created on: 30.05.2021
 ***********************************************************************************/

#include "BME280.h"

static const uint8_t BME280_CHIP_ID = 0x60;	//BME280 I2C ID
static const uint8_t BME280_RESET_COMMAND = 0xB6;	//BME280 software reset
#define BME280_CONCAT_BYTES(msb, lsb)	(((uint16_t)msb << 8) | (uint16_t)lsb)
//INITIALIZATION	================================================================
/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id and calibration data from the sensor.
 */
BME_Connect_Status BME280_Init(I2C_Connection *_i2c, bme280_dev *dev, uint8_t *pbuffer) {
	if (_i2c->i2cStatus == I2C_Bus_Free) {//send setup
        _i2c->addr = dev->addr;
        _i2c->rxtxp = pbuffer;
        switch (dev->step) {
		case 0://setup humidity
			dev->status = BME_Init;
			_i2c->reg = BME280_REG_CTRL_HUM;
			_i2c->len = 1;
			_i2c->mode = I2C_MODE_WRITE;
            _i2c->rxtxp[0] = BME280_HUM_OVERSAMPLING_16X;
			dev->step = 1;
			break;

		case 1://setup mode temp pressure
			_i2c->reg = BME280_REG_CTRL_MEAS_PWR;
			_i2c->len = 2;
			_i2c->mode = I2C_MODE_WRITE;
			_i2c->rxtxp[0] = BME280_NORMAL_MODE | BME280_PRESS_OVERSAMPLING_16X | BME280_TEMP_OVERSAMPLING_16X;
			_i2c->rxtxp[1] = BME280_SPI_3WIRE_MODE_OFF | BME280_FILTER_COEFF_16 | BME280_STANDBY_TIME_20_MS;
			dev->step++;
			break;

		case 2://read calib temp data
			_i2c->reg = BME280_REG_T_P_CALIB_DATA;
			_i2c->len = BME280_T_P_CALIB_DATA_LEN;
			_i2c->mode = I2C_MODE_READ;
			dev->step++;
			break;

		case 3://read calib pressure data
			_i2c->rxtxp = pbuffer;
			parse_temp_press_calib_data(_i2c, dev);
			_i2c->reg = BME280_REG_HUM_CALIB_DATA;
			_i2c->len = BME280_HUM_CALIB_DATA_LEN;
			_i2c->mode = I2C_MODE_READ;
			dev->step++;
			break;

		case 4:
			parse_humidity_calib_data(_i2c, dev);
			dev->status = BME_OK;
            dev->step = 0;
			return BME_Complite;
			break;
		default:
			dev->step = 0;
            break;
		}
        I2C_Start_IRQ(_i2c);
	}
	return BME_Processing;
}

BME_Connect_Status BME280_GetData(I2C_Connection *_i2c, bme280_dev *dev, uint8_t *pbuffer) {
	if (_i2c->i2cStatus == I2C_Bus_Free) {//send setup
        _i2c->addr = dev->addr;
        _i2c->rxtxp = pbuffer;
        if (dev->step == 0) {
            _i2c->reg = BME280_REG_DATA;
            _i2c->len = BME280_DATA_LEN;
            _i2c->mode = I2C_MODE_READ;
            //_i2c->rxtxp = pbuffer;
            dev->step++;
        } else if (dev->step == 1) {
        	//_i2c->rxtxp = pbuffer;
        	bme280_parse_sensor_data(_i2c, dev);
            bme280_calculate_data_int(dev);
            bme280_calculate_data_float(dev);
            dev->step = 0;
            return BME_Complite;
        }
        I2C_Start_IRQ(_i2c);
	}
	return BME_Processing;
}
//CALCULATING	==========================================================================
/*!
 *  @brief This internal API is used to parse the temperature and
 *  pressure calibration data and store it in device structure.
 */
void parse_temp_press_calib_data(I2C_Connection *_i2c, bme280_dev *dev) {
	dev->calib_data.dig_t1 = BME280_CONCAT_BYTES(_i2c->rxtxp[1], _i2c->rxtxp[0]);
	dev->calib_data.dig_t2 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[3], _i2c->rxtxp[2]);
	dev->calib_data.dig_t3 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[5], _i2c->rxtxp[4]);
	dev->calib_data.dig_p1 = BME280_CONCAT_BYTES(_i2c->rxtxp[7], _i2c->rxtxp[6]);
	dev->calib_data.dig_p2 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[9], _i2c->rxtxp[8]);
	dev->calib_data.dig_p3 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[11], _i2c->rxtxp[10]);
	dev->calib_data.dig_p4 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[13], _i2c->rxtxp[12]);
	dev->calib_data.dig_p5 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[15], _i2c->rxtxp[14]);
	dev->calib_data.dig_p6 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[17], _i2c->rxtxp[16]);
	dev->calib_data.dig_p7 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[19], _i2c->rxtxp[18]);
	dev->calib_data.dig_p8 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[21], _i2c->rxtxp[20]);
	dev->calib_data.dig_p9 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[23], _i2c->rxtxp[22]);
	dev->calib_data.dig_h1 = _i2c->rxtxp[25];
}

void parse_humidity_calib_data(I2C_Connection *_i2c, bme280_dev *dev) {
    int16_t dig_h4_lsb;
    int16_t dig_h4_msb;
    int16_t dig_h5_lsb;
    int16_t dig_h5_msb;

    dev->calib_data.dig_h2 = (int16_t)BME280_CONCAT_BYTES(_i2c->rxtxp[1], _i2c->rxtxp[0]);
    dev->calib_data.dig_h3 = _i2c->rxtxp[2];
    dig_h4_msb = (int16_t)(int8_t)_i2c->rxtxp[3] * 16;
    dig_h4_lsb = (int16_t)(_i2c->rxtxp[4] & 0x0F);
    dev->calib_data.dig_h4 = dig_h4_msb | dig_h4_lsb;
    dig_h5_msb = (int16_t)(int8_t)_i2c->rxtxp[5] * 16;
    dig_h5_lsb = (int16_t)(_i2c->rxtxp[4] >> 4);
    dev->calib_data.dig_h5 = dig_h5_msb | dig_h5_lsb;
    dev->calib_data.dig_h6 = (int8_t)_i2c->rxtxp[6];
}

/*!
 *  @brief This API is used to parse the pressure, temperature and
 *  humidity data and store it in the bme280_uncomp_data structure instance.
 */
void bme280_parse_sensor_data(I2C_Connection *_i2c, bme280_dev *dev) {
    /* Variables to store the sensor data */
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;

    /* Store the parsed register values for pressure data */
    data_msb = (uint32_t)_i2c->rxtxp[0] << 12;
    data_lsb = (uint32_t)_i2c->rxtxp[1] << 4;
    data_xlsb = (uint32_t)_i2c->rxtxp[2] >> 4;
    dev->uncomp_data.pressure = data_msb | data_lsb | data_xlsb;

    /* Store the parsed register values for temperature data */
    data_msb = (uint32_t)_i2c->rxtxp[3] << 12;
    data_lsb = (uint32_t)_i2c->rxtxp[4] << 4;
    data_xlsb = (uint32_t)_i2c->rxtxp[5] >> 4;
    dev->uncomp_data.temperature = data_msb | data_lsb | data_xlsb;

    /* Store the parsed register values for humidity data */
    data_msb = (uint32_t)_i2c->rxtxp[6] << 8;
    data_lsb = (uint32_t)_i2c->rxtxp[7];
    dev->uncomp_data.humidity = data_msb | data_lsb;
}

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in integer data type.
 */
int32_t compensate_temperature_int(bme280_dev *dev) {
	int32_t var1;
	int32_t var2;
	int32_t temperature;
	int32_t temperature_min = -4000;
	int32_t temperature_max = 8500;

	var1 = (int32_t)((dev->uncomp_data.temperature / 8) - ((int32_t)dev->calib_data.dig_t1 * 2));
	var1 = (var1 * ((int32_t)dev->calib_data.dig_t2)) / 2048;
	var2 = (int32_t)((dev->uncomp_data.temperature / 16) - ((int32_t)dev->calib_data.dig_t1));
	var2 = (((var2 * var2) / 4096) * ((int32_t)dev->calib_data.dig_t3)) / 16384;
	dev->calib_data.t_fine = var1 + var2;
	temperature = (dev->calib_data.t_fine * 5 + 128) / 256;
	if (temperature < temperature_min) {
		temperature = temperature_min;
	}
	else if (temperature > temperature_max) {
		temperature = temperature_max;
	}
	return temperature;
}

float compensate_temperature_float(bme280_dev *dev) {
	float var1;
	float var2;
	float temperature;
	float temperature_min = -40;
	float temperature_max = 85;

  var1 = ((float)dev->uncomp_data.temperature) / 16384.0 - ((float)dev->calib_data.dig_t1) / 1024.0;
  var1 = var1 * ((float)dev->calib_data.dig_t2);
  var2 = (((float)dev->uncomp_data.temperature) / 131072.0 - ((float)dev->calib_data.dig_t1) / 8192.0);
  var2 = (var2 * var2) * ((double)dev->calib_data.dig_t3);
  dev->calib_data.t_fine = (int32_t)(var1 + var2);
  temperature = (var1 + var2) / 5120.0;

  if (temperature < temperature_min) {
  	temperature = temperature_min;
  }
  else if (temperature > temperature_max) {
    temperature = temperature_max;
  }
  return temperature;
}

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in integer data type.
 */
uint32_t compensate_pressure_int(bme280_dev *dev) {
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)dev->calib_data.t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)dev->calib_data.dig_p6);
    var2 = var2 + ((var1 * ((int32_t)dev->calib_data.dig_p5)) * 2);
    var2 = (var2 / 4) + (((int32_t)dev->calib_data.dig_p4) * 65536);
    var3 = (dev->calib_data.dig_p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)dev->calib_data.dig_p2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)dev->calib_data.dig_p1)) / 32768;
    /* avoid exception caused by division by zero */
    if (var1) {
        var5 = (uint32_t)((uint32_t)1048576) - dev->uncomp_data.pressure;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
        if (pressure < 0x80000000) {
            pressure = (pressure << 1) / ((uint32_t)var1);
        }
        else {
            pressure = (pressure / (uint32_t)var1) * 2;
        }
        var1 = (((int32_t)dev->calib_data.dig_p9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)dev->calib_data.dig_p8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + dev->calib_data.dig_p7) / 16));
        if (pressure < pressure_min) {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max) {
            pressure = pressure_max;
        }
    }
    else {
        pressure = pressure_min;
    }
    return pressure;
}

float compensate_pressure_float(bme280_dev *dev) {
	float var1;
	float var2;
	float var3;
	float pressure;
	float pressure_min = 30000.0;
	float pressure_max = 110000.0;

  var1 = ((float)dev->calib_data.t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((float)dev->calib_data.dig_p6) / 32768.0;
  var2 = var2 + var1 * ((float)dev->calib_data.dig_p5) * 2.0;
  var2 = (var2 / 4.0) + (((float)dev->calib_data.dig_p4) * 65536.0);
  var3 = ((float)dev->calib_data.dig_p3) * var1 * var1 / 524288.0;
  var1 = (var3 + ((float)dev->calib_data.dig_p2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((float)dev->calib_data.dig_p1);

    /* avoid exception caused by division by zero */
  if (var1 > (0.0)) {
  	pressure = 1048576.0 - (float) dev->uncomp_data.pressure;
  	pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
  	var1 = ((float)dev->calib_data.dig_p9) * pressure * pressure / 2147483648.0;
  	var2 = pressure * ((float)dev->calib_data.dig_p8) / 32768.0;
  	pressure = pressure + (var1 + var2 + ((float)dev->calib_data.dig_p7)) / 16.0;
  	if (pressure < pressure_min) {
  		pressure = pressure_min;
      	}
    else if (pressure > pressure_max) {
          pressure = pressure_max;
      }
  }
  else /* Invalid case */ {
      pressure = pressure_min;
  }
   return pressure;
}
/*!
 * @brief This internal API is used to compensate the raw humidity data and
 * return the compensated humidity data in integer data type.
 */
uint32_t compensate_humidity_int(bme280_dev *dev) {
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = dev->calib_data.t_fine - ((int32_t)76800);
    var2 = (int32_t)(dev->uncomp_data.humidity * 16384);
    var3 = (int32_t)(((int32_t)dev->calib_data.dig_h4) * 1048576);
    var4 = ((int32_t)dev->calib_data.dig_h5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)dev->calib_data.dig_h6)) / 1024;
    var3 = (var1 * ((int32_t)dev->calib_data.dig_h3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)dev->calib_data.dig_h2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)dev->calib_data.dig_h1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);
    if (humidity > humidity_max) {
        humidity = humidity_max;
    }
    return humidity;
}

float compensate_humidity_float(bme280_dev *dev) {
	float humidity;
	float humidity_min = 0.0;
	float humidity_max = 100.0;
	float var1;
	float var2;
	float var3;
	float var4;
	float var5;
	float var6;

  var1 = ((float)dev->calib_data.t_fine) - 76800.0;
  var2 = (((float)dev->calib_data.dig_h4) * 64.0 + (((float)dev->calib_data.dig_h5) / 16384.0) * var1);
  var3 = dev->uncomp_data.humidity - var2;
  var4 = ((float)dev->calib_data.dig_h2) / 65536.0;
  var5 = (1.0 + (((float)dev->calib_data.dig_h3) / 67108864.0) * var1);
  var6 = 1.0 + (((float)dev->calib_data.dig_h6) / 67108864.0) * var1 * var5;
  var6 = var3 * var4 * (var5 * var6);
  humidity = var6 * (1.0 - ((float)dev->calib_data.dig_h1) * var6 / 524288.0);
  if (humidity > humidity_max) {
  	humidity = humidity_max;
  }
  else if (humidity < humidity_min) {
  	humidity = humidity_min;
  }
  return humidity;
}

/*!
 * @brief This API is used to compensate the pressure and/or
 * temperature and/or humidity data according to the component selected
 * by the user.
 */
void bme280_calculate_data_int(bme280_dev *dev) {
	/* Compensate the temperature data */
	dev->data_int.temperature = compensate_temperature_int(dev);
	/* Compensate the pressure data */
	dev->data_int.pressure = compensate_pressure_int(dev);
		/* Compensate the humidity data */
	dev->data_int.humidity = compensate_humidity_int(dev);
}

void bme280_calculate_data_float(bme280_dev *dev) {
	/* Compensate the temperature data */
	dev->data_float.temperature = compensate_temperature_float(dev);
	/* Compensate the pressure data */
	dev->data_float.pressure = compensate_pressure_float(dev);
		/* Compensate the humidity data */
	dev->data_float.humidity = compensate_humidity_float(dev);
}
