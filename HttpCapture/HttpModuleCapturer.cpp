#include "stdafx.h"
#include "HttpModuleCapturer.h"
#include <HttpFilt.h>
#pragma comment(lib, "advapi32.lib")

char* g_szHeaderName[] = {
	"HttpHeaderCacheControl",
	"HttpHeaderConnection",
	"HttpHeaderDate",
	"HttpHeaderKeepAlive",
	"HttpHeaderPragma",
	"HttpHeaderTrailer",    // general-header [section 4.5]
	"HttpHeaderTransferEncoding",    // general-header [section 4.5]
	"HttpHeaderUpgrade",    // general-header [section 4.5]
	"HttpHeaderVia",    // general-header [section 4.5]
	"HttpHeaderWarning",    // general-header [section 4.5]

	"HttpHeaderAllow",   // entity-header  [section 7.1]
	"HttpHeaderContentLength",   // entity-header  [section 7.1]
	"HttpHeaderContentType",   // entity-header  [section 7.1]
	"HttpHeaderContentEncoding",   // entity-header  [section 7.1]
	"HttpHeaderContentLanguage",   // entity-header  [section 7.1]
	"HttpHeaderContentLocation",   // entity-header  [section 7.1]
	"HttpHeaderContentMd5",   // entity-header  [section 7.1]
	"HttpHeaderContentRange",   // entity-header  [section 7.1]
	"HttpHeaderExpires",   // entity-header  [section 7.1]
	"HttpHeaderLastModified",   // entity-header  [section 7.1]


	// Request Headers

	"HttpHeaderAccept",   // request-header [section 5.3]
	"HttpHeaderAcceptCharset",   // request-header [section 5.3]
	"HttpHeaderAcceptEncoding",   // request-header [section 5.3]
	"HttpHeaderAcceptLanguage",   // request-header [section 5.3]
	"HttpHeaderAuthorization",   // request-header [section 5.3]
	"HttpHeaderCookie",   // request-header [not in rfc]
	"HttpHeaderExpect",   // request-header [section 5.3]
	"HttpHeaderFrom",   // request-header [section 5.3]
	"HttpHeaderHost",   // request-header [section 5.3]
	"HttpHeaderIfMatch",   // request-header [section 5.3]

	"HttpHeaderIfModifiedSince",   // request-header [section 5.3]
	"HttpHeaderIfNoneMatch",   // request-header [section 5.3]
	"HttpHeaderIfRange",   // request-header [section 5.3]
	"HttpHeaderIfUnmodifiedSince",   // request-header [section 5.3]
	"HttpHeaderMaxForwards",   // request-header [section 5.3]
	"HttpHeaderProxyAuthorization",   // request-header [section 5.3]
	"HttpHeaderReferer",   // request-header [section 5.3]
	"HttpHeaderRange",   // request-header [section 5.3]
	"HttpHeaderTe",   // request-header [section 5.3]
	"HttpHeaderTranslate",   // request-header [webDAV, not in rfc 2518]

	"HttpHeaderUserAgent",   // request-header [section 5.3]

	"HttpHeaderRequestMaximum",


	// Response Headers

	"HttpHeaderAcceptRanges",   // response-header [section 6.2]
	"HttpHeaderAge",   // response-header [section 6.2]
	"HttpHeaderEtag",   // response-header [section 6.2]
	"HttpHeaderLocation",   // response-header [section 6.2]
	"HttpHeaderProxyAuthenticate",   // response-header [section 6.2]
	"HttpHeaderRetryAfter",   // response-header [section 6.2]
	"HttpHeaderServer",   // response-header [section 6.2]
	"HttpHeaderSetCookie",   // response-header [not in rfc]
	"HttpHeaderVary",   // response-header [section 6.2]
	"HttpHeaderWwwAuthenticate",   // response-header [section 6.2]

	"HttpHeaderResponseMaximum",


	"HttpHeaderMaximum"
};

CHttpModuleCapturer::CHttpModuleCapturer()
{
	m_bResponseHeadersLog = false;
	m_pFileHttpStream = NULL;
	m_pFileResponseStream = NULL;
	//
	InitHttpLogFile();
}


