/*
 * XYBitSwarm.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYBITSWARM_H_
#define XYBITSWARM_H_

#include "XYCommonDefine.h"
#include "XYTCPSocketLayer.h"
#include "XYDataHandler.h"
#include "XYNetworkInterface.h"

namespace XYNetwork{

class XYBitSwarm : public XYNetworkInterface
{
public:
	XYBitSwarm();
	virtual ~XYBitSwarm();

	void Init();
	virtual void Connect(std::string host, long port);
	virtual void Disconnect();
	virtual void SendMsg(const XYPacketHeader* header, const void* data, int32_t len);

private:
	void OnConnectCall();
    void OnDisconnectCall(XYNetworkErrorCode code);
	void OnSendCall(char* data, long len);
	void OnReceiveCall(char* data, long len);
	void OnErrorCall(XYNetworkErrorCode code);
    void OnHandlerReceiveDataCall(const std::string& data);
    void OnInvalidDataCall(XYNetworkErrorCode code);

private:
	XYTCPSocketLayer* _socketLayer;
	XYDataHandler* _dataHandler;


};

}   //namespace XYNetwork

#endif /* XYBITSWARM_H_ */
