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
//  File           : work.cpp
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>

#include "object.h"
#include "packet.h"
#include "message.h"
#include "work.h"

#define DEBUG
#include "debug.h"

#define PORT_NO		15001



CWork::CWork() 
{
}

CWork::~CWork() 
{
	DBG("[%s:%s]  destroy \r\n", __FILE__,__func__ );
	
	if( m_pServer )
		delete m_pServer;

}

void CWork::Stop()
{
	m_bRun = false;
//	DBG("[%s:%s]  m_brun= false\r\n", __FILE__,__func__);
}

void CWork::Run()
{
	int ret = 0;
	char ch;
	short cmd;
	int size;
	char* pdata = NULL;

	// Create the socket
	m_pServer = new CServerSocket( PORT_NO );

	m_bRun = true;
	
	while(m_bRun) {
		try
		{
			DBG("[%s:%s] accept \r\n", __FILE__,__func__ );
			m_pServer->accept ( m_Sock );
			DBG("[%s:%s] new accepted \r\n", __FILE__,__func__ );

			SendMessage(0, PKT_GREETING, 0, 0);

			while ( true )
			{
				ret = m_Sock.recv(&ch);
//				DBG("[%s:%s]  ret=%d, recv=0x%x \r\n", __FILE__,__func__, ret, ch);			
				if( ret < 0 ) {
					throw SocketException ( "can't read." );
					}
				else if ( ret == 0 ) {
					throw SocketException ( "disconnect." );						
					}
				if( m_Parser.decode(ch) ) {
//					SendMessage(0, PKT_ACK, 0, 0);
					cmd = m_Parser.GetCmd();
					size = m_Parser.GetSize();
					pdata = m_Parser.GetData();
//					if( cmd != PKT_ALIVE )
					SendMessage(0, cmd, size, (int)((long long)pdata));
//					DBG("[%s:%s] \033[7;31m cmd=0x%x \033[0m, size=%d, pdata=%p \r\n", __FILE__, __func__, cmd, size, pdata );					
					}
				}
			}
		catch ( SocketException& e ) {
			DBG( "[%s:%s] close socket (%s)\r\n", __FILE__,__func__, e.description().c_str());
			SendMessage(0, MSG_DISCONNECT, 0, 0);
			m_Sock.close();
			}
		}

	DBG("[%s:%s]  end of loop \r\n", __FILE__,__func__ );

	if( m_pServer != NULL ) {
		m_pServer->close();
		delete m_pServer;
		m_pServer = NULL;
		}

}

bool CWork::IsRun()
{
	return m_bRun;
}