CHttpModuleCapturer::~CHttpModuleCapturer()
{

}

REQUEST_NOTIFICATION_STATUS CHttpModuleCapturer::OnPreExecuteRequestHandler(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider)
{
	//
	PCSTR pszRawUrl = pHttpContext->GetRequest()->GetRawHttpRequest()->pRawUrl;
	pHttpContext->GetRequest()->GetHttpMethod();
	std::string strInfo = "on CMyHttpModule::OnPreExecuteRequestHandler, url=";
	strInfo += pszRawUrl;
	OutputDebugStringA(strInfo.c_str());

	//output http header

	//method
	strInfo = pHttpContext->GetRequest()->GetHttpMethod();
	strInfo += " ";
	strInfo += pszRawUrl;
	strInfo += "\r\n";
	WriteDataToHttpLogFile((void*)strInfo.c_str(), strInfo.length(), m_pFileHttpStream);


	//know headers
	HTTP_KNOWN_HEADER* pKnowHeaders = pHttpContext->GetRequest()->GetRawHttpRequest()->Headers.KnownHeaders;
	for (HTTP_HEADER_ID headerID = HttpHeaderCacheControl; headerID < HttpHeaderRequestMaximum; headerID = (HTTP_HEADER_ID)(headerID + 1))
	{

		if (pKnowHeaders[headerID].pRawValue)
		{
			const char* pszHeaderValue = pKnowHeaders[headerID].pRawValue;

			std::string strHeader = g_szHeaderName[headerID] + 10;
			strHeader += ":";
			strHeader += pszHeaderValue;
			strHeader += "\r\n";

			WriteDataToHttpLogFile((void*)strHeader.c_str(), strHeader.length(), m_pFileHttpStream);
		}
	}

	//unknow headers
	HTTP_UNKNOWN_HEADER* pUnknowHeaders = pHttpContext->GetRequest()->GetRawHttpRequest()->Headers.pUnknownHeaders;
	for (int i = 0; i < pHttpContext->GetRequest()->GetRawHttpRequest()->Headers.UnknownHeaderCount; i++)
	{
		std::string strHeader = pUnknowHeaders[i].pName;
		strHeader += ":";
		strHeader += pUnknowHeaders[i].pRawValue;
		strHeader += "\r\n";

		WriteDataToHttpLogFile((void*)strHeader.c_str(), strHeader.length(), m_pFileHttpStream);
	}

	return RQ_NOTIFICATION_CONTINUE;
}


/*

REQUEST_NOTIFICATION_STATUS CHttpModuleCapturer::OnPostExecuteRequestHandler(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider)
{
OutputDebugStringA("on CMyHttpModule::OnPostExecuteRequestHandler");


if (m_pFile)
{
const char* pszResponseSep = "\r\n======================================================================================\r\n";
fwrite(pszResponseSep, 1, strlen(pszResponseSep), m_pFile);
fflush(m_pFile);
}

IHttpResponse* pHttpResponse = pHttpContext->GetResponse();
if (pHttpResponse)
{
//get content type
USHORT nContentLen = 0;
PCSTR szContentType = pHttpResponse->GetHeader(HttpHeaderContentType, &nContentLen);
if (nContentLen > 0)
{
OutputDebugStringA(szContentType);
//	bool bRightContentType = strstr(szContentType, "text") || strstr(szContentType, "html") || strstr(szContentType, "xml");
//bRightContentType = true;
//	if (bRightContentType)
{
HTTP_RESPONSE* pHttpRawResponse = pHttpResponse->GetRawHttpResponse();
if (pHttpRawResponse)
{
for (int i = 0; i < pHttpRawResponse->EntityChunkCount; i++)
{
HTTP_DATA_CHUNK* pChunk = &(pHttpRawResponse->pEntityChunks[i]);
if (pChunk && pChunk->DataChunkType==HttpDataChunkFromMemory)
{
if (m_pFile)
{
fwrite(pChunk->FromMemory.pBuffer, 1, pChunk->FromMemory.BufferLength, m_pFile);
fflush(m_pFile);
}
}

}
}

}
}


}


return RQ_NOTIFICATION_CONTINUE;
}

*/


