#pragma once
#include "stdafx.h"

class CHttpModuleFactory : public IHttpModuleFactory
{
public:
	CHttpModuleFactory();
	~CHttpModuleFactory();

protected:
	 HRESULT GetHttpModule(_Outptr_ CHttpModule ** ppModule, _In_ IModuleAllocator * pAllocator);
	 void Terminate();

};

