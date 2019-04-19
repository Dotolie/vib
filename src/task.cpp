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
//  File           : Task.cpp
//  Description :
//  Author      : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "stdafx.h"
#include "Socket.h"
#include "packet.h"


#include "task.h"

#define DEBUG
#include "debug.h"


#define 	EXEC_R(x)		&CTask::On_##x
#define 	EXEC_S(x)		&CTask::On_##x
#define 	EXEC(x)			&CTask::On_##x

CTask app;

unsigned short CTask::CRC16TBL[256] =
{

	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,

	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,

};


BEGIN_MESSAGE_MAP(CTask)
	{ PKT_ACK, 				EXEC(PKT_ACK)				},
	{ PKT_NAK,				EXEC(PKT_NAK)				},
	{ PKT_GREETING, 			EXEC(PKT_GREETING)			},
	{ PKT_RESET,				EXEC(PKT_RESET)				},
	{ PKT_VERSION,			EXEC(PKT_VERSION)			},
	{ PKT_REG_ALL,			EXEC(PKT_REG_ALL)			},
	{ PKT_REGISTER,			EXEC(PKT_REGISTER)			},
	{ PKT_GLOB_CMD,			EXEC(PKT_GLOB_CMD)			},	
	{ PKT_START_STOP,		EXEC(PKT_START_STOP)		},	
	{ PKT_SET_MODE,			EXEC(PKT_SET_MODE)			},	
	{ PKT_DATA_MANFFT,		EXEC(PKT_DATA_MANFFT)		},
	{ PKT_DATA_AUTOFFT,		EXEC(PKT_DATA_AUTOFFT)		},
	{ PKT_DATA_MANCAP,		EXEC(PKT_DATA_MANCAP)		},
	{ PKT_DATA_REALTIME,	EXEC(PKT_DATA_REALTIME)	},
	{ PKT_SET_CONFIG_ALARM,	EXEC(PKT_SET_CONFIG_ALARM) },
	{ PKT_GET_CONFIG_ALARM,	EXEC(PKT_GET_CONFIG_ALARM) },
	{ PKT_CONTROL_ALARM,	EXEC(PKT_CONTROL_ALARM)	},	
	{ PKT_STATUS_ALARM,		EXEC(PKT_STATUS_ALARM)		},
	{ PKT_CLEAR_ALARM, 		EXEC(PKT_CLEAR_ALARM)		},

	{ MSG_CREATE,			EXEC(MSG_CREATE)			},
	{ MSG_CLOSE,			EXEC(MSG_CLOSE)			},	
	{ MSG_DISCONNECT,		EXEC(MSG_DISCONNECT)		},	
	{ MSG_QUIT,				EXEC(MSG_QUIT)				},
END_MESSAGE_MAP()

/*
int CTask::Dispatch(short cmd, int size, char* pdata, Socket* psock)
{
	int i=0;
	int (CTask::*fp)(int, char*, Socket*);
	
	while(map[i].id !=0xffff) {
		if( cmd == map[i].id ) {
			fp = map[i].mfp;
			this->*fp( size, pdata, psock);
			}
		}
	DBG("[%s]-----cmd=0x%x, size=%d\r\n", __func__, cmd, size);
	
	return 0;
}
*/

CTask::CTask()
{
	DBG("[%s:%s] create\r\n", __FILE__,__func__);
	

}
CTask::~CTask()
{

	DBG("[%s:%s] destroy\r\n", __FILE__,__func__);
	

	if( m_pthSocket ) {
		m_pthSocket->Stop();
//		m_pWork->m_Sock.close();
//		m_pthSocket->Wait();
		DBG("\033[7;31m[%s:%s]\033[0m %s is Stopping \r\n", __FILE__,__func__, m_pthSocket->GetName());
		delete m_pthSocket;
		m_pthSocket = NULL;	
		delete m_pWork;
		}

	if( m_pthSensor ) {
		m_pthSensor->Stop();
		m_pthSensor->Wait();
		DBG("\033[7;31m[%s:%s]\033[0m %s is Stopping \r\n", __FILE__,__func__, m_pthSensor->GetName());
		delete m_pthSensor;
		m_pthSensor = NULL;			
		delete m_pSensor;
		}
		
}

