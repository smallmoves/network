/*
 * XYThreadManager.h
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#ifndef XYTHREADMANAGER_H_
#define XYTHREADMANAGER_H_

#include "XYCommonDefine.h"
#include "XYExecutor.h"

namespace XYNetwork{

class Singleton
{
protected:
	Singleton(){}
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
};

class XYThreadManager : public Singleton
{
public:
	static XYThreadManager* ShareIntance();
	~XYThreadManager();

	void Start();
	void Stop();

	void EnqueueInDataCall(XYExecutor* executor);
	void EnqueueOutData(XYExecutor* executor);

	static void* InThread(void* arg);
	static void* OutThread(void* arg);

	void ProcessOutItem();
	void ProcessItem();

private:
	XYThreadManager();

private:
	pthread_mutex_t _inMutex;
	std::list<XYExecutor*> _inThreadQueue;
	pthread_t _inPid;
	bool _inHasQueuedItems;

	pthread_mutex_t _outMutex;
	std::list<XYExecutor*> _outThreadQueue;
	pthread_t _outPid;
	bool _outHasQueuedItems;

	bool _running;
};

}   //namespace XYNetwork


#endif /* XYTHREADMANAGER_H_ */
