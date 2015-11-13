/*
 * XYDataHandler.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYDataHandler.h"
#include "XYExecutor.h"
#include "XYThreadManager.h"

namespace XYNetwork{

XYDataHandler::XYDataHandler()
: _receiveData("")
, _packetHeaderStream("")
, _packetDataLen(0)
{
	
}

void XYDataHandler::ClearData()
{
	_receiveData = "";
	_packetDataLen = 0;
	_packetHeaderStream.clear();
}

void XYDataHandler::HandleReadData(char* data, long len)
{
	std::function<void(char*, long)> func = std::bind(&XYDataHandler::_HandleReadData, this, std::placeholders::_1, std::placeholders::_2);
	XYExecutor* pXYExecutor = new XYExecutor(data, len, func);
	XYThreadManager::ShareIntance()->EnqueueInDataCall(pXYExecutor);
}

void XYDataHandler::HandleWriteData(const XYPacketHeader* header, const char* data, int32_t len)
{
	XYPacketHeader packetHeader = *header;
	packetHeader.len = (uint32_t)(sizeof(XYPacketHeader) + len);

	char* pOut = new char[packetHeader.len];
	char* pTmp = pOut;
	memcpy(pTmp, &packetHeader, sizeof(XYPacketHeader));
	pTmp += sizeof(XYPacketHeader);
	memcpy(pTmp, data, len);

	onSend(pOut, packetHeader.len);

	delete [] pOut;
}

void XYDataHandler::_HandleReadData(char* data, long len)
{
	if(len <= 0 || data == nullptr)
	{
		return;
	}

	//根据包头拆分或组装数据
	if(_packetDataLen == 0)    //new packet
	{
		long lTmpLen = 0;
		char* pTmp = data;
		while(lTmpLen < len && sizeof(XYPacketHeader) > _packetHeaderStream.size())
		{
			_packetHeaderStream.push_back(*pTmp);
            _receiveData.push_back(*pTmp);
			pTmp++;
			lTmpLen++;
		}

		if(sizeof(XYPacketHeader) == _packetHeaderStream.size())    //receive packet header over
		{
            const char* header = _packetHeaderStream.c_str();
            const XYPacketHeader* pHeader = (const XYPacketHeader*)header;
            if(pHeader->len > PACKET_MAX_BYTE)   //error data
            {
                _receiveData = "";
                _packetDataLen = 0;
                _packetHeaderStream.clear();
                
                onInvalidData(XYNetworkErrorCode(XYNetworkErrorCode::eXYNetworkErrorCode::INVALID_DATA_DISCONNECT));
                return;
            }
            
            _packetHeader = *pHeader;
			_packetDataLen = _packetHeader.len - sizeof(XYPacketHeader);
            if(_packetDataLen == 0)   //packet header only
            {
                onHandlerReceiveData(_receiveData);
                _receiveData = "";
            }
			_packetHeaderStream = "";

			_HandleReadData(pTmp, len - lTmpLen);
			return;
		}
		else
		{
			return;
		}
	}
	else	//packet data
	{
		char* pTmp = data;
		if(len >= _packetDataLen)
		{
            _receiveData.append(data, _packetDataLen);
            
			//dispatch data to main thread
            onHandlerReceiveData(_receiveData);

			pTmp += _packetDataLen;
			long lRemainDataLen = len - _packetDataLen;

			_receiveData = "";
			_packetDataLen = 0;
			_packetHeaderStream.clear();
			_HandleReadData(pTmp, lRemainDataLen);
			return;
		}
		else
		{
            _receiveData.append(data, len);
			_packetDataLen -= len;
			return;
		}
	}
}



}   //namespace XYNetwork