unsigned short CTask::CRC16( const unsigned char* pdata, int size )
{
	unsigned char cVal = 0;
	unsigned short sTemp = 0;

	for(int i=0;i<size;i++ ) 	{
		cVal = pdata[i] ^ (sTemp>>8);
		sTemp = (sTemp<<8) ^ CRC16TBL[cVal];
		}

	return sTemp;
}

int CTask::On_PKT_ACK( int wparam, int lparam )
{
//	char data[4];
	
//	DBG("\033[7;31m[%s:%s]\033[0m  w=%d, l=%d \r\n", __FILE__,__func__, wparam, lparam);

	// TODO: update timeout flag()
	
	return 0;
}

int CTask::On_PKT_NAK( int wparam, int lparam )
{

	DBG("\033[7;31m[%s:%s]\033[0m \r\n", __FILE__,__func__);

	
	return 0;
}

int CTask::On_PKT_GREETING( int wparam, int lparam )
{
	char data[100];
	char check = 0;
	int len = 5;
	int ret = 0;
	int nSize = sizeof(STATUS_t);

	m_pSensor->GetStatus();

	MAKE_NETHDR( data, (len+nSize), PKT_GREETING );
	memcpy(data+len, (char*)&(m_pSensor->m_Status), nSize);

	if( m_pWork ) {
		ret = m_pWork->m_Sock.send(data, (len+nSize));

		DBG("\033[7;31m[%s:%s]\033[0m size=%d, id=%x, mode=%d, rec_peri=%d, real_axis=%d, scale=%d, band=%d, almctl=%x\r\n", 
			__FILE__,__func__, nSize,
			m_pSensor->m_Status.user_id, 			
			m_pSensor->m_Status.mode, 
			m_pSensor->m_Status.record_period,
			m_pSensor->m_Status.real_axis,
			m_pSensor->m_Status.scale,
			m_pSensor->m_Status.bandwidth,
			m_pSensor->m_Status.alm_ctrl
			);
		}

	return 0;
}

int CTask::On_PKT_RESET( int wparam, int lparam )
{
	int nRet = 0;

	DBG("\033[7;31m[%s:%s]\033[0m w=%d\r\n", __FILE__,__func__, wparam);
	return 0;
}

int CTask::On_PKT_VERSION( int wparam, int lparam )
{
	DBG("\033[7;31m[%s:%s]\033[0m w=%d, l=%d\r\n", __FILE__,__func__, wparam, lparam);

	
	return 0;
}

int CTask::On_PKT_REG_ALL( int wparam, int lparam )
{
	short	addr = 0;
	short	val = 0x1234;
	char data[0x82];
	int len = 5;
	int ret = 0;

//	m_pSensor->readRegisterAll();
		
	MAKE_NETHDR( data, (len+0x7d), PKT_REG_ALL );
	memcpy(data+5, m_pSensor->m_cRegister, sizeof(m_pSensor->m_cRegister));
	
	if( m_pWork )
		ret = m_pWork->m_Sock.send(data, (len+0x7d));

	DBG("\033[7;31m[%s:%s]\033[0m send register size=%d, ret=%d\r\n", __FILE__,__func__, (len+0x7e), ret);	

	return ret;
}


