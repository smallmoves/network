/*
 * XYTCPSocketLayer.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYTCPSOCKETLAYER_H_
#define XYTCPSOCKETLAYER_H_

#include "XYCommonDefine.h"

namespace XYNetwork{

class XYTCPSocketLayer
{
public:
	enum States
	{
		States_Disconnected,
		States_Connecting,
		States_Connected
	};
public:
	XYTCPSocketLayer();
	~XYTCPSocketLayer();

	void Connect(std::string host, long port);
	bool IsConnected();
	void Disconnect(XYNetworkErrorCode code);
	void SendMsg(char* data, long len);

private:
	static void* ConnectThread(void* arg);
	void ConnectToServer();
	void ReadSocket();
	void WriteSocket(const char* data, long len);

	ssize_t Readn(int fd, void* vptr, size_t n);
	ssize_t Writen(int fd, const void* vptr, size_t n);

public:
	std::function<void()> onConnect;
    std::function<void(XYNetworkErrorCode)> onDisconnect;
	std::function<void(char*, long)> onReceive;
	std::function<void(XYNetworkErrorCode)> onError;

private:
	int _fd;
	States _stat;
	std::string _host;
	long _port;
	pthread_t _pid;
	pthread_mutex_t _mutex;

	bool _IsReadThreadRunning;

};

} //namespace XYNetwork

#endif /* XYTCPSOCKETLAYER_H_ */
