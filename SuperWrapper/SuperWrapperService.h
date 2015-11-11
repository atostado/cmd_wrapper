#pragma once
#include "ServiceBase.h"
#include "MainWorker.h"

class CSuperWrapperService : public CServiceBase
{
public:
	CSuperWrapperService(LPCTSTR szServiceName);
	~CSuperWrapperService(void);

protected:
	CRunInfo* m_pRunInfo;

public:
	CMainWorker m_worker;

public:
	void SetRunInfo(CRunInfo* pRunInfo);

protected:
    virtual void OnStart(DWORD dwArgc, PSTR *pszArgv);
	virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
};