REQUEST_NOTIFICATION_STATUS CHttpModuleCapturer::OnSendResponse(_In_ IHttpContext * pHttpContext, _In_ ISendResponseProvider * pProvider)
{

	TString strInfo = _T("on CMyHttpModule::OnSendResponse\n");
	OutputDebugString(strInfo.c_str());


	//out put headers
	if (!m_bResponseHeadersLog)
	{
		m_bResponseHeadersLog = true;

		//http status	 
		USHORT nStatusCode = pHttpContext->GetResponse()->GetRawHttpResponse()->StatusCode;
		char szCode[200];
		sprintf_s(szCode, "\r\n%d\r\n", nStatusCode);
		WriteDataToHttpLogFile((void*)szCode, strlen(szCode), m_pFileHttpStream);

		//know headers
		HTTP_KNOWN_HEADER* pKnowHeaders = pHttpContext->GetResponse()->GetRawHttpResponse()->Headers.KnownHeaders;
		for (HTTP_HEADER_ID headerID = HttpHeaderCacheControl; headerID < HttpHeaderResponseMaximum; headerID = (HTTP_HEADER_ID)(headerID + 1))
		{
			if (pKnowHeaders[headerID].pRawValue)
			{
				std::string strHeader = g_szHeaderName[headerID] + 10;
				strHeader += ":";
				strHeader += pKnowHeaders[headerID].pRawValue;
				strHeader += "\r\n";
				WriteDataToHttpLogFile((void*)strHeader.c_str(), strHeader.length(), m_pFileHttpStream);
			}
		}

		//unknown headers
		HTTP_UNKNOWN_HEADER* pUnknowHeaders = pHttpContext->GetResponse()->GetRawHttpResponse()->Headers.pUnknownHeaders;
		for (int i = 0; i < pHttpContext->GetRequest()->GetRawHttpRequest()->Headers.UnknownHeaderCount; i++)
		{
			std::string strHeader = pUnknowHeaders[i].pName;
			strHeader += ":";
			strHeader += pUnknowHeaders[i].pRawValue;
			strHeader += "\r\n";
			WriteDataToHttpLogFile((void*)strHeader.c_str(), strHeader.length(), m_pFileHttpStream);
		}

	}


	//write response fileName
	if (NULL == m_pFileResponseStream)
	{
		std::string strFileResponse = "C:\\test\\" + m_strLogFileKey + "_HttpResponse.txt";
		errno_t nError = fopen_s(&m_pFileResponseStream, strFileResponse.c_str(), "wb");

		//write tip info
		std::string strTip = "\r\nResponse Body File:" + strFileResponse + "\r\n";
		WriteDataToHttpLogFile((void*)strTip.c_str(), strTip.length(), m_pFileHttpStream);
	}

	int nResponseLen = 0;
	IHttpResponse * pHttpResponse = pHttpContext->GetResponse();
	if (pHttpResponse)
	{
		HTTP_RESPONSE *pResponseStruct = pHttpResponse->GetRawHttpResponse();
		if (pResponseStruct)
		{
			for (int i = 0; i < pResponseStruct->EntityChunkCount; i++)
			{
				HTTP_DATA_CHUNK* pChunk = &(pResponseStruct->pEntityChunks[i]);
				if (pChunk->DataChunkType == HttpDataChunkFromMemory)
				{
					WriteDataToHttpLogFile(pChunk->FromMemory.pBuffer, pChunk->FromMemory.BufferLength, m_pFileResponseStream);
					nResponseLen += pChunk->FromMemory.BufferLength;
				}
			}
		}
	}

	//write tip info
	char szTip[200];
	sprintf_s(szTip, "Write Response Data, Len=%d\r\n", nResponseLen);
	WriteDataToHttpLogFile((void*)szTip, strlen(szTip), m_pFileHttpStream);

	return RQ_NOTIFICATION_CONTINUE;
}


