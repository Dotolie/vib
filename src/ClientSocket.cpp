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
//  File           : ClientSocket.cpp
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include "ClientSocket.h"
#include "SocketException.h"


CClientSocket::CClientSocket ( std::string host, int port )
{
	if ( ! CSocket::create() )
	{
		throw SocketException ( "Could not create client socket." );
	}

	if ( ! CSocket::connect ( host, port ) )
	{
		throw SocketException ( "Could not bind to port." );
	}

}


