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
//  File           : sensor.h
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <stddef.h>
#include <stdbool.h>
#include "thread.h"
#include "work.h"
#include "packet.h"

// Registers of ADIS18266
#define FLASH_CNT		0x00
#define X_SENS			0x02
#define Y_SENS			0x04
#define Z_SENS			0x06
#define TEMP_OUT		0x08
#define SUPPLY_OUT		0x0a
#define FFT_AVG1		0x0c
#define FFT_AVG2		0x0e
#define BUF_PNTR		0x10
#define REC_PNTR		0x12
#define X_BUF			0x14
#define Y_BUF			0x16
#define Z_BUF			0x18
#define REC_CTRL1		0x1a
#define REC_CTRL2		0x1c
#define REC_PRD			0x1e
#define ALM_F_LOW		0x20
#define ALM_F_HIGH		0x22
#define ALM_X_MAG1		0x24
#define ALM_Y_MAG1		0x26
#define ALM_Z_MAG1		0x28
#define ALM_X_MAG2		0x2a
#define ALM_Y_MAG2		0x2c
#define ALM_Z_MAG2		0x2e
#define ALM_PNTR		0x30
#define ALM_S_MAG		0x32
#define ALM_CTRL		0x34
#define DIO_CTRL			0x36
#define GPIO_CTRL		0x38
#define AVG_CNT			0x3a
#define DIAG_STAT		0x3c
#define GLOB_CMD		0x3e
#define ALM_X_STAT		0x40
#define ALM_Y_STAT		0x42
#define ALM_Z_STAT		0x44
#define ALM_X_PEAK		0x46
#define ALM_Y_PEAK		0x48
#define ALM_Z_PEAK		0x4a
#define TIME_STAMP_L	0x4c
#define TIME_STAMP_H	0x4e

#define LOT_ID1			0x52
#define LOT_ID2			0x54
#define PROD_ID			0x56
#define SERIAL_NUM		0x58

#define USER_ID			0x5c
#define REC_FLSH_CNT	0x5e



#define REC_INFO1		0x6e
#define ALM_X_FREQ		0x70
#define ALM_Y_FREQ		0x72
#define ALM_Z_FREQ		0x74
#define REC_INFO2		0x76
#define REC_CNTR		0x78
#define CAL_ENABLE		0x7a





// GLOBAL COMMANDS to GLOB_CMD  (0x3E)
#define  	GCMD_AUTO_NULL	0x0001
#define 	GCMD_PWR_DWN		0x0002
#define	GCMD_SELF_TEST	0x0004
#define	GCMD_REST_F_SET	0x0008
#define	GCMD_CLR_DIAG_ST	0x0010
#define	GCMD_FLASH_TST	0x0020
#define	GCMD_SAVE_REG		0x0040
#define	GCMD_SOFT_RESET	0x0080
// GLOBAL COMMANDS to GLOB_CMD  (0x3F)
#define	GCMD_CLR_RECORD	0x0001
#define	GCMD_CLR_ALM_FL	0x0002
#define	GCMD_SET_BUF_PTR	0x0004
#define	GCMD_START_STOP	0x0008
#define	GCMD_SAVE_ALM	0x0010
#define	GCMD_RET_DATA_FL	0x0020
#define	GCMD_RET_ALM_INF	0x0040
#define	GCMD_CLR_AUTO_NL	0x0080


// REC_CTRL1 (0x1a) 
#define	RC1_MAN_FFT		0x0000
#define	RC1_AUTO_FFT		0x0001
#define	RC1_MAN_CAP		0x0002
#define	RC1_REAL_CAP		0x0003

#define	RC1_STR_NONE		0x0000
#define	RC1_STR_ALM_TR	0x0004
#define	RC1_STR_ALL		0x0008

#define	RC1_PWRDN_REC		0x0080
// REC_CTRL1 (0x1b) 
#define	RC1_SR0_FFT_EN		0x0001
#define	RC1_SR1_FFT_EN		0x0002
#define	RC1_SR2_FFT_EN		0x0004
#define	RC1_SR3_FFT_EN		0x0008

