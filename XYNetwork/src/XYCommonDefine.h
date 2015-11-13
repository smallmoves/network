/*
 * XYCommonDefine.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYCOMMONDEFINE_H_
#define XYCOMMONDEFINE_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>

namespace XYNetwork{
    
#define MAX_BYTE_READ_ONCE 1024*4
#define PACKET_MAX_BYTE 1024*8

#pragma pack(push)
#pragma pack(1)
    
//struct XYPacketHeader
//{
//    uint32_t len;
//    uint16_t msg_type;
//    uint32_t user_id;
//    uint32_t result;
//    char body[0];
//};
    
struct XYPacketHeader
{
    uint32_t len;
    uint32_t seq_id;
    uint16_t msg_type;
    uint32_t result;
    uint32_t user_id;
    char body[0];
};
    
#pragma pack(push)

class XYNetworkErrorCode
{
public:
    enum eXYNetworkErrorCode
    {
        CREATE_SOCKET_ERROR,
        CONNECT_ERROR,
        SHOWDOWN_ERROR,
        READ_ERROR,
        WRITE_ERROR,
        
        CLIENT_DISCONNECT,
        SERVER_DISCONNECT,
        INVALID_DATA_DISCONNECT,
    };
    
public:
    XYNetworkErrorCode(eXYNetworkErrorCode code) : _code(code) {}
    eXYNetworkErrorCode GetCode() const { return _code; }
    
private:
    eXYNetworkErrorCode _code;
};
    
class XYLog
{
public:
	static void Log(std::string log)
	{
		std::cout << log << std::endl;
	}
};

}   //namespace XYNetwork

#endif /* XYCOMMONDEFINE_H_ */
