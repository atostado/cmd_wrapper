#pragma once
#include "RunInfo.h"
#include "PROCESS_IOutputRedirector.h"
#include "PROCESS_Process.h"

class CMainWorker : public IOutputRedirector
{
public:
	CMainWorker();
	~CMainWorker(void);

protected:
	CWinThread* m_pThread;
public:	// Accesible desde el Thread
	CRunInfo* m_pRunInfo;
	DWORD m_dwExitCode;
	CProcess* m_pP;

public:
	void Run(CRunInfo* pRunInfo);
	void Stop();
	void WaitToEnd();
	DWORD GetExitCode();
    static UINT Thread(LPVOID lParam);

	bool m_bStopping;

	virtual void OnOutputText(LPCTSTR szText);
	virtual void OnProcessTerminated();
};
