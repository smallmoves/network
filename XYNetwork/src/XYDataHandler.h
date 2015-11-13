/*
 * XYDataHandler.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYDATAHANDLER_H_
#define XYDATAHANDLER_H_

#include "XYCommonDefine.h"

namespace XYNetwork{

class XYDataHandler
{
public:
	XYDataHandler();
	void ClearData();

	void HandleReadData(char* data, long len);
	void HandleWriteData(const XYPacketHeader* header, const char* data, int32_t len);

private:
	void _HandleReadData(char* data, long len);

public:
	std::function<void(char*, long)> onSend;
    std::function<void(const std::string&)> onHandlerReceiveData;
    std::function<void(XYNetworkErrorCode)> onInvalidData;

private:
    XYPacketHeader _packetHeader;
    
	std::string _receiveData;
    std::string _packetHeaderStream;
	long _packetDataLen;

};

}   //namespace XYNetwork


#endif /* XYDATAHANDLER_H_ */
