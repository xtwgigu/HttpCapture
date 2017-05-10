#pragma once
#include "stdafx.h"

class CHttpModuleCapturer : public CHttpModule
{
public:
	CHttpModuleCapturer();
	~CHttpModuleCapturer();

protected:
	REQUEST_NOTIFICATION_STATUS OnPreExecuteRequestHandler(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider);
	REQUEST_NOTIFICATION_STATUS OnSendResponse(_In_ IHttpContext * pHttpContext, _In_ ISendResponseProvider * pProvider);
	//	REQUEST_NOTIFICATION_STATUS OnPostExecuteRequestHandler(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider);
	//	REQUEST_NOTIFICATION_STATUS OnExecuteRequestHandler(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider);
	REQUEST_NOTIFICATION_STATUS OnReadEntity(_In_ IHttpContext * pHttpContext, _In_ IReadEntityProvider * pProvider);
	REQUEST_NOTIFICATION_STATUS OnEndRequest(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider *  pProvider);

	void Dispose();


protected:
	void InitHttpLogFile();
	void UnInitHttpLogFile();
	void WriteDataToHttpLogFile(void* pData, int nLen, FILE* f);
	std::string CreateGuid();
protected:
	std::string m_strLogFileKey;
	FILE * m_pFileHttpStream;
	FILE * m_pFileResponseStream;
	FILE * m_pFileRequestStream;
	bool m_bResponseHeadersLog;
	char m_szResponseFileName[MAX_PATH];

};

