#include "StdAfx.h"
#include "MainWorker.h"
#include "conio.h"
#include "PROCESS_Process.h"
#include "Console.h"



CMainWorker::CMainWorker()
{
	m_pRunInfo = NULL;
	m_pP = NULL;
}

CMainWorker::~CMainWorker(void)
{
}

void CMainWorker::Run(CRunInfo* pRunInfo)
{
	m_pP = new CProcess(
				pRunInfo->m_strUser,
				pRunInfo->m_strPassword,
				pRunInfo->m_strDomain);
	m_pP = new CProcess();
	m_pRunInfo = pRunInfo;

	CON().PrintLn("Starting...");
	m_bStopping = false;	
	m_pThread = ::AfxBeginThread((AFX_THREADPROC)CMainWorker::Thread, (LPVOID)this);
}

void CMainWorker::Stop()
{
	CON().PrintLn("Shutdown signal received!  Stopping...");
	//m_pP->SendControlC();

	CString strShutdownCommand = m_pRunInfo->m_strShutdownCommand;
	strShutdownCommand.Replace("#PID#", _F_("%d", this->m_pP->m_ProcessInfo.dwProcessId));

	// Temporal, hay que mandar la señal a todos los hijos.
	// De momento, si hay un solo hijo, funciona.
	CArray<DWORD, DWORD> childs;
	m_pP->GetChildsPID(m_pP->m_ProcessInfo.dwProcessId, &childs);
	for(int i=0; i<childs.GetSize(); i++)
	{
		strShutdownCommand.Replace("#CHILDPID#", _F_("%d", childs[i]));
	}


	if (m_pRunInfo->m_strShutdownCommand.GetLength() > 0)
	{
		CON().PrintLn(_F_("Stopping with shutdown command: %s", (LPCTSTR)strShutdownCommand));
		//CProcess P("oiuser", "omnilab2010", ".");
		CProcess P;
		// Usamos como redirector de output el m_worker
		if (!P.Open_Console(strShutdownCommand, this))
		{
			CON().PrintLn(_F_("ERROR: %s", (LPCTSTR)P.GetLastErrorMessage()));
			CON().PrintLn("Killing (1)...");
			m_pP->Kill();
		}
		else
		{
			P.WaitForExitCode();
		}
	}
	else
	{
		m_pP->SendControlC();
	}

	::Sleep(1000);	// Esperamos a que haga efecto el comando de Shutdown

	if (m_pP->IsRunning())
	{
		DWORD tickcount = ::GetTickCount();
		DWORD dw = ::WaitForSingleObject(m_pThread->m_hThread, 5000);
		switch (dw)
		{
		case 0x00000000L:
			CON().Print("WAIT_OBJECT_0"); break;
		case 0x00000080L:
			CON().Print("WAIT_ABANDONED"); break;
		case 0x00000102L:
			CON().Print("WAIT_TIMEOUT"); break;
		default:
			CON().PrintLn(_F_("WaitForSingleObject ERR: %d", dw));
		}

		CON().PrintLn(_F_(". Time: %d", ::GetTickCount() - tickcount));
	}

	if (m_pP->IsRunning())
	{
		CON().PrintLn("Killing (2)...");
		m_pP->Kill();
	}

	CON().PrintLn("Stopped");
}


UINT CMainWorker::Thread(LPVOID lParam)
{
	CON().PrintLn("Worker thread started.");
	LPVOID p = lParam;
	CMainWorker* _THIS = (CMainWorker*)p;

	_THIS->m_dwExitCode = 0;

	if (!_THIS->m_pP->Open_Console(_THIS->m_pRunInfo->m_strCmdLine, _THIS))
	{
		CON().PrintLn(_THIS->m_pP->GetLastErrorMessage());
		_THIS->m_dwExitCode = 0;
	}
	else
	{
		_THIS->m_dwExitCode = _THIS->m_pP->WaitForExitCode();
	}

	CON().PrintLn(_F_("Worker thread terminated. ExitCode = %d", _THIS->m_dwExitCode));
	return 0;
}


void CMainWorker::OnOutputText(LPCTSTR szText)
{
	CON().Print(szText);
}

void CMainWorker::OnProcessTerminated()
{

}

void CMainWorker::WaitToEnd()
{
	m_pP->WaitForExitCode();
}
