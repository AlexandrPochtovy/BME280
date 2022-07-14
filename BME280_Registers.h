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
   
 	BME280_Registers.h
	Created on: 30.05.2021
 ***********************************************************************************/
 
#ifndef BME280_BME280_REGISTERS_H_
#define BME280_BME280_REGISTERS_H_

//=============================================================
//calib & data size
enum BME280_Len {
	BME280_T_P_CALIB_DATA_LEN	= 26,
	BME280_HUM_CALIB_DATA_LEN	= 16,
	BME280_DATA_LEN				= 8
};
//BME280 registers address---------------------------------------------------------------------
enum BME280_REG {
	BME280_REG_T_P_CALIB_DATA	= 0x88,	//calibration data 26 bytes
	BME280_REG_CHIP_ID			= 0xD0,	//BME280 ID REGISTER
	BME280_REG_RESET			= 0xE0,	//SOFTWARE RESET WRITE 0xB6 for reset chip
	BME280_REG_HUM_CALIB_DATA	= 0xE1,	//16 bytes
	BME280_REG_CTRL_HUM			= 0xF2,	//bits for setup osrs hum
	BME280_REG_STATUS			= 0xF3,	//status register
	BME280_REG_CTRL_MEAS_PWR	= 0xF4,
	BME280_REG_CFG				= 0xF5,
	BME280_REG_DATA				= 0xF7,	//8 bytes dat
	BME280_CHIP_ID 				= 0x60,	//BME280 I2C ID
	BME280_RESET_COMMAND 		= 0xB6	//BME280 software reset
};
//-------------------------------------------------------------------------------------
//	MASK for select mode sensor	concat to OR in byte
//Register 0xF2 “ctrl_hum” Sensor oversampling HUMIDITY
enum BME280_HUM_OVERSAMPLING {
	BME280_HUM_OVERSAMPLING_OFF	= 0x00,	//xxxx x000
	BME280_HUM_OVERSAMPLING_1X	= 0x01,	//xxxx x001
	BME280_HUM_OVERSAMPLING_2X	= 0x02,	//xxxx x010
	BME280_HUM_OVERSAMPLING_4X	= 0x03,	//xxxx x011
	BME280_HUM_OVERSAMPLING_8X	= 0x04,	//xxxx x100
	BME280_HUM_OVERSAMPLING_16X	= 0x05		//xxxx x101
};
//-------------------------------------------------------------------------------------
//Register 0xF3 'status' mask
enum BME280_STATUS {
	BME280_STATUS_IM_UPDATE		= 0x01,	//xxxx xxx1
	BME280_STATUS_IS_MEASURE	= 0x08		//xxxx 1xxx
};
//-------------------------------------------------------------------------------------
//Register 0xF4 "ctrl_meas" mask
//Sensor power mode
enum BME280_MODE {
	BME280_SLEEP_MODE			= 0x00,	//xxxx xx00
	BME280_FORCED_MODE		=	0x01,	//xxxx xx01 OR xxxx xx10
	BME280_NORMAL_MODE		= 0x03		//xxxx xx11
};
//pressure oversampling
enum BME280_PRESS_OVERSAMPLING {
	BME280_PRESS_OVERSAMPLING_OFF	= 0x00,	//xxx0 00xx
	BME280_PRESS_OVERSAMPLING_1X		= 0x04,	//xxx0 01xx
	BME280_PRESS_OVERSAMPLING_2X		= 0x08,	//xxx0 10xx
	BME280_PRESS_OVERSAMPLING_4X		= 0x0C,	//xxx0 11xx
	BME280_PRESS_OVERSAMPLING_8X		= 0x10,	//xxx1 00xx
	BME280_PRESS_OVERSAMPLING_16X	= 0x14	//xxx1 01xx
};
//TEMPERATURE oversampling
enum BME280_TEMP_OVERSAMPLING {
	BME280_TEMP_OVERSAMPLING_OFF	= 0x00,	//000x xxxx
	BME280_TEMP_OVERSAMPLING_1X		=	0x20,	//001x xxxx
	BME280_TEMP_OVERSAMPLING_2X		=	0x40,	//010x xxxx
	BME280_TEMP_OVERSAMPLING_4X		=	0x60,	//011x xxxx
	BME280_TEMP_OVERSAMPLING_8X		=	0x80,	//100x xxxx
	BME280_TEMP_OVERSAMPLING_16X	= 0xA0	//101x xxxx
};
//-------------------------------------------------------------------------------------
//Register Register 0xF5 "config"
//SPI wire setup
enum BME280_SPI_3WIRE_MODE {
	BME280_SPI_3WIRE_MODE_OFF 	= 0x00,	//xxxx xxx0
	BME280_SPI_3WIRE_MODE_ON		= 0x01	//xxxx xxx1
};
//Filter coefficient selection
enum BME280_FILTER_COEFF {
	BME280_FILTER_COEFF_OFF	=	0x00	,	//xxx0 00xx
	BME280_FILTER_COEFF_2		=	0x04	,	//xxx0 01xx
	BME280_FILTER_COEFF_4		=	0x08	,	//xxx0 10xx
	BME280_FILTER_COEFF_8		=	0x0C,	//xxx0 11xx
	BME280_FILTER_COEFF_16		= 0x10		//xxx1 00xx
};
//Time standby duration selection
enum BME280_STANDBY_TIME {
	BME280_STANDBY_TIME_0_5_MS	 	= 0x00,	//000x xxxx
	BME280_STANDBY_TIME_62_5_MS 	= 0x20,	//001x xxxx
	BME280_STANDBY_TIME_125_MS		=	0x40,	//010x xxxx
	BME280_STANDBY_TIME_250_MS		=	0x60,	//011x xxxx
	BME280_STANDBY_TIME_500_MS		=	0x80,	//100x xxxx
	BME280_STANDBY_TIME_1000_MS	=	0xA0,	//101x xxxx
	BME280_STANDBY_TIME_10_MS		= 0xC0,	//110x xxxx
	BME280_STANDBY_TIME_20_MS		= 0xE0		//111x xxxx
};
//=============================================================================================
#endif /* SRC_BME280_BME280_REGISTERS_H_ */
