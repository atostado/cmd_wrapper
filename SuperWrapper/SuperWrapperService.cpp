#include "StdAfx.h"
#include "SuperWrapperService.h"
#include "Console.h"

CSuperWrapperService::CSuperWrapperService(LPCTSTR szServiceName) :
	CServiceBase(szServiceName)
{
}

CSuperWrapperService::~CSuperWrapperService(void)
{
}

UINT CSuperWrapperService_Thread(LPVOID lParam)
{
	CSuperWrapperService* _pTHIS = (CSuperWrapperService*)lParam; 
	::Sleep(500);
	_pTHIS->m_worker.WaitToEnd();
	
	if (_pTHIS->m_status.dwCurrentState == SERVICE_RUNNING)
	{
		CON().PrintLn("Wrapped process finished. Shutting down service...");
		_pTHIS->Stop();
	}

	return 0;
}


void CSuperWrapperService::SetRunInfo(CRunInfo* pRunInfo)
{
	m_pRunInfo = pRunInfo;
	m_worker.Run(m_pRunInfo);

	::AfxBeginThread((AFX_THREADPROC)CSuperWrapperService_Thread, (LPVOID)this);
}

void CSuperWrapperService::OnStart(DWORD dwArgc, PSTR *pszArgv)
{
}

void CSuperWrapperService::OnStop()
{
	m_worker.Stop();
}

void CSuperWrapperService::OnPause()
{
}

void CSuperWrapperService::OnContinue()
{
}

void CSuperWrapperService::OnShutdown()
{
	//m_worker.Stop();
}


