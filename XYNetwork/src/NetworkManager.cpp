//
//  NetworkManager.cpp
//  XYNetworkTest
//
//  Created by xiao on 15/3/19.
//
//

#include "NetworkManager.h"

using namespace XYNetwork;
using namespace cocos2d;

NetworkManager* NetworkManager::ShareInstance()
{
    static NetworkManager* p = nullptr;
    if(p == nullptr)
    {
        p = new NetworkManager;
    }
    return p;
}

NetworkManager::NetworkManager()
{
    _networkInterface = XYNetworkInterface::GetNetworkInstance();
}

NetworkManager::~NetworkManager()
{
    delete _networkInterface;
}

void NetworkManager::initServer()
{
    Node::init();
    
    _networkInterface->AddEventListener(XYNetworkInterface::NetworkEvent_Connect, std::bind(&NetworkManager::OnConnectCall, this, std::placeholders::_1, std::placeholders::_2));
    _networkInterface->AddEventListener(XYNetworkInterface::NetworkEvent_Disconnect, std::bind(&NetworkManager::OnDisconnectCall, this, std::placeholders::_1, std::placeholders::_2));
    _networkInterface->AddEventListener(XYNetworkInterface::NetworkEvent_ReceiveData, std::bind(&NetworkManager::OnReceiveDataCall, this, std::placeholders::_1, std::placeholders::_2));
    _networkInterface->AddEventListener(XYNetworkInterface::NetworkEvent_Error, std::bind(&NetworkManager::OnErrorCall, this, std::placeholders::_1, std::placeholders::_2));
    
    Director::getInstance()->getScheduler()->unschedule(schedule_selector(NetworkManager::update), this);
    Director::getInstance()->getScheduler()->schedule(schedule_selector(NetworkManager::update),this,0.1,false);
}

void NetworkManager::update(float delta)
{
    _networkInterface->ProcessEvent();
}

void NetworkManager::ConnectToServer(std::string host, long port)
{
    _networkInterface->Connect(host, port);
    
//    hostent *host_entry = gethostbyname(host.c_str());
//    char IPStr[64] = {0};
//    if (host_entry != 0)
//    {
//        sprintf(IPStr, "%d.%d.%d.%d",
//                (host_entry->h_addr_list[0][0]&0x00ff), (host_entry->h_addr_list[0][1]&0x00ff),
//                (host_entry->h_addr_list[0][2]&0x00ff), (host_entry->h_addr_list[0][3]&0x00ff));
//        _networkInterface->Connect(IPStr, port);
//    }
}

void NetworkManager::Disconnect()
{
    _networkInterface->Disconnect();
}

void NetworkManager::SendMsg(const XYNetwork::XYPacketHeader* header, const void* data, int32_t len)
{
    _networkInterface->SendMsg(header, data, len);
}

void NetworkManager::OnConnectCall(const char* data, long len)
{
    CCLOG("connect success");
}

void NetworkManager::OnDisconnectCall(const char* data, long len)
{
    const XYNetwork::XYNetworkErrorCode* code = (const XYNetwork::XYNetworkErrorCode*)data;
    std::cout << "disconnect code: " << code->GetCode() << std::endl;
}

void NetworkManager::OnReceiveDataCall(const char* data, long len)
{
    CCLOG("receive success");

    const XYPacketHeader* pHeader = (const XYPacketHeader*)data;
    
    
    if(pHeader->msg_type == 103 && pHeader->result == 0)
    {
        const char* body = pHeader->body;
        const stMsgSer2Cli* pMsg = (const stMsgSer2Cli*)body;
        
        std::cout << pMsg->flag << std::endl;
        std::cout << pMsg->session << std::endl;
    }
    else
    {
        std::cout << "data error: " << pHeader->result << std::endl;
    }
    
//    XYPacketHeader* pHeader = (struct XYPacketHeader*)malloc(len);
//    memcpy(pHeader, data, len);
//    
//    char* body = pHeader->body;
//    stMsgSer2Cli* pMsg = (stMsgSer2Cli*)body;
//    
//    free(pHeader);
    
//    const XYPacketHeader* pHeader = (const XYPacketHeader*)data;
//    int iLen = pHeader->len;
}

void NetworkManager::OnErrorCall(const char* data, long len)
{
    const XYNetwork::XYNetworkErrorCode* code = (const XYNetwork::XYNetworkErrorCode*)data;
    std::cout << "error code: " << code->GetCode() << std::endl;
}