#define	RC1_WIN_RECT		0x0000
#define	RC1_WIN_HANN		0x0010
#define	RC1_WIN_TOP		0x0020
#define	RC1_WIN_NONE		0x0030


#define	BUF_SIZE			1100
#define	MAX_BUF			64	


#define SPIREG_MINOR   	252
#define SPIREG_NAME    	"adis16228"

#define IOCTL_MAGIC	SPIREG_MINOR

#define IOCTL_SPI_GET_CLK		_IO(IOCTL_MAGIC,0)
#define IOCTL_SPI_SET_CLK		_IO(IOCTL_MAGIC,1)
#define IOCTL_SPI_READ       		_IO(IOCTL_MAGIC,2)
#define IOCTL_SPI_WRITE		_IO(IOCTL_MAGIC,3)
#define IOCTL_SPI_INT_EN		_IO(IOCTL_MAGIC,4)
#define IOCTL_SPI_INT_DISEN		_IO(IOCTL_MAGIC,5)
//#define IOCTL_SPI_INT_DISEN		_IOW(SPI_IOC_MAGIC,100,__u8)
#define IOCTL_SET_XYZ_REG		_IO(IOCTL_MAGIC,6)
#define IOCTL_GET_DATA		_IO(IOCTL_MAGIC,7)
#define IOCTL_SET_SIZE			_IO(IOCTL_MAGIC,8)
#define IOCTL_SET_PID			_IO(IOCTL_MAGIC,9)
#define IOCTL_SPI_WRITE1		_IO(IOCTL_MAGIC,10)
#define IOCTL_SPI_INT_EN1		_IO(IOCTL_MAGIC,11)
#define IOCTL_SPI_SET_MODE		_IO(IOCTL_MAGIC,12)
//#define IOCTL_SPI_INT_EN1		_IOW(SPI_IOC_MAGIC,101,__u8)


typedef struct {
        unsigned char inb[3];
        unsigned char outb[2];
        unsigned int len;
}SPI_DataType;

class CSensor: public Runnable
{
protected:
	bool m_bRun;
	bool m_bRead;
	bool m_bClearAlarm;
	int 	m_nIrq;
	int	m_nFd;
	CWork *m_pWork;
	int	m_nIdxBuf;
	char *m_pcBuf;
	char m_cBuf[MAX_BUF][BUF_SIZE];
	pthread_mutex_t	m_Mutex;
	
	int gpio_is_exported(size_t gpio);
	int gpio_export(size_t gpio);
	int gpio_unexport(size_t gpio);
	int gpio_set_direction(size_t gpio, bool output);
	int gpio_set_edge(size_t gpio, bool rising, bool falling);
	int gpio_open(size_t gpio, int mode);	
	int Set_Interface_Link(void);
	void Cleanup_Interface_Link(int irq);

	int openDev( char *name );
	void closeDev(void);

	void getDataRealTime( char );
	void getDataRealTime(int nFd);
	void getData( char );
	

	
public:
	CSensor();
	virtual ~CSensor();
	
	bool IsRun();
	virtual void Run();
	virtual void Stop();

	int writeReg(char reg, char val);
	int writeReg(char reg, short val);
	short readReg(char reg);
	int readRegisterAll();
	int readAlarmBands();
	void SetNetwork(CWork *pWork);

	int GetMode();
	void SetMode(int);

	int GetScale();
	int GetStatus();

	int GetStatusAlm();
	int SetAlarm(int nOp, int nBand, ALARM_t *pAlarm);
	int GetAlarm(int nOp, int nBand, ALARM_t *pAlarm);
	
	int SaveConfigAlm();
	int ClearConfigAlm();

	int EnableAlarm( char );
	int ClearAlarm();
	
	int GetBandWidth();
	int GetRecPeriod();




	int	m_nIndex;
	int	m_nRecord;
	short m_sData[3][512];
	char m_cRegister[0x7d];
	unsigned short *m_psRegister;
	STATUS_t m_Status;

	
};

#endif  // __SENSOR_H__

