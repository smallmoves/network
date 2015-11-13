/*
 * XYBitSwarm.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYBitSwarm.h"

namespace XYNetwork{

XYBitSwarm::XYBitSwarm()
{
	_socketLayer = new XYTCPSocketLayer;
	_dataHandler = new XYDataHandler;
	Init();
}

XYBitSwarm::~XYBitSwarm()
{
	delete _socketLayer;
	delete _dataHandler;
}

void XYBitSwarm::Init()
{
	_socketLayer->onConnect = std::bind(&XYBitSwarm::OnConnectCall, this);
    _socketLayer->onDisconnect = std::bind(&XYBitSwarm::OnDisconnectCall, this, std::placeholders::_1);
	_socketLayer->onReceive = std::bind(&XYBitSwarm::OnReceiveCall, this, std::placeholders::_1, std::placeholders::_2);
	_socketLayer->onError = std::bind(&XYBitSwarm::OnErrorCall, this, std::placeholders::_1);
	_dataHandler->onSend = std::bind(&XYBitSwarm::OnSendCall, this, std::placeholders::_1, std::placeholders::_2);
    _dataHandler->onHandlerReceiveData = std::bind(&XYBitSwarm::OnHandlerReceiveDataCall, this, std::placeholders::_1);
    _dataHandler->onInvalidData = std::bind(&XYBitSwarm::OnInvalidDataCall, this, std::placeholders::_1);
}

void XYBitSwarm::Connect(std::string host, long port)
{
	_socketLayer->Connect(host, port);
}

void XYBitSwarm::Disconnect()
{
	_socketLayer->Disconnect(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::CLIENT_DISCONNECT));
}

void XYBitSwarm::SendMsg(const XYPacketHeader* header, const void* data, int32_t len)
{
    if(!_socketLayer->IsConnected())
    {
        return;
    }
    
	_dataHandler->HandleWriteData(header, (const char*)data, len);
}

void XYBitSwarm::OnSendCall(char* data, long len)
{
	_socketLayer->SendMsg(data, len);
}

void XYBitSwarm::OnReceiveCall(char* data, long len)
{
	_dataHandler->HandleReadData(data, len);
}
    
void XYBitSwarm::OnConnectCall()
{
    NetworkEvent* pEvent = new NetworkEvent(XYNetworkInterface::NetworkEvent_Connect, nullptr, 0);
    SendEvent(pEvent);
}
    
void XYBitSwarm::OnDisconnectCall(XYNetworkErrorCode code)
{
	_dataHandler->ClearData();

    NetworkEvent* pEvent = new NetworkEvent(XYNetworkInterface::NetworkEvent_Disconnect, (const char*)&code, sizeof(XYNetworkErrorCode));
    SendEvent(pEvent);
}

void XYBitSwarm::OnErrorCall(XYNetworkErrorCode code)
{
	_dataHandler->ClearData();

    NetworkEvent* pEvent = new NetworkEvent(XYNetworkInterface::NetworkEvent_Error, (const char*)&code, sizeof(XYNetworkErrorCode));
    SendEvent(pEvent);
}
    
void XYBitSwarm::OnHandlerReceiveDataCall(const std::string& data)
{
    NetworkEvent* pEvent = new NetworkEvent(XYNetworkInterface::NetworkEvent_ReceiveData, data.c_str(), data.size() + 1);
    SendEvent(pEvent);
}
    
void XYBitSwarm::OnInvalidDataCall(XYNetworkErrorCode code)
{
    _socketLayer->Disconnect(code);
}

}   //namespace XYNetwork













