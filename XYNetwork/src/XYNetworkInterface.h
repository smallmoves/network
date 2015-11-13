/*
 * XYNetworkInterface.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYNETWORKINTERFACE_H_
#define XYNETWORKINTERFACE_H_

#include "XYCommonDefine.h"

namespace XYNetwork{

class NetworkEvent;
class XYNetworkInterface
{
public:
    enum NetworkEventType
    {
        NetworkEvent_Connect,
        NetworkEvent_Disconnect,
        NetworkEvent_ReceiveData,
        NetworkEvent_Error,
    };
    
public:
    static XYNetworkInterface* GetNetworkInstance();
    XYNetworkInterface();
    virtual ~XYNetworkInterface();
    
    virtual void Connect(std::string host, long port) = 0;
    virtual void Disconnect() = 0;
    virtual void SendMsg(const XYPacketHeader* header, const void* data, int32_t len) = 0;
    
    void ProcessEvent();
    void SendEvent(NetworkEvent* pEvent);
    
    void AddEventListener(NetworkEventType event, std::function<void(const char*, long)> fun);
    void RemoveEventListener(NetworkEventType event);
    void RemoveAllListener();
    
private:
    void DispatchEvent(NetworkEvent* pEvent);
    void ClearEventQueue();
    
private:
    pthread_mutex_t _mutex;
    
    std::vector<NetworkEvent*> _eventQueue;
    std::map<NetworkEventType, std::function<void(const char*, long)> > _listeners;
};
    
class NetworkEvent
{
public:
    NetworkEvent(XYNetworkInterface::NetworkEventType event, const char* data, long len);
    ~NetworkEvent();
    
    XYNetworkInterface::NetworkEventType GetEvent() { return _event; }
    char* GetData() { return _data; }
    long GetDataLen() { return _len; }
    
private:
    XYNetworkInterface::NetworkEventType _event;
    char* _data;
    long _len;
};

}   //namespace XYNetwork


#endif /* XYNETWORKINTERFACE_H_ */
