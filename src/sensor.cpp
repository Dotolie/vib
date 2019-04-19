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
//  File           : sensor.cpp
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h> 
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>

#include "sensor.h"
#include "object.h"

#define DEBUG
#include "debug.h"



// GPX0(5) = 125
#define 	IRQ_PIN			125

#define	SPI_FREQ		2400000
#define	SPI_BIT			8
#define	SPI_DEV			"/dev/spidev0.0"

#define CFG_GPIO_DIR 	"/sys/class/gpio"
#define CFG_GPIO_SYS_FILE_EXPORTED_TIME_IN_100MS 	10

#define NEXT_BUFFER()		\
	do {						\
		m_nIdxBuf++;				\
		m_nIdxBuf%=MAX_BUF;		\
		m_pcBuf = m_cBuf[m_nIdxBuf];		\
	}while(0)			

CSensor::CSensor() 
{
	m_bRun = false;
	m_bRead = false;
	m_nRecord = 256;
	m_nIndex = 0;
	m_nIdxBuf = 0;
	m_pcBuf = m_cBuf[0];
	m_bClearAlarm = false;
	
	pthread_mutex_init( &m_Mutex, NULL );
	
	m_psRegister= (unsigned short*)m_cRegister;
#if 0
//	m_nIrq = Set_Interface_Link();
	if( m_nIrq < 0 ) {
		DBG("[%s:%s] can't link irq pin\r\n", __FILE__,__func__);
		}

	m_nFd = openDev(SPI_DEV);
	if( m_nFd < 0 ) {
		DBG("[%s:%s] can't open spi dev.\r\n", __FILE__,__func__);
		}	
	
//	writeReg((char)GLOB_CMD, (char)GCMD_REST_F_SET);
//	usleep(84000);
	writeReg( (char)GLOB_CMD, (short)GCMD_SOFT_RESET);
	usleep(52000);

	writeReg( (char)GLOB_CMD, (short)GCMD_AUTO_NULL);
	usleep(882000);
	
	writeReg( (char)REC_PRD, (char)0x01);
	usleep(52000);

	readRegisterAll();

	m_Status.mode = GetMode();
	m_Status.record_period = GetRecPeriod();
	m_Status.real_axis = REAL_AXIS_X;
#else
	m_nFd = open("/dev/adis16228", O_RDWR);
	if( m_nFd < 0 ) {
		DBG("[%s:%s] can't open spi dev.\r\n", __FILE__,__func__);
		}	

	writeReg( (char)GLOB_CMD, (short)GCMD_SOFT_RESET);
	usleep(52000);

//	writeReg((char)GLOB_CMD, (char)GCMD_REST_F_SET);
//	usleep(84000);

	writeReg( (char)GLOB_CMD, (short)GCMD_AUTO_NULL);
	usleep(882000);
	
	writeReg( (char)REC_PRD, (char)0x01);
	usleep(1000);

	writeReg( (char)FFT_AVG1, (char)0x01);
	usleep(1000);

	readRegisterAll();
	readAlarmBands();
#if 0
	readAlarmBands();

	m_Status.mode = GetMode();
	m_Status.record_period = GetRecPeriod();
	m_Status.real_axis = REAL_AXIS_X;
	m_Status.scale = GetScale();
	m_Status.bandwidth = GetBandWidth();
#endif;

#endif
	
}

CSensor::~CSensor() 
{
	Stop();

	closeDev();
//	Cleanup_Interface_Link(m_nIrq);

	pthread_mutex_unlock( &m_Mutex );
	pthread_mutex_destroy( &m_Mutex );
	
	DBG("[%s:%s]  destroy \r\n", __FILE__,__func__ );
}

void CSensor::Stop()
{
	m_bRun = false;
	DBG("[%s:%s]  m_brun= false\r\n", __FILE__,__func__);
}

