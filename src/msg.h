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
//  File           : msg.h
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __MSG_H__
#define __MSG_H__

#include <pthread.h>


#define MAX_Q_SIZE	512



typedef struct Msg_t
{     
	int hdl;
	int message;
	int wparam;
	int lparam;
} MSG, *PMSG; 


class CMessageQ
{
protected:
	MSG m_buffer[MAX_Q_SIZE];
	int m_in;
	int m_out;
	int m_count;
	pthread_mutex_t	m_Mutex;

public:
	CMessageQ();
	~CMessageQ();

	int push(MSG);
	int pop(MSG&);
	int get(MSG& data);
	bool IsEmpty();
	bool IsFull();
	int GetSize();
};

#endif /* __MSG_H__ */