int CTask::On_PKT_REGISTER( int wparam, int lparam )
{
	REGISTER_t	param;
	short	addr = 0;
	short	val = 0;
	char data[10];
	int len = 5;
	int ret = 0;

	memcpy((char*)&param, (char*)lparam, wparam);
	addr = param.address;
	val = param.value;
	
	DBG("\033[7;31m[%s:%s]\033[0m type=%c, addr=0x%x, val=0x%x\r\n", __FILE__,__func__, param.type, addr, val);		

	if( param.type == 'R' ) {
		// read
		m_pSensor->readReg((char)addr);
		val = m_pSensor->readReg((char)addr);
		
		MAKE_NETHDR( data, (len+5), PKT_REGISTER );
		data[len++]= 'R';
		}
	else if( param.type == 'W' ) {
		// wirte
		m_pSensor->writeReg((char)addr, (short)val);
		usleep(20);		
		// read
		m_pSensor->readReg((char)addr);
		val = m_pSensor->readReg((char)addr);
		
		MAKE_NETHDR( data, (len+5), PKT_REGISTER );
		data[len++]= 'W';
		}
	else {
		// wirte
		m_pSensor->writeReg((char)addr, (char)val);
		usleep(20);
		// read
		m_pSensor->readReg((char)addr);
		val = m_pSensor->readReg((char)addr);
		
		MAKE_NETHDR( data, (len+5), PKT_REGISTER );
		data[len++]= 'w';
		}

	data[len++]= addr & 0xff;
	data[len++]= (addr>>8) & 0xff;
	data[len++]= val & 0xff;
	data[len++]= (val>>8) & 0xff;
	if( m_pWork )
		ret = m_pWork->m_Sock.send(data, (len+5));

	
	DBG("\033[7;31m[%s:%s]\033[0m type=%c, addr=0x%x, val=0x%x\r\n", __FILE__,__func__, param.type, addr, val);	

	
	return ret;
}

int CTask::On_PKT_GLOB_CMD( int wparam, int lparam )
{

	return 0;
}

int CTask::On_PKT_START_STOP( int wparam, int lparam )
{
	int nRet = 0;
	
	if( !m_pSensor->IsRun() ) {
		m_pSensor->writeReg((GLOB_CMD), (char)GCMD_CLR_DIAG_ST);
		usleep(40);
		m_pSensor->writeReg((GLOB_CMD+1), (char)GCMD_START_STOP);
		m_pthSensor->Start();
		}
	else {
		m_pthSensor->Stop();
		}
	
	DBG("\033[7;31m[%s:%s]\033[0m mode=%d, Record=%d, run=%d\r\n", __FILE__,__func__, m_pSensor->GetMode(), m_pSensor->m_nRecord, m_pSensor->IsRun());	
	
	return 0;
}

int CTask::On_PKT_SET_MODE( int wparam, int lparam )
{
	int nRet = 0;
	char data[1];
	char cVal = m_pSensor->m_cRegister[REC_CTRL1];

	memcpy(data, (char*)lparam, wparam);
	cVal &= 0xfc;
	cVal |= data[0];

	
	m_pSensor->m_Status.mode = data[0];

//	if( m_pSensor->m_Status.mode == MODE_REAL_TIME ) {
		if( m_pSensor->IsRun() ) {
			m_pthSensor->Stop();
			}
//		}

	nRet = m_pSensor->writeReg((REC_CTRL1), cVal);
	nRet = m_pSensor->writeReg((REC_CTRL1), cVal);
	
	m_pSensor->SetMode(data[0]);
	
	if( data[0] == MODE_REAL_TIME ) {
		// real time mode
		m_pSensor->m_nRecord = 0;
		m_pSensor->m_nIndex = 0;
		m_pthSensor->Start();
		}

	
	DBG("\033[7;31m[%s:%s]\033[0m mode=0x%x\r\n", __FILE__,__func__, data[0]);	
	
	return 0;
}

int CTask::On_PKT_DATA_MANFFT( int wparam, int lparam )
{
	int ret = 0;
	char *pPkt = (char*)lparam;

	if( m_pWork )
		ret = m_pWork->m_Sock.send(pPkt, wparam);

	DBG("\033[7;32m[%s:%s]\033[0m ret=%d \r\n", __FILE__,__func__, ret);
	
	return 0;
}

