#include "StdAfx.h"
#include "SputnikWorker.h"
#include "conio.h"
#include "Console.h"

CSputnikWorker::CSputnikWorker(void)
{
}

CSputnikWorker::~CSputnikWorker(void)
{
}

void CSputnikWorker::Run()
{
	m_bStopping = false;	
	CWinThread* th = ::AfxBeginThread((AFX_THREADPROC)CSputnikWorker::Thread, (LPVOID)this);
	char sz[1024];
	getch();
	CON().PrintLn("Closing...");
	m_bStopping = true;
	::WaitForSingleObject(th->m_hThread, INFINITE);
	CON().PrintLn("Bye");
}




UINT CSputnikWorker::Thread(LPVOID lParam)
{
	LPVOID p = lParam;
	CSputnikWorker* _THIS = (CSputnikWorker*)p;
	long lCounter = 0;
	while(!_THIS->m_bStopping)
	{
		lCounter++;
		CString str;
		str.Format("#%d. Good morning. Current time: %s", 
			lCounter,
			(LPCTSTR)COleDateTime::GetCurrentTime().Format());
		CON().PrintLn(str);
		::Beep(200, 100);
		::Sleep(200);
		::Beep(400, 100);
		::Sleep(2000);
	}

	return 0;
}