void CSensor::Run()
{
#if 1
	int ret = 0;
	int nMode = 0;
	struct pollfd pollfd;
	char buf[1024];

//	DBG("[%s:%s] start of thread [%c]\r\n",  __FILE__,__func__, c );
	nMode = GetMode();
	switch( nMode )	{
		case MODE_MANUAL_FFT:
			m_nRecord = 256;
			break;
		case MODE_AUTOMATIC_FFT:
			m_nRecord = 256;
			break;
		case MODE_MANUAL_CAPTURE:
			m_nRecord = 512;
			break;
		case MODE_REAL_TIME:
			m_nRecord = 512;
			break;
		}


	ioctl(m_nFd, IOCTL_SET_SIZE, m_nRecord);
	ioctl(m_nFd, IOCTL_SPI_INT_EN1, 0);
	
	memset(&pollfd, 0, sizeof(pollfd));
	pollfd.fd = m_nFd;
	pollfd.events = POLLIN;

	
	m_bRun = true;
	m_Status.running = 1;

	while(m_bRun) {
		ret = poll( &pollfd, 1, 1000 ); //Block forever
		if( ret < 0  || pollfd.revents & POLLNVAL)	{
			DBG("[%s:%s] poll.fd=%d poll.rev=0x%x, ret=%d\r\n", __FILE__,__func__ , pollfd.fd, pollfd.revents, ret);
			m_bRun = false;
			continue;
			//Return if thread is joining
			}
		if( pollfd.revents & POLLIN ) {
//			DBG(" evnet post----c=%c, ret=%d, poll=0x%x\r\n",  c, ret, pollfd.revents);

			switch( nMode ){
				case MODE_MANUAL_FFT:
					ret = read(m_nFd, buf, 1);
					getData(PKT_DATA_MANFFT);
					m_bRun = false;
					break;
				case MODE_AUTOMATIC_FFT:
					ret = read(m_nFd, buf, 1);
					getData(PKT_DATA_AUTOFFT);
					break;
				case MODE_MANUAL_CAPTURE:
					ret = read(m_nFd, buf, 1);
					getData(PKT_DATA_MANCAP);
					m_bRun = false;
					break;
				case MODE_REAL_TIME:
					getDataRealTime(m_nFd);
//					DBG("[%s:%s] ret=%d \r\n", __FILE__, __func__, ret);
					break;
				default:
					DBG("[%s:%s] error \r\n", __FILE__, __func__);
					break;
				}
			}

		}
	m_Status.running = 0;
	ioctl(m_nFd, IOCTL_SPI_INT_DISEN, 0);
	if( nMode == MODE_AUTOMATIC_FFT){
		writeReg((GLOB_CMD+1), (char)GCMD_START_STOP);
		}


#else
	int ret = 0;
	int nMode = 0;
	struct pollfd pollfd;
	char volatile c = 0;
	short sStatus = 0;

	memset(&pollfd, 0, sizeof(pollfd));
	pollfd.fd = m_nIrq;
	pollfd.events = POLLPRI;

	//Initial status: If pin is already high, post an event
	lseek(pollfd.fd, 0, SEEK_SET);
	read(pollfd.fd, (char *)&c, 1);


//	DBG("[%s:%s] start of thread [%c]\r\n",  __FILE__,__func__, c );
	nMode = GetMode();
	switch( nMode )	{
		case MODE_MANUAL_FFT:
			m_nRecord = 256;
			break;
		case MODE_AUTOMATIC_FFT:
			m_Status.running = 1;
			m_nRecord = 256;
			break;
		case MODE_MANUAL_CAPTURE:
			m_nRecord = 512;
			break;
		case MODE_REAL_TIME:
			m_nRecord = 0;
			break;
		}

	
	m_bRun = true;

	while(m_bRun) {
		ret = poll( &pollfd, 1, 1000 ); //Block forever
		if( ret < 0  || pollfd.revents & POLLNVAL)	{
			DBG("[%s:%s] poll.fd=%d poll.rev=0x%x, ret=%d\r\n", __FILE__,__func__ , pollfd.fd, pollfd.revents, ret);
			m_bRun = false;
			continue;
			//Return if thread is joining
			}
		if( pollfd.revents & POLLPRI ) {
			//Do a dummy read to acknowledge the event, before posting the event (to avoid race condition)
			lseek(pollfd.fd, 0, SEEK_SET);			
			read(pollfd.fd, (char *)&c, 1);

//			DBG(" evnet post----c=%c, ret=%d, poll=0x%x, status=0x%x \r\n",  c, ret, pollfd.revents, sStatus);

			switch( GetMode() ){
				case MODE_MANUAL_FFT:
					getData(PKT_DATA_MANFFT);
					m_bRun = false;
					break;
				case MODE_AUTOMATIC_FFT:
					getData(PKT_DATA_AUTOFFT);
					if( m_bRun == false ) {
						writeReg((GLOB_CMD+1), (char)GCMD_START_STOP);
						m_Status.running = 0;
						}
					break;
				case MODE_MANUAL_CAPTURE:
					getData(PKT_DATA_MANCAP);
					m_bRun = false;
					break;
				case MODE_REAL_TIME:
					getDataRealTime(PKT_DATA_REALTIME);
					break;
				default:
					DBG("[%s:%s] error \r\n", __FILE__, __func__);
					break;
				}
			}

		}
#endif
	DBG("[%s:%s]  end of loop \r\n", __FILE__,__func__ );

}

