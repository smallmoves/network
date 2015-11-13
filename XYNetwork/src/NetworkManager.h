//
//  NetworkManager.h
//  XYNetworkTest
//
//  Created by xiao on 15/3/19.
//
//

#ifndef __XYNetworkTest__NetworkManager__
#define __XYNetworkTest__NetworkManager__

#include "cocos2d.h"
#include "XYNetworkInterface.h"

//test
#pragma pack(push)
#pragma pack(1)

struct stMsgCli2Ser
{
    char account_email[64];
    char ud[64];
    char account_pwd[32];
    uint16_t channel_id;
    uint16_t which_game;
};

struct stMsgSer2Cli
{
    uint32_t flag;
    char session[16];
};

typedef struct
{
    uint32_t golds;
    uint32_t golds_change;
    uint32_t diamonds;
    uint32_t diamonds_change;
    uint8_t op_type;
}golds_monitor_req_t;

#pragma pack(pop)


class NetworkManager  : public cocos2d::Node
{
public:
    static NetworkManager* ShareInstance();
    
    NetworkManager();
    ~NetworkManager();
    void initServer();
    virtual void update(float delta);
    
    void ConnectToServer(std::string host, long port);
    void Disconnect();
    void SendMsg(const XYNetwork::XYPacketHeader* header, const void* data, int32_t len);
    
    void OnConnectCall(const char* data, long len);
    void OnDisconnectCall(const char* data, long len);
    void OnReceiveDataCall(const char* data, long len);
    void OnErrorCall(const char* data, long len);
    
private:
    XYNetwork::XYNetworkInterface* _networkInterface;
};

#endif /* defined(__XYNetworkTest__NetworkManager__) */