int CTask::On_PKT_DATA_AUTOFFT( int wparam, int lparam )
{
	int ret = 0;
	char *pPkt = (char*)lparam;

	if( m_pWork )
		ret = m_pWork->m_Sock.send(pPkt, wparam);

	DBG("\033[7;32m[%s:%s]\033[0m ret=%d \r\n", __FILE__,__func__, ret );
	
	return 0;
}

int CTask::On_PKT_DATA_MANCAP( int wparam, int lparam )
{
	int ret = 0;
	char *pPkt = (char*)lparam;

	if( m_pWork )
		ret = m_pWork->m_Sock.send(pPkt, wparam);

	DBG("\033[7;32m[%s:%s]\033[0m ret=%d \r\n", __FILE__,__func__, ret);
	
	return 0;
}

int CTask::On_PKT_DATA_REALTIME( int wparam, int lparam )
{
	int ret = 0;
	char *pPkt = (char*)lparam;

	if( m_pWork )
		ret = m_pWork->m_Sock.send(pPkt, wparam);

	DBG("\033[7;32m[%s:%s]\033[0m ret=%d \r\n", __FILE__,__func__, ret);
	
	return 0;
}

int CTask::On_PKT_SET_CONFIG_ALARM( int wparam, int lparam )
{
	int ret = 0;
	int nOp = 0;
	ALARM_t alarms[6];
	ALARM_t alarm;

	memcpy((char*)alarms, (char*)lparam, wparam);

	m_pSensor->ClearConfigAlm();
	

	m_pSensor->SetAlarm(nOp, 0, &alarms[0]);
	m_pSensor->SetAlarm(nOp, 1, &alarms[1]);
	m_pSensor->SetAlarm(nOp, 2, &alarms[2]);
	m_pSensor->SetAlarm(nOp, 3, &alarms[3]);
	m_pSensor->SetAlarm(nOp, 4, &alarms[4]);
	m_pSensor->SetAlarm(nOp, 5, &alarms[5]);
		
#if 1
	alarm.alm_f_l = 0;
	alarm.alm_f_h = 0;
	alarm.alm_x_mag1 = 0;
	alarm.alm_x_mag2 = 0;	
	alarm.alm_y_mag1 = 0;
	alarm.alm_y_mag2 = 0;	
	alarm.alm_z_mag1 = 0;
	alarm.alm_z_mag2 = 0;	
	
	for( nOp = 1; nOp<4; nOp++) {
		m_pSensor->SetAlarm(nOp, 0, &alarm);
		m_pSensor->SetAlarm(nOp, 1, &alarm);
		m_pSensor->SetAlarm(nOp, 2, &alarm);
		m_pSensor->SetAlarm(nOp, 3, &alarm);
		m_pSensor->SetAlarm(nOp, 4, &alarm);
		m_pSensor->SetAlarm(nOp, 5, &alarm);
		}
	
#endif
	DBG("\033[7;32m[%s:%s]\033[0m\r\n", __FILE__,__func__);
	
	return 0;
}

int CTask::On_PKT_GET_CONFIG_ALARM( int wparam, int lparam )
{
	int ret = 0;
	int nOp = 0;
	char data[90];
	ALARM_t alarms[6];
	int len = 5;
	int nSize = sizeof(alarms);


	m_pSensor->GetAlarm(nOp, 0, &alarms[0]);
	m_pSensor->GetAlarm(nOp, 1, &alarms[1]);
	m_pSensor->GetAlarm(nOp, 2, &alarms[2]);
	m_pSensor->GetAlarm(nOp, 3, &alarms[3]);	
	m_pSensor->GetAlarm(nOp, 4, &alarms[4]);
	m_pSensor->GetAlarm(nOp, 5, &alarms[5]);

	MAKE_NETHDR( data, (len+nSize), PKT_GET_CONFIG_ALARM);
	memcpy(data+len, (char*)alarms, nSize);
	if( m_pWork )
		ret = m_pWork->m_Sock.send(data, (len+nSize));

	DBG("\033[7;32m[%s:%s]\033[0m nSize=%d\r\n", __FILE__,__func__, nSize);
	return 0;
}

