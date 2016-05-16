#include "testSocket.h"

#include <iostream>
#include <string.h>

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "socket/socket.h"

using namespace std;

testSocket::testSocket()
{
}

testSocket::~testSocket()
{
}

void testSocket ::doCommunication(ocvApp* myApp){

	SocketServer in(3000,2);
	while (1) {
		Socket* s = in.Accept();
		cout << "Client Connected to Request Server" << endl;
		int i = 0;
		while (1) {
			
			if (myApp->f->frameIsClosed){
				cout<<"BREAk"<<endl;
				exit(0);
			}
			
			string inputCommand = s->ReceiveLine();
			
			if (inputCommand.empty())
				break;
			
			string returnCommand = myApp->camera->processor->getResult();
			s->SendLine(returnCommand);
		}
		
		cout << "Client Disconnected from Request Server" << endl;
	}
}

