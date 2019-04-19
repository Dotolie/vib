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
//  File           : object.h
//  Description :
//  Author      : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "msg.h"


typedef int (*funcPtr)(int, int, int, int);

int SendMessage(int hdl, int message, int wparam, int lparam);


class CObject
{     
protected:
	static char szAppName();
	int		m_hdl;
	MSG		m_msg;
	funcPtr 	m_callback;
	
public:
	CObject();
	~CObject();
	int InitInstance(funcPtr);
	void Run();
	int	ExitInstance();

}; 


#endif /* __OBJECT_H__ */