int CTask::On_PKT_CONTROL_ALARM( int wparam, int lparam )
{
	int ret = 0;
	char data[4] = {0,};

	memcpy(data, (char*)lparam, wparam);

	m_pSensor->EnableAlarm( data[0] );

	DBG("\033[7;32m[%s:%s]\033[0m w=%d, l=%d, data=0x%x\r\n", __FILE__,__func__, wparam, lparam, data[0]);
	
	return 0;
}

int CTask::On_PKT_STATUS_ALARM( int wparam, int lparam )
{
	int ret = 0;
	int len = 5;
	char data[20];
		
	memcpy(data, (char*)lparam, wparam);
	
	if( m_pWork )
		ret = m_pWork->m_Sock.send(data, wparam);
	
	DBG("\033[7;32m[%s:%s]\033[0m w=%d, l=%d \r\n", __FILE__,__func__, wparam, lparam);

	return 0;
}

int CTask::On_PKT_CLEAR_ALARM( int wparam, int lparam )
{
	int ret = 0;

	m_pSensor->ClearAlarm();

	DBG("\033[7;32m[%s:%s]\033[0m w=%d, l=%d \r\n", __FILE__,__func__, wparam, lparam );
	
	return 0;
}

int CTask::On_MSG_CREATE( int wparam, int lparam )
{

	DBG("\033[7;33m[%s:%s]\033[0m w=%d, l=%d\r\n", __FILE__,__func__, wparam, lparam);

	system("dd if=up.bin of=/dev/mmcblk0 skip=0 seek=17440256 bs=1 count=4 > /dev/null 2>&1");
	system("dd if=up.bin of=/dev/mmcblk0 skip=0 seek=17423872 bs=1 count=4 > /dev/null 2>&1");

	m_pWork = new CWork();
	m_pWork->SetName("Work");
	m_pthSocket = new Thread( m_pWork );
	m_pthSocket->Start();


	m_pSensor = new CSensor();
	m_pSensor->SetName("Sensor");
	m_pthSensor = new Thread( m_pSensor );
	m_pSensor->SetNetwork(m_pWork);
//	m_pthSensor->Start();
	
	return 0;
}
int CTask::On_MSG_CLOSE( int wparam, int lparam )
{
	DBG("\033[7;33m[%s:%s]\033[0m w=%d, l=%d\r\n", __FILE__,__func__, wparam, lparam);

	
	return 0;
}

int CTask::On_MSG_DISCONNECT( int wparam, int lparam )
{
	short sStatus = 0;

//	if( m_pSensor->IsRun() ) {
//		m_pthSensor->Stop();
//		}
	
//	while( ((sStatus = m_pSensor->readReg(DIAG_STAT)) & 0x0080 ) == 0 )
//	if( m_pSensor->GetMode() == MODE_AUTOMATIC_FFT )
	if (0)
	{
		m_pSensor->writeReg( (char)0x68, (char)0xe8);
		usleep(50);
		printf("=\r\n");
	}
//	m_pSensor->writeReg( (char)GLOB_CMD, (short)GCMD_SOFT_RESET);
//	usleep(52000);

	
	DBG("\033[7;33m[%s:%s]\033[0m w=%d, l=%d\r\n", __FILE__,__func__, wparam, lparam);


	return 0;
}

int CTask::On_MSG_QUIT( int wparam, int lparam )
{
	
	DBG("\033[7;33m[%s:%s]\033[0m w=%d, l=%d\r\n", __FILE__,__func__, wparam, lparam);

	return 0;
}

