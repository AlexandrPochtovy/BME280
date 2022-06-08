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
   
 	BME280.h
	Created on: 30.05.2021
 ***********************************************************************************/

#ifndef BME280_MASTER_BME280_H_
#define BME280_MASTER_BME280_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "I2C_Master/MyI2C.h"
#include "BME280_Registers.h"
//===========================================================================================
//#define BME280_CONCAT_BYTES(msb, lsb)	(((uint16_t)msb << 8) | (uint16_t)lsb)
/*!
 * @brief Calibration data
 */
typedef struct bme280_calib_data_t {
		uint16_t dig_t1;	// Calibration coefficient for the temperature sensor
		int16_t dig_t2; 	// Calibration coefficient for the temperature sensor
		int16_t dig_t3;		// Calibration coefficient for the temperature sensor
		uint16_t dig_p1;	// Calibration coefficient for the pressure sensor
		int16_t dig_p2;		// Calibration coefficient for the pressure sensor
		int16_t dig_p3;		// Calibration coefficient for the pressure sensor
		int16_t dig_p4;		// Calibration coefficient for the pressure sensor
		int16_t dig_p5;		// Calibration coefficient for the pressure sensor
		int16_t dig_p6;		// Calibration coefficient for the pressure sensor
		int16_t dig_p7;		// Calibration coefficient for the pressure sensor
		int16_t dig_p8;		// Calibration coefficient for the pressure sensor
		int16_t dig_p9;		// Calibration coefficient for the pressure sensor
		uint8_t dig_h1;		// Calibration coefficient for the humidity sensor
		int16_t dig_h2;		// Calibration coefficient for the humidity sensor
		uint8_t dig_h3;		// Calibration coefficient for the humidity sensor
		int16_t dig_h4;		// Calibration coefficient for the humidity sensor
		int16_t dig_h5;		// Calibration coefficient for the humidity sensor
		int8_t dig_h6;		// Calibration coefficient for the humidity sensor
		int32_t t_fine;		// Variable to store the intermediate temperature coefficient
} bme280_calib_data;

/*!
 * @brief bme280 sensor structure which comprises of uncompensated temperature,
 * pressure and humidity data
 */
typedef struct bme280_uncomp_data_t {
	uint32_t pressure;		// un-compensated pressure
	uint32_t temperature;	// un-compensated temperature
	uint32_t humidity; 		// un-compensated humidity
} bme280_uncomp_data;

typedef struct bme280_data_int_t {
		uint32_t pressure; 		// Compensated pressure
		int32_t temperature;	// Compensated temperature
		uint32_t humidity;		// Compensated humidity
} bme280_data_int;

/*!
 * @brief bme280 sensor structure which comprises of temperature, pressure and
 * humidity data
 */
typedef struct bme280_data_float_t {
		float pressure; 		// Compensated pressure
		float temperature;	// Compensated temperature
		float humidity;			// Compensated humidity
} bme280_data_float;

/* состояние процесса обмена данными с устройством как с отдельным элементом сети
 * 	применяется для отображения состояния процесса работы с устройством для главного кода
 */
typedef enum BME_status_t {//состояние устройства
	BME_Init,		//устройство не настроено
	BME_OK,		//устройство готово к опросу
	BME_Faulth	//устройство неисправно
} BME_status;

/*	состояние обмена данными с устройством, использовать для завершения функции работы с устройством */
typedef enum BME_Connect_Status_t {
	BME_Processing, //выполняется работа с устройством: обмен данными, обработка результатов
	BME_Complite	//работа с устройством завершена, данные считаны/записаны корректно
} BME_Connect_Status;

//common data struct for sensor
typedef struct bme280_dev_t {
		uint8_t addr;
		uint8_t step;
		BME_status status;
		bme280_calib_data calib_data;
		bme280_uncomp_data uncomp_data;
		bme280_data_int data_int;
		bme280_data_float data_float;
} bme280_dev;

//INITIALIZATION	================================================================
BME_Connect_Status BME280_Init(I2C_Connection *_i2c, bme280_dev *dev, uint8_t *pbuffer);
BME_Connect_Status BME280_GetData(I2C_Connection *_i2c, bme280_dev *dev, uint8_t *pbuffer);
//CALCULATING	==========================================================================
void parse_temp_press_calib_data(I2C_Connection *_i2c, bme280_dev *dev);
void parse_humidity_calib_data(I2C_Connection *_i2c, bme280_dev *dev);
void bme280_parse_sensor_data(I2C_Connection *_i2c, bme280_dev *dev);
int32_t compensate_temperature_int(bme280_dev *dev);
float compensate_temperature_float(bme280_dev *dev);
uint32_t compensate_pressure_int(bme280_dev *dev);
float compensate_pressure_float(bme280_dev *dev);
uint32_t compensate_humidity_int(bme280_dev *dev);
float compensate_humidity_float(bme280_dev *dev);

void bme280_calculate_data_int(bme280_dev *dev);
void bme280_calculate_data_float(bme280_dev *dev);

#ifdef __cplusplus
}
#endif
#endif /* BME280_MASTER_BME_H_ */
