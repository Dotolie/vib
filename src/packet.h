//------------------------------------------------------------------------------
//
//  Copyright (C) 2008 MVTech Co., Ltd. All Rights Reserved
//  MVTech Co.,Ltd. Proprietary & Confidential
//
//  Reproduction in whole or in part is prohibited without the written consent 
//  of the copyright owner. MVTech reserves the right to make changes 
//  without notice at any time. MVTech makes no warranty, expressed, 
//  implied or statutory, including but not limited to any implied warranty of 
//  merchantability or fitness for any particular purpose, or that the use will
//  not infringe any third party patent,  copyright or trademark. 
//  MVTech must not be liable for any loss or damage arising from its use.
//
//  Module      :
//  File           :
//  Description :
//  Author      :
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __PACKET_H__
#define __PACKET_H__


#define PACKET_ID			0x434f5802


#define MAKE_NETHDR( data, size, cmd ) 	\
	do {									\
		data[0] = 'M'; 					\
		data[1] = 'V'; 					\
		data[2] = size & 0xff;				\
		data[3] = (size >> 8) & 0xff;		\
		data[4] = cmd;					\
	} while(0)



enum {
	PKT_ACK                 		= 0x01,
	PKT_NAK                 		= 0x02,
	PKT_GREETING		     	= 0x03,
	PKT_RESET          	    	= 0x04,
	PKT_VERSION			= 0x05,
	
	PKT_REG_ALL			= 0x10,
	PKT_REGISTER 	         	= 0x11,
	PKT_GLOB_CMD			= 0x12,
	PKT_START_STOP			= 0x13,
	PKT_SET_MODE			= 0x14,

	PKT_DATA_MANFFT		= 0x20,
	PKT_DATA_AUTOFFT		= 0x21,
	PKT_DATA_MANCAP		= 0x22,
	PKT_DATA_REALTIME		= 0x23,

	PKT_SET_CONFIG_ALARM	= 0x30,
	PKT_GET_CONFIG_ALARM	= 0x31,
	PKT_CONTROL_ALARM	= 0x32,
	PKT_STATUS_ALARM		= 0x33,
	PKT_CLEAR_ALARM		= 0x34,
	
	PKT_END				= 0xff,
};


enum {
	MODE_MANUAL_FFT 		= 0,
	MODE_AUTOMATIC_FFT,
	MODE_MANUAL_CAPTURE,
	MODE_REAL_TIME,
};

enum {
	REAL_AXIS_X			= 0,
	REAL_AXIS_Y,
	REAL_AXIS_Z,
};

typedef struct  
{
	uint8_t		type;
	uint16_t		address;
	uint16_t		value;
} __attribute__((packed)) REGISTER_t, *REGISTER_PTR_t;

typedef struct
{
	uint8_t		alm_f_l;
	uint8_t		alm_f_h;
	uint16_t		alm_x_mag1;
	uint16_t		alm_x_mag2;	
	uint16_t		alm_y_mag1;
	uint16_t		alm_y_mag2;	
	uint16_t		alm_z_mag1;
	uint16_t		alm_z_mag2;	
}__attribute__((packed)) ALARM_t, *ALARM_PTR_t;

typedef struct
{
	uint16_t		user_id;
	
	uint8_t		mode;
	uint8_t		record_period;
	uint8_t		real_axis;
	uint8_t		running;
	uint8_t		scale;
	uint8_t		bandwidth;

	uint8_t		alm_ctrl;
	ALARM_t		alarms[6];
} __attribute__((packed)) STATUS_t, *STATUS_PTR_t;

typedef struct
{
	uint16_t		diag_stat;
	uint16_t		alm_x_stat;
	uint16_t		alm_y_stat;
	uint16_t		alm_z_stat;
} __attribute__((packed)) ALMSTAT_t, *ALMSTAT_PTR_t;



#endif /* __PACKET_H__ */