REQUEST_NOTIFICATION_STATUS CHttpModuleCapturer::OnReadEntity(_In_ IHttpContext * pHttpContext, _In_ IReadEntityProvider * pProvider)
{
	OutputDebugStringW(L"Enter CMyHttpModule::OnReadEntity");

	PVOID pBuffer = NULL;
	DWORD dwDataLen = 0;
	DWORD dwBufferLen = 0;
	pProvider->GetEntity(&pBuffer, &dwDataLen, &dwBufferLen);

	//copy memory and set
	unsigned char* pNewBuffer = new unsigned char[dwDataLen];
	memcpy_s(pNewBuffer, dwDataLen, pBuffer, dwDataLen);
	pProvider->SetEntity(pNewBuffer, dwDataLen, dwDataLen);


	wchar_t szLog[200];
	wsprintfW(szLog, L"Data Len:%u, Buffer Len:%u", dwDataLen, dwBufferLen);
	OutputDebugStringW(szLog);

	//
	std::string strRequestBodyFile = "c:\\test\\" + m_strLogFileKey + "_HttpRequestBody.txt";

	//output tip info;
	std::string wstrTipInfo = "\r\nRequest Body File:";
	wstrTipInfo += strRequestBodyFile + "\r\n";
	WriteDataToHttpLogFile((void*)wstrTipInfo.c_str(), wstrTipInfo.length(), m_pFileHttpStream);

	//output request body
	FILE* fRequest = NULL;
	fopen_s(&fRequest, strRequestBodyFile.c_str(), "wb");
	WriteDataToHttpLogFile(pBuffer, dwDataLen, fRequest);
	fclose(fRequest);


	return RQ_NOTIFICATION_CONTINUE;

}

REQUEST_NOTIFICATION_STATUS CHttpModuleCapturer::OnEndRequest(_In_ IHttpContext * pHttpContext, _In_ IHttpEventProvider * pProvider)
{
	OutputDebugStringW(L"Enter CMyHttpModule::OnEndRequest");

	return RQ_NOTIFICATION_CONTINUE;

}

void CHttpModuleCapturer::InitHttpLogFile()
{
	m_strLogFileKey = CreateGuid();
	std::string strFileHttpStream = "C:\\test\\" + m_strLogFileKey + "_HttpStream.txt";

	fopen_s(&m_pFileHttpStream, strFileHttpStream.c_str(), "wb");
}

void CHttpModuleCapturer::UnInitHttpLogFile()
{
	if (m_pFileHttpStream != NULL)
	{
		fflush(m_pFileHttpStream);
		fclose(m_pFileHttpStream);
		m_pFileHttpStream = NULL;
	}

	if (m_pFileResponseStream != NULL)
	{
		fflush(m_pFileResponseStream);
		fclose(m_pFileResponseStream);
		m_pFileResponseStream = NULL;
	}
}


std::string CHttpModuleCapturer::CreateGuid()
{
#if 0
	std::string strGuid;
	GUID guid;

	CoCreateGuid(&guid);

	// Convert the GUID to a string
	char* pwszGuid = NULL;
	UuidToStringA(&guid, (RPC_CSTR*)&pwszGuid);
	if (pwszGuid)
	{
		strGuid = pwszGuid;
	}
	RpcStringFreeA((RPC_CSTR*)pwszGuid);

	return strGuid;
#else
	DWORD dwThreadID = GetCurrentThreadId();
	DWORD dwTick = GetTickCount();
	char keyBuf[200];
	sprintf_s(keyBuf, "%d-%d", dwTick, dwThreadID);
	std::string strKey = keyBuf;
	return strKey;
#endif
}

void CHttpModuleCapturer::WriteDataToHttpLogFile(void* pData, int nLen, FILE* f)
{
	if (f)
	{
		fwrite(pData, 1, nLen, f);
		fflush(f);
	}
	else
	{
		OutputDebugStringA("WriteDataToHttpLogFile file=NULL\n");
	}

	//OutputDebugStringA((char*)pData);
}

void CHttpModuleCapturer::Dispose()
{
	OutputDebugStringA("On CMyHttpModule::Dispose\n");
	m_bResponseHeadersLog = false;
	UnInitHttpLogFile();
}