/*
 * XYNetworkInterface.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYNetworkInterface.h"
#include "XYBitSwarm.h"

namespace XYNetwork{

XYNetworkInterface* XYNetworkInterface::GetNetworkInstance()
{
    return new XYBitSwarm;
}

XYNetworkInterface::XYNetworkInterface()
{
    _listeners.clear();
    ClearEventQueue();
    
    pthread_mutex_init(&_mutex,NULL);
}
    
XYNetworkInterface::~XYNetworkInterface()
{
    _listeners.clear();
    ClearEventQueue();
    
    pthread_mutex_destroy(&_mutex);
}
    
void XYNetworkInterface::ProcessEvent()
{
    pthread_mutex_lock(&_mutex);
    
    if(_eventQueue.empty())
    {
        pthread_mutex_unlock(&_mutex);
        return;
    }
    
    std::vector<NetworkEvent*> events(_eventQueue);
    _eventQueue.clear();
    
    pthread_mutex_unlock(&_mutex);
    
    std::vector<NetworkEvent*>::iterator iter = events.begin();
    for(; iter != events.end(); iter++)
    {
        DispatchEvent(*iter);
        
        delete *iter;
        *iter = nullptr;
    }
    events.clear();
}
    
void XYNetworkInterface::SendEvent(NetworkEvent* pEvent)
{
    pthread_mutex_lock(&_mutex);
    _eventQueue.push_back(pEvent);
    pthread_mutex_unlock(&_mutex);
}

void XYNetworkInterface::AddEventListener(NetworkEventType event, std::function<void(const char*, long)> fun)
{
    std::map<NetworkEventType, std::function<void(const char*, long)> >::iterator iter = _listeners.find(event);
    if(iter != _listeners.end())
    {
        _listeners.erase(iter);
    }
    
    _listeners.insert(std::pair<NetworkEventType, std::function<void(const char*, long)> >(event, fun));
}
    
void XYNetworkInterface::RemoveEventListener(NetworkEventType event)
{
    std::map<NetworkEventType, std::function<void(const char*, long)> >::iterator iter = _listeners.find(event);
    if(iter != _listeners.end())
    {
        _listeners.erase(iter);
    }
}
    
void XYNetworkInterface::RemoveAllListener()
{
    _listeners.clear();
}
    
void XYNetworkInterface::DispatchEvent(NetworkEvent* pEventData)
{
    std::map<NetworkEventType, std::function<void(const char*, long)> >::iterator iter = _listeners.find(pEventData->GetEvent());
    if(iter != _listeners.end())
    {
        std::function<void(const char*, long)> fun = iter->second;
        fun(pEventData->GetData(), pEventData->GetDataLen());
    }
}
    
void XYNetworkInterface::ClearEventQueue()
{
    std::vector<NetworkEvent*>::iterator iter = _eventQueue.begin();
    for(; iter != _eventQueue.end(); iter++)
    {
        delete *iter;
        *iter = nullptr;
    }
    _eventQueue.clear();
}
    

    
//NetworkEvent
NetworkEvent::NetworkEvent(XYNetworkInterface::NetworkEventType event, const char* data, long len)
: _event(event)
, _len(len)
{
    if(len > 0)
    {
        _data = new char[len];
        bzero(_data, len);
        memcpy(_data, data, len);
    }
}
    
NetworkEvent::~NetworkEvent()
{
    if(_len > 0)
    {
        delete [] _data;
    }
}
    
    
}   //namespace XYNetwork


