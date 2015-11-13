/*
 * XYExecutor.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYEXECUTOR_H_
#define XYEXECUTOR_H_

#include "XYCommonDefine.h"

namespace XYNetwork{

class XYExecutor
{
public:
	XYExecutor(char* data, long len, std::function<void(char*, long)> func);
	~XYExecutor();
	void Execute();

private:
	char* _data;
	long _len;
	std::function<void(char*, long)> _func;
};

}   //namespace XYNetwork



#endif /* XYEXECUTOR_H_ */
