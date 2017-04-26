#include "HttpModuleFactory.h"
#include "HttpModuleCapturer.h"


CHttpModuleFactory::CHttpModuleFactory()
{
}


CHttpModuleFactory::~CHttpModuleFactory()
{
}

HRESULT CHttpModuleFactory::GetHttpModule(_Outptr_ CHttpModule ** ppModule, _In_ IModuleAllocator * pAllocator)
{
	CHttpModuleCapturer* pHttpModule = new CHttpModuleCapturer();
	*ppModule = pHttpModule;
	return S_OK;
}

void CHttpModuleFactory::Terminate()
{
	OutputDebugString(_T("CHttpModuleFactory::Terminate"));
}
