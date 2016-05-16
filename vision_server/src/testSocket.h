#ifndef TESTSOCKET_H_
#define TESTSOCKET_H_

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "socket/socket.h"

#include "ocvApp.h"

class ocvApp;

class testSocket
{
public:
	testSocket();
	void doCommunication(ocvApp* myApp);
	virtual ~testSocket();
public:
	ocvApp* myApp;
	boost::mutex m_mutex;
};

#endif /*TESTSOCKET_H_*/
