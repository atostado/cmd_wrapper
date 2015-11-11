/////////////////////////////////////////////////////////////////////////////
// --- ntservice.cpp ---
//
// J.M.Wehlou, 2001
//
// Description:
//
//	Simple NT service skeleton. Inspired by "netblock" by Ton Plooy,
//	WDJ october 2000.
//
//	Command line params:	-i	install
//							-u	uninstall
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include <stdlib.h>
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include "Winsvc.h"
// --------------------------------------------------------------------------
// globals 
//
LPSTR					g_szServiceName	= "NTSrvYourName";
LPSTR					g_szRegBase		= "SOFTWARE\\Cactus\\NTSrvYourName";
SERVICE_STATUS			g_hSvcStat		= {0};
SERVICE_STATUS_HANDLE	g_hSvc			= NULL;


// --------------------------------------------------------------------------
// Prototypes 
//

void		Install(LPCSTR szServiceName);
void		Uninstall(LPCSTR szServiceName);
VOID WINAPI	Handler(DWORD fdwControl);
VOID WINAPI	ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);

// --------------------------------------------------------------------------
// Service Handle wrapper with exception safe close semantics 
//

class SvcHandle
{
	SC_HANDLE	m_h;
public:
	explicit SvcHandle(SC_HANDLE h) : m_h(h) {};
	~SvcHandle()			{ if (NULL != m_h) CloseServiceHandle(m_h); }
	operator SC_HANDLE ()	{ return m_h; }
	bool isnull()			{ return NULL == m_h; }
};


// --------------------------------------------------------------------------
// WinMain 
//

int Start()
{
	SERVICE_TABLE_ENTRY svcEntry[2];
	svcEntry[0].lpServiceName	= g_szServiceName;
	svcEntry[0].lpServiceProc	= ServiceMain;
	svcEntry[1].lpServiceName	= NULL;
	svcEntry[1].lpServiceProc	= NULL;
	StartServiceCtrlDispatcher(svcEntry);
	return 0;
}



int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR psArgs, int)
throw()
{
	try
	{

		SERVICE_TABLE_ENTRY svcEntry[2];
		
		if (psArgs != NULL)
		{
			if (strncmp(psArgs, "-i", 2) == 0)
			{
				Install(g_szServiceName);
			}
			else if (strncmp(psArgs, "-u", 2) == 0)
			{
				Uninstall(g_szServiceName);
			}
		}
		else
		{
			svcEntry[0].lpServiceName	= g_szServiceName;
			svcEntry[0].lpServiceProc	= ServiceMain;
			svcEntry[1].lpServiceName	= NULL;
			svcEntry[1].lpServiceProc	= NULL;
			StartServiceCtrlDispatcher(svcEntry);
		}

		//
		//	here you can plug in stuff to run while debugging,
		//	since this is the path you go under the interactive
		//	debugger. so you don't need to debug installed as service,
		//	just as any old exe.
		//
		
		return 0;
	}

	catch(char* msg)
	{
		MessageBox(NULL, msg, "Exception", MB_OK);
		return -1;
	}

	catch(...)
	{
		MessageBox(NULL, "Unknown error", "Exception", MB_OK);
		return -1;
	}
}

// --------------------------------------------------------------------------
// Install 
//
void Install(LPCSTR szServiceName)
throw(char*)
{
	char		szFilePath[256];

	SvcHandle hSCM(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
	if (hSCM.isnull())
		throw "Could not open Service Control Manager";

	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));

	SvcHandle hService (CreateService(  hSCM,							// handle to SC manager
										g_szServiceName,				// service name
										g_szServiceName,				// display name
										SERVICE_ALL_ACCESS,				// desired access
										SERVICE_WIN32_OWN_PROCESS,		// service type
										SERVICE_AUTO_START,				// start type
										SERVICE_ERROR_NORMAL,			// error control
										szFilePath,						// binary path name
										NULL,							// load order group
										NULL,							// tag ID
										NULL,							// dependencies
										NULL,							// service start name
										NULL));							// password

	if (hService.isnull())
		throw "Could not create service";
}

// --------------------------------------------------------------------------
// Uninstall 
//
void Uninstall(LPCSTR szServiceName)
throw(char*)
{
	SvcHandle hSCM (OpenSCManager(	NULL,						// machine name
									NULL,						// database name
									SC_MANAGER_ALL_ACCESS));	// desired access

	if (hSCM.isnull())
		throw "Could not open Service Control Manager";

	SvcHandle hService (OpenService(	hSCM,					// SC manager
										g_szServiceName,		// service name
										DELETE));				// desired access
	if (!hService.isnull())
		if (!DeleteService(hService))
			throw "Could not delete service";

}

// --------------------------------------------------------------------------
// ServiceMain 
//
VOID WINAPI	ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
throw()
{
	g_hSvc = RegisterServiceCtrlHandler(g_szServiceName, Handler);

	g_hSvcStat.dwServiceType				= SERVICE_WIN32_OWN_PROCESS;
	g_hSvcStat.dwCurrentState				= SERVICE_START_PENDING;
	g_hSvcStat.dwControlsAccepted			= SERVICE_ACCEPT_STOP;
	g_hSvcStat.dwWin32ExitCode				= NO_ERROR;
	g_hSvcStat.dwServiceSpecificExitCode	= NO_ERROR;
	g_hSvcStat.dwCheckPoint					= 0;
	g_hSvcStat.dwWaitHint					= 0;
	SetServiceStatus(g_hSvc, &g_hSvcStat);

	// ------------------------------------------------------------------------- //
	// here you should start your thread or whatever the program needs to do


	// ------------------------------------------------------------------------- //
	
	g_hSvcStat.dwCurrentState				= SERVICE_RUNNING;
	SetServiceStatus(g_hSvc, &g_hSvcStat);

	return;
}

// --------------------------------------------------------------------------
// Handler 
//
VOID WINAPI Handler(DWORD fdwControl)
{
	switch(fdwControl)
	{
	case SERVICE_CONTROL_STOP:

		g_hSvcStat.dwCurrentState	= SERVICE_STOP_PENDING;
		SetServiceStatus(g_hSvc, &g_hSvcStat);

		// ------------------------------------------------------------------------- //
		// here you should terminate your threads

		// ------------------------------------------------------------------------- //
				
		g_hSvcStat.dwCurrentState	= SERVICE_STOPPED;
		SetServiceStatus(g_hSvc, &g_hSvcStat);

		break;

	default:
		break;
	}
}

