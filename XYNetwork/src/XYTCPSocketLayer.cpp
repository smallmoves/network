/*
 * XYTCPSocketLayer.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYTCPSocketLayer.h"
#include "XYDataHandler.h"
#include "XYExecutor.h"
#include "XYThreadManager.h"

#if defined(__ANDROID__)
#define SO_NOSIGPIPE MSG_NOSIGNAL
#endif

//#if defined(__APPLE__)
//#define MSG_NOSIGNAL SO_NOSIGPIPE
//#endif

namespace XYNetwork{

//void sig_pipe(int signo)
//{
//    int i = signo;
//}
//
//void (*Signal(int, void(*func)(int)))(int);
//typedef void Sigfunc(int);
//
//Sigfunc* Signal(int signo, Sigfunc* func)
//{
//    struct sigaction act, oact;
//    act.sa_handler = func;
//    sigemptyset(&act.sa_mask);
//    act.sa_flags = 0;
//
//    if(sigaction(signo, &act, &oact) < 0)
//    {
//        return (SIG_ERR);
//    }
//    return oact.sa_handler;
//}

XYTCPSocketLayer::XYTCPSocketLayer()
: _fd(0)
, _stat(States_Disconnected)
, _host("")
, _port(0)
, _pid(0)
, _IsReadThreadRunning(false)
{
	pthread_mutex_init(&_mutex,NULL);
}

XYTCPSocketLayer::~XYTCPSocketLayer()
{
	_IsReadThreadRunning = false;
	pthread_mutex_destroy(&_mutex);
}

void XYTCPSocketLayer::Connect(std::string host, long port)
{
//    struct sigaction sa;
//    sa.sa_handler = SIG_IGN;
//    sigaction( SIGPIPE, &sa, 0 );
    
	_host = host;
	_port = port;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_pid, &attr, (void*(*)(void*))&XYTCPSocketLayer::ConnectThread, this);
}

void* XYTCPSocketLayer::ConnectThread(void* arg)
{
	XYTCPSocketLayer* p = static_cast<XYTCPSocketLayer*>(arg);
	p->ConnectToServer();
	return nullptr;
}

void XYTCPSocketLayer::ConnectToServer()
{
	if(_stat != States_Disconnected)
	{
		return;
	}
	_stat = States_Connecting;

	pthread_mutex_lock(&_mutex);
	if(_stat != States_Connecting)
	{
		return;
	}

	if(_IsReadThreadRunning)
	{
		_IsReadThreadRunning = false;
		usleep(10000);
	}

	struct sockaddr_in servaddr;
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd < 0)
	{
		_stat = States_Disconnected;
		//dispatch error msg
        onError(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::CREATE_SOCKET_ERROR));

		return;
	}
    
    //no SIGPIPE
    int set = 1;
    setsockopt(_fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(_port);
	inet_pton(AF_INET, _host.c_str(), &servaddr.sin_addr);
	if(connect(_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
	{
		_stat = States_Disconnected;
		//dispatch connect error
        onError(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::CONNECT_ERROR));

		return;
	}

	_stat = States_Connected;
	_IsReadThreadRunning = true;
	pthread_mutex_unlock(&_mutex);

    XYThreadManager::ShareIntance()->Start();
    
	//callback
	onConnect();

	//read schedule
	ReadSocket();
    
	return;
}

bool XYTCPSocketLayer::IsConnected()
{
	return _stat == States_Connected;
}

void XYTCPSocketLayer::Disconnect(XYNetworkErrorCode code)
{
	if(_stat == States_Connected)
	{
		_stat = States_Disconnected;
		_IsReadThreadRunning = false;
	}
	else if(_stat == States_Connecting)
	{
		pthread_mutex_lock(&_mutex);

		_stat = States_Disconnected;
		_IsReadThreadRunning = false;

		pthread_mutex_unlock(&_mutex);
	}

    XYThreadManager::ShareIntance()->Stop();
    
	if(shutdown(_fd, SHUT_WR) != 0)
	{
		//dispatch shutdown error
        onError(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::SHOWDOWN_ERROR));
	}
    
    onDisconnect(code);
}

void XYTCPSocketLayer::ReadSocket()
{
	while(_IsReadThreadRunning)
	{
		usleep(5000);

		if(!_IsReadThreadRunning)
		{
			break;
		}

		char* pOut = new char[MAX_BYTE_READ_ONCE];
		ssize_t readSize = Readn(_fd, pOut, MAX_BYTE_READ_ONCE);
		if(readSize > 0)
		{
			//dispatch data
			onReceive(pOut, readSize);

			bzero(pOut, MAX_BYTE_READ_ONCE);
			delete [] pOut;
		}
		else if(readSize == 0)
		{
			//dispatch disconnect
            onDisconnect(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::SERVER_DISCONNECT));

			bzero(pOut, MAX_BYTE_READ_ONCE);
			delete [] pOut;

			_IsReadThreadRunning = false;
			_stat = States_Disconnected;
			break;
		}
		else
		{
			//dispatch read error
            onError(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::READ_ERROR));
			bzero(pOut, MAX_BYTE_READ_ONCE);
			delete [] pOut;
		}
	}
	return;
}

void XYTCPSocketLayer::SendMsg(char* data, long len)
{
	std::function<void(char*, long)> func = std::bind(&XYTCPSocketLayer::WriteSocket, this, std::placeholders::_1, std::placeholders::_2);
	XYExecutor* pXYExecutor = new XYExecutor(data, len, func);
	XYThreadManager::ShareIntance()->EnqueueOutData(pXYExecutor);
}

void XYTCPSocketLayer::WriteSocket(const char* data, long len)
{
	if( Writen(_fd, data, len) < 0 )
	{
		//dispatch write error
        onError(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::WRITE_ERROR));
		return;
	}
}

ssize_t XYTCPSocketLayer::Readn(int fd, void* vptr, size_t n)
{
    ssize_t nread;
    while(1)
    {
    	nread = read(fd, vptr, n);
        if( nread < 0 )
        {
            if(errno == EINTR)
            {
            	XYLog::Log("interrupt");
                nread = 0;
            }
            else
            {
            	XYLog::Log("error");
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    return nread;
}

ssize_t XYTCPSocketLayer::Writen(int fd, const void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char* ptr = (const char *)vptr;
    nleft = n;
    while(nleft > 0)
    {
        if( (nwritten = write(fd, ptr, nleft)) <= 0 )
        {
            if(nwritten < 0 && errno == EINTR)
            {
                nwritten = 0;
            }
            else if(nwritten < 0 && errno == EPIPE)
            {
                Disconnect(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::SERVER_DISCONNECT));
                return -1;
            }
            else
            {
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

}   //namespace XYNetwork


