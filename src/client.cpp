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
//  File           : client.cpp
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packet.h"
#include "parser.h"



int main ( int argc, char **argv )
{
	try
	{
		CClientSocket client_socket ( "192.168.1.252", 15001 );

		char reply;
		char ch;
		char data[256];
		int ret;
		int size;
		CParser parser;
		short cmd;
		char *pdata = NULL;

			
		MAKE_NETHDR(data, 16+4, PKT_ALIVE);
		data[16+0]= 0x11;
		data[16+1]= 0x22;
		data[16+2]= 0x33;
		data[16+3]= 0x44;

		for(int i=0;i<20;i++) {
			printf("data[%d]=0x%x\r\n", i, data[i]);
			}
		
		std::cout << "connected" << std::endl;

		while(1) 
		{
			try
			{
				printf("Enter message : ");
				gets( &ch);
				printf("[%c]\n", ch);
				switch(ch) {
					case '0':
						client_socket.send(data, 16+4);
						break;
					case 'q':
						throw SocketException ( "manual stop." );		
						break;
					default:
						break;
					}
			}
			catch ( SocketException& ) 
			{ 
				throw SocketException ( "Could not send to socket." );
			}

			try {
				while(1) {
					ret = client_socket.recv(&ch);
					if( ret < 0 ) 
						throw SocketException ( "Could not send to socket." );
					if( parser.decode(ch) > 0 ) {
						cmd = parser.GetCmd();
						size = parser.GetSize();
						pdata = parser.GetData();
						printf("size=%d, cmd=0x%x, \r\n", size, cmd );
						break;
						}
					}
				}
			catch ( SocketException& ) 
			{ 
				throw SocketException ( "Could not send to socket." );
			}
		}

	}
	catch ( SocketException& e )
	{
		std::cout << "Exception was caught:" << e.description() << "\n";
	}

	return 0;
}

