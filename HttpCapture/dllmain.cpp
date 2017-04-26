// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "HttpModuleFactory.h"

BOOL __stdcall DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


HTTP_MODULE_ID g_pModuleContext = NULL;
IHttpServer*  g_pHttpServer = NULL;

HRESULT __stdcall RegisterModule(DWORD dwServerVersion, IHttpModuleRegistrationInfo*  pModuleInfo, IHttpServer *pHttpServer)
{
	OutputDebugString(_T("Enter RegisterModule"));

	// step 1: save the IHttpServer and the module context id for future use 
	g_pModuleContext = pModuleInfo->GetId();
	g_pHttpServer = pHttpServer;

	// step 2: create the module factory 
	CHttpModuleFactory* pFactory = new CHttpModuleFactory();

	// step 3: register for server events 
	return  pModuleInfo->SetRequestNotifications(pFactory,
		RQ_PRE_EXECUTE_REQUEST_HANDLER | RQ_END_REQUEST | RQ_READ_ENTITY | RQ_SEND_RESPONSE/**/,
		0);

	/*| RQ_EXECUTE_REQUEST_HANDLER*/
}


