/*
 * XYThreadManager.cpp
 *
 *  Created on: 2015?3?10?
 *      Author: xiao
 */

#include "XYThreadManager.h"

namespace XYNetwork{

//重连的时候要考虑线程里的历史数据的清理问题

XYThreadManager* XYThreadManager::ShareIntance()
{
	static XYThreadManager* s = nullptr;
	if(s == nullptr)
	{
		s = new XYThreadManager;
	}
	return s;
}

XYThreadManager::XYThreadManager()
: _inPid(0)
, _outPid(0)
, _inHasQueuedItems(false)
, _outHasQueuedItems(false)
, _running(true)
{
	pthread_mutex_init(&_inMutex,NULL);
	pthread_mutex_init(&_outMutex,NULL);

	_inThreadQueue.clear();
	_outThreadQueue.clear();

	pthread_attr_t inattr;
	pthread_attr_init(&inattr);
	pthread_attr_setdetachstate(&inattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_inPid, &inattr, (void*(*)(void*))&XYThreadManager::InThread, this);

	pthread_attr_t outattr;
	pthread_attr_init(&outattr);
	pthread_attr_setdetachstate(&outattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_outPid, &outattr, (void*(*)(void*))&XYThreadManager::OutThread, this);
}

XYThreadManager::~XYThreadManager()
{
	_running = false;
	pthread_mutex_destroy(&_inMutex);
	pthread_mutex_destroy(&_outMutex);
//	pthread_join(_inPid,NULL);
//	pthread_join(_outPid,NULL);

	std::list<XYExecutor*>::iterator iter = _inThreadQueue.begin();
	for( ;iter != _inThreadQueue.end(); iter++)
	{
		delete *iter;
		*iter = nullptr;
	}
	_inThreadQueue.clear();

	iter = _outThreadQueue.begin();
	for( ;iter != _outThreadQueue.end(); iter++)
	{
		delete *iter;
		*iter = nullptr;
	}
	_outThreadQueue.clear();
}

void XYThreadManager::Start()
{
	if(_running)
	{
		return;
	}
	_running = true;

	pthread_attr_t inattr;
	pthread_attr_init(&inattr);
	pthread_attr_setdetachstate(&inattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_inPid, &inattr, (void*(*)(void*))&XYThreadManager::InThread, this);

	pthread_attr_t outattr;
	pthread_attr_init(&outattr);
	pthread_attr_setdetachstate(&outattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_outPid, &outattr, (void*(*)(void*))&XYThreadManager::OutThread, this);
}

void XYThreadManager::Stop()
{
	if(!_running)
	{
		return;
	}
	_running = false;
	usleep(10000);

	std::list<XYExecutor*>::iterator iter = _inThreadQueue.begin();
	for( ;iter != _inThreadQueue.end(); iter++)
	{
		delete *iter;
		*iter = nullptr;
	}
	_inThreadQueue.clear();

	iter = _outThreadQueue.begin();
	for( ;iter != _outThreadQueue.end(); iter++)
	{
		delete *iter;
		*iter = nullptr;
	}
	_outThreadQueue.clear();
}

void XYThreadManager::EnqueueInDataCall(XYExecutor* executor)
{
	pthread_mutex_lock(&_inMutex);
	_inThreadQueue.push_back(executor);
	_inHasQueuedItems = true;
	pthread_mutex_unlock(&_inMutex);
}

void XYThreadManager::EnqueueOutData(XYExecutor* executor)
{
	pthread_mutex_lock(&_outMutex);
	_outThreadQueue.push_back(executor);
	_outHasQueuedItems = true;
	pthread_mutex_unlock(&_outMutex);
}

void* XYThreadManager::InThread(void* arg)
{
	XYThreadManager* p = static_cast<XYThreadManager*>(arg);
	p->ProcessItem();
	return nullptr;
}

void* XYThreadManager::OutThread(void* arg)
{
	XYThreadManager* p = static_cast<XYThreadManager*>(arg);
	p->ProcessOutItem();
	return nullptr;
}

void XYThreadManager::ProcessOutItem()
{
	while(_running)
	{
		usleep(5000);

		if (_running == false) break;

		if (!_outHasQueuedItems) continue;

		XYExecutor* item;

		pthread_mutex_lock(&_outMutex);

		while (!_outThreadQueue.empty())
		{
			item = _outThreadQueue.front();
			_outThreadQueue.pop_front();
			item->Execute();

			delete item;
		}

		_outHasQueuedItems = false;

		pthread_mutex_unlock(&_outMutex);
	}
	return;
}

void XYThreadManager::ProcessItem()
{
	while(_running)
	{
		usleep(5000);

		if (_running == false) break;

		if (!_inHasQueuedItems) continue;

		XYExecutor* item;

		pthread_mutex_lock(&_inMutex);

		while (!_inThreadQueue.empty())
		{
			item = _inThreadQueue.front();
			_inThreadQueue.pop_front();
			item->Execute();

			delete item;
		}

		_inHasQueuedItems = false;

		pthread_mutex_unlock(&_inMutex);
	}
	return;
}


}   //namespace XYNetwork