bool CSensor::IsRun()
{
	return m_bRun;
}



int CSensor::gpio_is_exported(size_t gpio)
{
	int fd = 0;
	int i;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/direction", gpio);
	fd = open(buf, O_WRONLY);
	if (fd >= 0) {
		close(fd);
		//OK, GPIO is exported
		return 0;
	}

	return -1;
}


int CSensor::gpio_export(size_t gpio)
{
	int fd = 0;
	int i;
	char buf[64] = {0};
	size_t len = 0;

	if( gpio_is_exported(gpio) == 0 )
	{
		return 0; //No need to re-export
	}

	fd = open(CFG_GPIO_DIR "/export", O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	len = snprintf(buf, sizeof(buf), "%lu", gpio);
	write(fd, buf, len);
	close(fd);

	/* wait until file is actually available in user space */
	for (i = 0; i < CFG_GPIO_SYS_FILE_EXPORTED_TIME_IN_100MS; i++)
	{
		if( gpio_is_exported(gpio) == 0 )
		{
			return 0; //GPIO is present in user space
		}
		usleep(100 * 1000);
	}

	return -1;
}

int CSensor::gpio_unexport(size_t gpio)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	fd = open(CFG_GPIO_DIR "/unexport", O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	len = snprintf(buf, sizeof(buf), "%lu", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int CSensor::gpio_set_direction(size_t gpio, bool output)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/direction", gpio);

	fd = open(buf, O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	if(output)
	{
		write(fd, "out", 3);
	}
	else
	{
		write(fd, "in", 2);
	}

	close(fd);

	return 0;
}

int CSensor::gpio_set_edge(size_t gpio, bool rising, bool falling)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/edge", gpio);

	fd = open(buf, O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	if(rising && falling)
	{
		write(fd, "both", 4);
	}
	else if(rising)
	{
		write(fd, "rising", 6);
	}
	else if(falling)
	{
		write(fd, "falling", 7);
	}
	else
	{
		write(fd, "none", 4);
	}

	close(fd);

	return 0;
}

int CSensor::gpio_open(size_t gpio, int mode)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/value", gpio);

	fd = open(buf, mode | O_NONBLOCK );
	if( fd < 0 )
	{
		return -1;
	}

	return fd;
}

int CSensor::Set_Interface_Link(void)
{
	int ret = 0;
	int irq = 0;

	gpio_unexport(IRQ_PIN);
		
	ret = gpio_export(IRQ_PIN);
	if(ret)
	{
		Cleanup_Interface_Link(irq);
		return -1;
	}

	//Set pin directions
	ret = gpio_set_direction(IRQ_PIN, 0);
	if(ret)
	{
		Cleanup_Interface_Link(irq);
		return -1;
	}

	//Set pin edge
	ret = gpio_set_edge(IRQ_PIN, 0, 1);
	if(ret)
	{
		Cleanup_Interface_Link(irq);
		return -1;
	}

	irq = gpio_open(IRQ_PIN, O_RDONLY);
	if(irq < 0)
	{
		Cleanup_Interface_Link(irq);
		return -1;
	}

	return irq;
}

void CSensor::Cleanup_Interface_Link(int irq)
{
	//Close open file descriptors
	if( irq >= 0)
	{
		close( irq);
		irq = -1;
	}

	gpio_unexport(IRQ_PIN);
}

int CSensor::openDev( char *name )
{
	int fd = -1;
	int ret = 0;

	int mode = SPI_MODE_3;
	int speed = SPI_FREQ;
	char bits = SPI_BIT;
	
	if( (fd = open( name, O_RDWR)) < 0 ) 
	{
		DBG("[%s:%s] open err\n", __FILE__, __func__ );
		return fd;
	}
	
	//Set mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if( ret < 0 )
	{
		return ret;
	}
	DBG("[%s:%s] mode=%d\n", __FILE__,__func__, mode);

	//Set
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if( ret < 0 )
	{
		return ret;
	}
	DBG("[%s:%s] bits=%d\n", __FILE__,__func__, bits);

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if( ret < 0 )
	{
		return ret;
	}

	DBG("[%s:%s] speed=%d\n", __FILE__,__func__, speed);

	return fd;
}

void CSensor::closeDev() 
{
	if( m_nFd > 0 )
		close(m_nFd);
}

int CSensor::writeReg(char reg, char val)
{
#if 1
	int nRet;
	SPI_DataType stTemp;

	stTemp.len = 2;
	stTemp.inb[0] = reg|0x80;
	stTemp.inb[1] = val;

	pthread_mutex_lock( &m_Mutex );

	nRet = ioctl(m_nFd, IOCTL_SPI_WRITE1, &stTemp);
	m_cRegister[reg] = val;

	pthread_mutex_unlock( &m_Mutex );
	
#else
	int nRet;
	unsigned char cmd_buf[2] = {0,};
	unsigned char data_buf[2] = {0,};
	struct spi_ioc_transfer xfer[1];

	pthread_mutex_lock( &m_Mutex );
	
	cmd_buf[0] = reg | 0x80; 
	cmd_buf[1] = val; 
	m_cRegister[reg] = cmd_buf[1];
	/* You must set command sequence on cmd_buf */

	xfer[0].tx_buf = (unsigned long) cmd_buf;
	xfer[0].rx_buf = (unsigned long) data_buf;
	xfer[0].len = 2; /* Length of  command to write*/
	xfer[0].cs_change = 0; /* keep CS activated */
	xfer[0].speed_hz = SPI_FREQ;
	xfer[0].bits_per_word = SPI_BIT;
	xfer[0].delay_usecs = 0;


	nRet = ioctl(m_nFd, SPI_IOC_MESSAGE(1), xfer);
//	usleep(17);
	
	pthread_mutex_unlock( &m_Mutex );
#endif	
	return nRet;
}

int CSensor::writeReg(char reg, short val)
{
#if 1
	int nRet;
	SPI_DataType stTemp;

	stTemp.len = 2;
	stTemp.inb[0] = reg+0x80;
	stTemp.inb[1] = (val&0xff);
	stTemp.inb[2] = ((val>>8)&0xff);
	
	pthread_mutex_lock( &m_Mutex );

	nRet = ioctl(m_nFd, IOCTL_SPI_WRITE, &stTemp);
	m_cRegister[reg] = (val&0xff);
	m_cRegister[reg+1] = ((val>>8)&0xff);

	pthread_mutex_unlock( &m_Mutex );	
#else
	int nRet;
	unsigned char cmd_buf[2] = {0,};
	unsigned char data_buf[2] = {0,};
	struct spi_ioc_transfer xfer[1];

	pthread_mutex_lock( &m_Mutex );

	cmd_buf[0] = reg | 0x80; 
	cmd_buf[1] = val & 0xff; 
	m_cRegister[reg] = cmd_buf[1];
	/* You must set command sequence on cmd_buf */

	xfer[0].tx_buf = (unsigned long) cmd_buf;
	xfer[0].rx_buf = (unsigned long) data_buf;
	xfer[0].len = 2; /* Length of  command to write*/
	xfer[0].cs_change = 0; /* keep CS activated */
	xfer[0].speed_hz = SPI_FREQ;
	xfer[0].bits_per_word = SPI_BIT;
	xfer[0].delay_usecs = 0;

	nRet = ioctl(m_nFd, SPI_IOC_MESSAGE(1), xfer);
//	usleep(17);


	cmd_buf[0] = (reg+1) | 0x80; 
	cmd_buf[1] = (val>>8) & 0xff; 
	m_cRegister[reg+1] = cmd_buf[1];

	nRet = ioctl(m_nFd, SPI_IOC_MESSAGE(1), xfer);
//	usleep(17);
	
	pthread_mutex_unlock( &m_Mutex );
#endif	
	return nRet;
}

short CSensor::readReg(char reg)
{
#if 1
	short sData;
	SPI_DataType stTemp;

	stTemp.len = 2;
	stTemp.inb[0] = reg;
	
	pthread_mutex_lock( &m_Mutex );

	sData = ioctl(m_nFd, IOCTL_SPI_READ, &stTemp);
	m_cRegister[reg] = (sData&0xff);
	m_cRegister[reg+1] = ((sData>>8)&0xff);
	usleep(1);
	pthread_mutex_unlock( &m_Mutex );
	
#else
	unsigned char cmd_buf[2] = {0,};
	unsigned char data_buf[2] = {0,};
	short sData;
	struct spi_ioc_transfer xfer[1];

	pthread_mutex_lock( &m_Mutex );

	cmd_buf[0] = reg; 
	cmd_buf[1] = 0;

	/* You must set command sequence on cmd_buf */
	xfer[0].tx_buf = (unsigned long) cmd_buf;
	xfer[0].rx_buf = (unsigned long) data_buf;
	xfer[0].len = 2; /* Length of  command to write*/
	xfer[0].cs_change = 0; /* keep CS activated */
	xfer[0].speed_hz = SPI_FREQ;
	xfer[0].bits_per_word = SPI_BIT;
	xfer[0].delay_usecs = 0;
	
#if 0

	xfer[1].tx_buf = (unsigned long) cmd_buf;
	xfer[1].rx_buf = (unsigned long) data_buf;
	xfer[1].len = 2; /* Length of Data to read */
	xfer[1].cs_change = 0;
	xfer[1].speed_hz = 2500000;
	xfer[1].bits_per_word = 8;
	xfer[1].delay_usecs = 0;

	ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
#endif

	ioctl(m_nFd, SPI_IOC_MESSAGE(1), xfer);
//	usleep(17);

//	printf("cmd=%x,%x\n", cmd_buf[0],cmd_buf[1]);
//	printf("%x, %x\n", data_buf[0],data_buf[1]);
//	printf("0x%x (%d)\n", sData, sData);

	sData = (data_buf[0] << 8) | data_buf[1];	

	m_cRegister[reg] = data_buf[1];
	m_cRegister[reg+1] = data_buf[0];


	pthread_mutex_unlock( &m_Mutex );
#endif	
	return sData;
}

void CSensor::getDataRealTime(char cCmd)
{
	int nLen = 5;

	m_sData[0][m_nIndex++] = readReg(X_BUF);
	m_nIndex %= 512;
	if( m_nIndex == 0 ) {
		MAKE_NETHDR( m_pcBuf, (nLen+1024), cCmd );
		memcpy(m_pcBuf+5, (char*)m_sData[0], 1024);
		SendMessage(0, PKT_DATA_REALTIME, 1029, (int)m_pcBuf);
		NEXT_BUFFER();
		}
}
void CSensor::getDataRealTime(int nFd)
{
	int nLen = 5;
	int nRet = 0;
	char *pBuf = m_pcBuf+5;

	MAKE_NETHDR( m_pcBuf, (nLen+1024), PKT_DATA_REALTIME );
	nRet = read(nFd, pBuf, 1024);
//	memcpy(m_pcBuf+5, pData, 1024);
	SendMessage(0, PKT_DATA_REALTIME, 1029, (int)m_pcBuf);
	NEXT_BUFFER();
}

void CSensor::getData(char cCmd)
{
	int count = 0;
	int nLen = 5;
	short sDiag = 0;
	short sAlmX = 0;
	short sAlmY = 0;
	short sAlmZ = 0;
	
	readReg(DIAG_STAT);
	sDiag = readReg(ALM_X_STAT);
	sAlmX = readReg(ALM_Y_STAT);
	sAlmY = readReg(ALM_Z_STAT);
	sAlmZ = readReg(X_BUF);
	
	//-------------------- x-aix ---------------------------------
	count = 0;
	while(count < (m_nRecord-1)) {
		m_sData[0][count++] = readReg(X_BUF);
		}
	m_sData[0][count] = readReg(Y_BUF);
	
	MAKE_NETHDR( m_pcBuf, (nLen+4+m_nRecord*2), cCmd );
	m_pcBuf[nLen]= 'x';
	m_pcBuf[nLen+1]= ((sDiag>>8)&0xff);
	m_pcBuf[nLen+2]= (sAlmX&0xff);
	m_pcBuf[nLen+3]= ((sAlmX>>8)&0xff);
	
	memcpy(m_pcBuf+9, (char*)m_sData[0], (m_nRecord*2));
	SendMessage(0, cCmd, (9+m_nRecord*2), (int)m_pcBuf);
	NEXT_BUFFER();

	
	//-------------------- y-aix ---------------------------------
	count = 0;
	while(count < (m_nRecord-1)) {
		m_sData[1][count++] = readReg(Y_BUF);
		}
	m_sData[1][count] = readReg(Z_BUF);
	
	MAKE_NETHDR( m_pcBuf, (nLen+4+m_nRecord*2), cCmd );
	m_pcBuf[nLen]= 'y';
	m_pcBuf[nLen+1]= ((sDiag>>8)&0xff);
	m_pcBuf[nLen+2]= (sAlmY&0xff);
	m_pcBuf[nLen+3]= ((sAlmY>>8)&0xff);
	memcpy(m_pcBuf+9, (char*)m_sData[1], (m_nRecord*2));
	SendMessage(0, cCmd, (9+m_nRecord*2), (int)m_pcBuf);
	NEXT_BUFFER();



	//-------------------- z-aix ---------------------------------
	count = 0;
	while(count < m_nRecord) {
		m_sData[2][count++] = readReg(Z_BUF);
		}

	MAKE_NETHDR( m_pcBuf, (nLen+4+m_nRecord*2), cCmd );
	m_pcBuf[nLen]= 'z';
	m_pcBuf[nLen+1]= ((sDiag>>8)&0xff);
	m_pcBuf[nLen+2]= (sAlmZ&0xff);
	m_pcBuf[nLen+3]= ((sAlmZ>>8)&0xff);
	memcpy(m_pcBuf+9, (char*)m_sData[2], (m_nRecord*2));
	SendMessage(0, cCmd, (9+m_nRecord*2), (int)m_pcBuf);
	NEXT_BUFFER();

#if 0
	for(int i=0;i<m_nRecord;i++) {
		printf("%4d : %8d, %8d, %8d\n", 
			i, m_sData[0][i], m_sData[1][i], m_sData[2][i] );			
//		printf("%4d : %8.1f, %8.1f, %8.1f\n", 
//			i, sDat[0][i]*0.61f, sDat[1][i]*0.61f, sDat[2][i]*0.61f );
		}
#endif
	
}

int CSensor::readRegisterAll()
{
	int i;
	int j;
	
	readReg(0);
	for( i=2, j=0;i<=0x7c;i+=2, j++) {
		m_psRegister[j] = readReg(i);
//		DBG("[%s:%s] reg[%02x]=0x%04x (%d)\n", __FILE__, __func__, j*2, m_psRegister[j], m_psRegister[j]);
		}
	
	return 0;
}

int CSensor::readAlarmBands()
{
	int i = 0;
	int nOp = 0;
	ALARM_t alarms[6];

	GetAlarm(nOp, 0, &alarms[0]);
	GetAlarm(nOp, 1, &alarms[1]);
	GetAlarm(nOp, 2, &alarms[2]);
	GetAlarm(nOp, 3, &alarms[3]);	
	GetAlarm(nOp, 4, &alarms[4]);
	GetAlarm(nOp, 5, &alarms[5]);

	memcpy( m_Status.alarms, alarms, sizeof(alarms));
	

	for(i=0;i<6;i++) {
		DBG("[%s:%s] alarm[%d] %x %x %x %x %x %x %x %x\n", __FILE__, __func__,
			i, m_Status.alarms[i].alm_f_l, m_Status.alarms[i].alm_f_h, 
			m_Status.alarms[i].alm_x_mag1, m_Status.alarms[i].alm_x_mag2,
			m_Status.alarms[i].alm_y_mag1, m_Status.alarms[i].alm_y_mag2,			
			m_Status.alarms[i].alm_z_mag1, m_Status.alarms[i].alm_z_mag2);
		}
	return 0;
}
void CSensor::SetNetwork(CWork *pWork)
{
	m_pWork = pWork;
}

int CSensor::GetMode()
{
//	DBG("[%s:%s] m_cRegister[REC_CTRL1]=0x%02x\n", __FILE__, __func__, m_cRegister[REC_CTRL1]);
	return (m_cRegister[REC_CTRL1] & 0x03);
}

void CSensor::SetMode(int nMode)
{
	ioctl(m_nFd, IOCTL_SPI_SET_MODE, nMode);
}

int CSensor::GetScale()
{
//	DBG("[%s:%s] m_cRegister[REC_CTRL1]=0x%02x\n", __FILE__, __func__, m_cRegister[REC_CTRL1]);
	return (m_cRegister[REC_CTRL2] & 0x03);
}

int CSensor::GetBandWidth()
{
//	DBG("[%s:%s] m_cRegister[REC_CTRL1]=0x%02x\n", __FILE__, __func__, m_cRegister[REC_CTRL1]);
	return (m_cRegister[AVG_CNT] & 0x0f);
}

int CSensor::GetRecPeriod()
{
	int nScale = m_cRegister[REC_PRD+1] & 0x03;
	int nData = m_cRegister[REC_PRD];
	int nTime = 0;
	
	switch( nScale ) {
		case 0:
			nTime = nData;
			break;
		case 1:
			nTime = nData*60;
			break;
		case 2:
			nTime = nData*60*60;
			break;
		}

	return nTime;
}

int CSensor::GetStatus()
{
	m_Status.user_id =  (m_cRegister[USER_ID+1]<<8) | m_cRegister[USER_ID];
	m_Status.mode = GetMode();
	m_Status.record_period = GetRecPeriod();
	m_Status.real_axis = REAL_AXIS_X;
	m_Status.scale = GetScale();
	m_Status.bandwidth = GetBandWidth();

	m_Status.alm_ctrl = m_cRegister[ALM_CTRL];


	return 0;
}

int CSensor::GetStatusAlm()
{
	int nRet = 0;
	int nLen = 5;
	ALMSTAT_t almStat;

	
	readReg(DIAG_STAT);
	almStat.diag_stat = readReg(ALM_X_STAT) & 0x3f00;
	almStat.alm_x_stat = readReg(ALM_Y_STAT);
	almStat.alm_y_stat = readReg(ALM_Z_STAT);
	almStat.alm_z_stat = readReg(ALM_X_STAT);
	
	MAKE_NETHDR( m_pcBuf, (nLen+8), PKT_STATUS_ALARM );
	memcpy(m_pcBuf+5, (char*)&almStat, 8);
	SendMessage(0, PKT_STATUS_ALARM, (nLen+8), (int)m_pcBuf);
	NEXT_BUFFER();
	
	DBG("[%s:%s] alarm diag=%x,x=%x, y=%x, z=%x\n", __FILE__, __func__, 
		almStat.diag_stat, almStat.alm_x_stat, almStat.alm_y_stat, almStat.alm_z_stat
		);
	return nRet;
}

int CSensor::SetAlarm(int nOp, int nBand, ALARM_t *pAlarm)
{
	char cVal = 0;
	short sVal = 0;

	cVal = (nBand+1)&0xff;
	writeReg(ALM_PNTR, cVal);

	cVal = (nOp)&0xff;
	writeReg(ALM_PNTR+1, cVal);

	readReg(ALM_PNTR);
	sVal = readReg(ALM_PNTR);
//printf("nOp=%d, nBand=%d, readReg=0x%x \r\n", nOp, nBand, sVal);
	
	
	cVal =pAlarm->alm_f_l;
	writeReg( ALM_F_LOW, cVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_f_l = cVal;
	
	cVal = pAlarm->alm_f_h;
	writeReg( ALM_F_HIGH, cVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_f_h = cVal;
	
	sVal = pAlarm->alm_x_mag1;
	writeReg( ALM_X_MAG1, sVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_x_mag1 = sVal;
	
	sVal = pAlarm->alm_x_mag2;
	writeReg( ALM_X_MAG2, sVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_x_mag2 = sVal;
	
	sVal = pAlarm->alm_y_mag1;
	writeReg( ALM_Y_MAG1, sVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_y_mag1 = sVal;
	
	sVal = pAlarm->alm_y_mag2;
	writeReg( ALM_Y_MAG2, sVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_y_mag2 = sVal;
	
	
	sVal = pAlarm->alm_z_mag1;
	writeReg( ALM_Z_MAG1, sVal);
	if( nOp ==  0) m_Status.alarms[nBand].alm_z_mag1 = sVal;

	sVal = pAlarm->alm_z_mag2;
	writeReg( ALM_Z_MAG2, sVal);
	if( nOp == 0 ) m_Status.alarms[nBand].alm_z_mag2 = sVal;

	writeReg(GLOB_CMD+1, (char)GCMD_SAVE_ALM);
	usleep(461);
	
	DBG("[%s:%s] nOp=%d, nBand=%d, %d, %d, %x, %x, %x, %x, %x, %x \n", __FILE__, __func__,
		nOp, nBand,
		pAlarm->alm_f_l, pAlarm->alm_f_h,
		pAlarm->alm_x_mag1, pAlarm->alm_x_mag2,
		pAlarm->alm_y_mag1, pAlarm->alm_y_mag2,		
		pAlarm->alm_z_mag1, pAlarm->alm_z_mag2	);	
	
	return 0;
	
}

int CSensor::GetAlarm(int nOp, int nBand, ALARM_t *pAlarm)
{
	char cVal = 0;
	short sVal = 0;
	
	cVal = (nBand+1)&0xff;
	writeReg(ALM_PNTR, cVal);

	cVal = (nOp & 0xff);
	writeReg(ALM_PNTR+1, cVal);


	writeReg(GLOB_CMD+1, (char)0x40);	// retrieve spectal alarm info
	usleep(40);
	
	readReg( ALM_F_LOW);
	sVal = readReg( ALM_F_HIGH);
	pAlarm->alm_f_l = sVal & 0xff;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_f_l = pAlarm->alm_f_l;

	sVal = readReg( ALM_X_MAG1);
	pAlarm->alm_f_h = sVal & 0xff;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_f_h = pAlarm->alm_f_h;
	
	sVal = readReg( ALM_X_MAG2);
	pAlarm->alm_x_mag1 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_x_mag1 = pAlarm->alm_x_mag1;
	
	sVal = readReg( ALM_Y_MAG1);
	pAlarm->alm_x_mag2 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_x_mag2 = pAlarm->alm_x_mag2;
	
	sVal = readReg( ALM_Y_MAG2);
	pAlarm->alm_y_mag1 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_y_mag1 = pAlarm->alm_y_mag1;
		
	sVal = readReg( ALM_Z_MAG1);
	pAlarm->alm_y_mag2 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_y_mag2 = pAlarm->alm_y_mag2;
		
	sVal = readReg( ALM_Z_MAG2);
	pAlarm->alm_z_mag1 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_z_mag1 = pAlarm->alm_z_mag1;
		
	sVal = readReg( ALM_Z_MAG2);
	pAlarm->alm_z_mag2 = sVal;
	if( nOp == 0 ) m_Status.alarms[nBand].alm_z_mag2 = pAlarm->alm_z_mag2;
	
	DBG("[%s:%s] nOp=%d, nBand=%d, %d, %d, %d, %d, %d, %d, %d, %d \n", __FILE__, __func__,
		nOp, nBand,
		pAlarm->alm_f_l, pAlarm->alm_f_h,
		pAlarm->alm_x_mag1, pAlarm->alm_x_mag2,
		pAlarm->alm_y_mag1, pAlarm->alm_y_mag2,		
		pAlarm->alm_z_mag1, pAlarm->alm_z_mag2	);	
	
	return 0;
}


int CSensor::SaveConfigAlm()
{
	writeReg(GLOB_CMD+1, (char)GCMD_SAVE_ALM);
	usleep(461);

	DBG("[%s:%s] \n", __FILE__, __func__);	
	return 0;
}

int CSensor::ClearConfigAlm()
{
	writeReg(GLOB_CMD+1, (char)GCMD_CLR_ALM_FL);
	usleep(25800);
	
	DBG("[%s:%s] \n", __FILE__, __func__);		
	return 0;
}

int CSensor::EnableAlarm(char cSet)
{
	writeReg(ALM_CTRL, cSet);
	usleep(17);
	
	DBG("[%s:%s] cSet=0x%x \n", __FILE__, __func__, cSet);	
	return 0;
}

int CSensor::ClearAlarm()
{
	m_bClearAlarm = true;
	
	DBG("[%s:%s] \n", __FILE__, __func__);	
	return 0;
}

