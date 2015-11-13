//============================================================================
// Name        : XYNetwork.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include "XYThreadManager.h"
#include "XYTCPSocketLayer.h"
#include "XYBitSwarm.h"

using namespace std;
using namespace XYNetwork;

class testThread
{
public:
	void Intest(char* data, long len)
	{
		std::string str = data;
		std::cout << "in test data " << str << std::endl;
	}
	void Outtest(char* data, long len)
	{
		std::string str = data;
		std::cout << "out test data " << str << std::endl;
	}

	void Start(std::string s)
	{
		char* pData = new char[s.size() + 1];
		strcpy(pData, s.c_str());
//		memcpy(pData, s.c_str(), s.size());
//		pData[s.size()] = '\0';

		std::function<void(char*, long)> func = std::bind(&testThread::Intest, this, std::placeholders::_1, std::placeholders::_2);
		XYExecutor* pXYExecutor = new XYExecutor(pData, s.size() + 1, func);
		XYThreadManager::ShareIntance()->EnqueueInDataCall(pXYExecutor);
	}
};

int main()
{

//	XYThreadManager::ShareIntance();
//	testThread test;
//
//	for(int i = 0; i < 100; i++)
//	{
//		ostringstream ss;
//		ss << i;
//		string s1 = ss.str();
//		test.Start(s1);
//	}
//	sleep(25);

//	XYBitSwarm* p = new XYBitSwarm;
//	p->Connect("127.0.0.1", 53321);

//	sleep(2);
//	while(1)
//	{
//		sleep(1);
//		p->SendMsg("helloworld");
//	}

//	XYTCPSocketLayer* socketLayer = new XYTCPSocketLayer;
//	socketLayer->Connect("127.0.0.1", 53321);

//	sleep(50);
//	while(1)
//	{
//		sleep(50);
//	}
	return 0;
}
















