/*
 * XYExecutor.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYExecutor.h"

namespace XYNetwork{

XYExecutor::XYExecutor(char* data, long len, std::function<void(char*, long)> func)
//: _data(data)
: _len(len)
, _func(func)
{
    if(_len > 0)
    {
        _data = new char[len];
        bzero(_data, len);
        memcpy(_data, data, len);
    }
}

XYExecutor::~XYExecutor()
{
	if(_len > 0)
	{
		delete [] _data;
	}
}

void XYExecutor::Execute()
{
	_func(_data, _len);
}

}   //namespace XYNetwork

