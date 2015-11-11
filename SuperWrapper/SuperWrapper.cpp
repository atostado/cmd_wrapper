// SuperWrapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SuperWrapper.h"
#include "utils.h"
#include "conio.h"
#include "SputnikWorker.h"
#include "MainWorker.h"
#include "RunInfo.h"
#include "Console.h"
#include "SuperWrapperService.h"
#include "ServiceInstaller.h"
#include "StringEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif





enum CommandEnum
{
	INSTALL_SERVICE = 1,
	UNINSTALL_SERVICE = 2,
	RUN_ON_CONSOLE = 3,
	RUN_AS_SERVICE = 4,
	SHOW_HELP = 5,
	SHOW_RUNINFO = 6,
	SPUTNIK = 7
};



struct CommandLineParams
{
	CommandEnum command;
	CString strAccount;
	CString strPassword;
	DWORD mode;	// SERVICE_AUTO_START   SERVICE_DEMAND_START
};


CRunInfo g_RunInfo;
CommandLineParams g_CommandLineParams;


void ShowBanner()
{
	CON().PrintLn("SuperWrapper 1.3 - Omnilab Iberia 2012-2013");
	CON().PrintLn("--------------------------------------------------");
}



void LoadRunInfo()
{
	g_RunInfo.m_strServiceName = ::GetAlfa("SuperWrapper", "ServiceName", ::SYSGetExeName(), true);
	g_RunInfo.m_strCmdLine = ::GetAlfa("SuperWrapper", "CmdLine", (::SYSGetExeName() + " -t"), true);
	g_RunInfo.m_strStartupPath = ::GetAlfa("SuperWrapper", "StartupPath", "", true);
	if (g_RunInfo.m_strStartupPath.GetLength() == 0)
	{
		g_RunInfo.m_strStartupPath = ::SYSGetExeDirectory();
	}
	//g_RunInfo.m_strShutdownMethod = ::GetAlfa("SuperWrapper", "ShutdownMethod", "COMMAND", true);
	g_RunInfo.m_strShutdownCommand = ::GetAlfa("SuperWrapper", "ShutdownCommand", "", true);
	g_RunInfo.m_strUser =  ::GetAlfa("SuperWrapper", "User", "", true);
	g_RunInfo.m_strPassword =  ::GetAlfa("SuperWrapper", "Password", "", true);
	g_RunInfo.m_strDomain =  ::GetAlfa("SuperWrapper", "Domain", "", true);

	if (g_RunInfo.m_strStartupPath == "")
	{
		g_RunInfo.m_strStartupPath = ::SYSGetRunDirectory();
	}
}



void CrackArgs(int argc, _TCHAR* argv[])
{
	g_CommandLineParams.command = SHOW_HELP;
	g_CommandLineParams.mode = SERVICE_DEMAND_START;

	for(int i=0; i<argc; i++)
	{
		CStringEx arg = CString(argv[i]).MakeUpper();
		if (arg.StartBy("-USR:"))
		{
			g_CommandLineParams.strAccount = CString(argv[i]).Mid(5);
		}
		if (arg.StartBy("-PWD:"))
		{
			g_CommandLineParams.strPassword = CString(argv[i]).Mid(5);
		}
		if (arg == "-MODE:AUTO")
		{
			g_CommandLineParams.mode = SERVICE_AUTO_START;
		}
		if (arg == "-MODE:MANUAL")
		{
			g_CommandLineParams.mode = SERVICE_DEMAND_START;
		}
		if (arg == "-I") g_CommandLineParams.command = INSTALL_SERVICE;
		if (arg == "-U") g_CommandLineParams.command = UNINSTALL_SERVICE;
		if (arg == "-C") g_CommandLineParams.command = RUN_ON_CONSOLE;
		if (arg == "-S") g_CommandLineParams.command = RUN_AS_SERVICE;
		if (arg == "-R") g_CommandLineParams.command = SHOW_RUNINFO;
		if (arg == "-K") g_CommandLineParams.command = SPUTNIK;
	}
}



// ===============================================================================

void DoShowHelp()
{
	CON().PrintLn("Usage:");
	CON().PrintLn(" SuperWrapper [-i|-u|-c|-s|-r|-k] [-usr:Account] [-pwd:Password] [-mode:Manual|Auto]");
	CON().PrintLn("   -i  Install");
	CON().PrintLn("       Allowed params: <usr>, <pwd> and <mode>   (Default mode: Manual)");
	CON().PrintLn("       Example 1: omnitool2 -i -usr:.\\alan -pwd:kjiere -mode:auto");
	CON().PrintLn("       Example 2: omnitool2 -i -usr:mydomain\\linda -pwd:cat54 -mode:manual");
	CON().PrintLn("   -u  Uninstall");
	CON().PrintLn("   -c  Run as console");
	CON().PrintLn("   -s  Run as service");
	CON().PrintLn("   -r  Show run info");
	CON().PrintLn("   -k  Sputnik mode (Only for tests. Beeps every 2 seconds)");
	CON().PrintLn("");


}

void DoShowRunInfo()
{
	CON().PrintLn(g_RunInfo.BuildReport());
}

void DoInstallService()
{
	char szServiceName[256];
	strcpy(szServiceName, g_RunInfo.m_strServiceName);

	char dependencies[2];
	dependencies[0] = 0;
	dependencies[1] = 0;

	char* szAccount = NULL;
	char* szPassword = NULL;

	if (g_CommandLineParams.strAccount.GetLength() > 0 &&
		g_CommandLineParams.strPassword.GetLength())
	{
		szAccount = g_CommandLineParams.strAccount.GetBuffer(255);
		szPassword = g_CommandLineParams.strPassword.GetBuffer(255);
	}

	InstallService(szServiceName, szServiceName,
		g_CommandLineParams.mode, 
		dependencies, 
		szAccount,
		szPassword,
		"-s");

	if (szAccount && szPassword)
	{
		g_CommandLineParams.strAccount.ReleaseBuffer();
		g_CommandLineParams.strPassword.ReleaseBuffer();
	}
}

void DoUninstallService()
{
	char szServiceName[256];
	strcpy(szServiceName, g_RunInfo.m_strServiceName);
	UninstallService(szServiceName);
}

void DoRunAsConsole()
{
	CMainWorker W;
	W.Run(&g_RunInfo);

	char c;

	do
	{
		char sz[1024];
		CON().PrintLn("\n\n<<Pulse X para finalizar>>\n");
		c = getch();
	} while (c != 'X' && c != 'x');

	W.Stop();
}

void DoRunAsService()
{
	CSuperWrapperService S(g_RunInfo.m_strServiceName);	
	S.SetRunInfo(&g_RunInfo);
	S.Run(S);
}

void DoSputnikDance()
{
	CSputnikWorker sputnik;
	sputnik.Run();
}

// ===============================================================================

int main2(int argc, TCHAR* argv[], TCHAR* envp[])
{
	LoadRunInfo();
	CON().Start(g_RunInfo.m_strStartupPath, g_RunInfo.m_strServiceName);
	ShowBanner();

	if (g_RunInfo.m_strStartupPath.GetLength() > 0)
	{
		::SetCurrentDirectory(g_RunInfo.m_strStartupPath);
	}
	else
	{
		::SetCurrentDirectory(::SYSGetExeDirectory());
	}

	CrackArgs(argc, argv);

	switch (g_CommandLineParams.command)
	{
	case SHOW_HELP:
		DoShowHelp();
		break;
	case SHOW_RUNINFO:
		DoShowRunInfo();
		break;
	case INSTALL_SERVICE:
		DoInstallService();
		break;
	case UNINSTALL_SERVICE:
		DoUninstallService();
		break;
	case RUN_ON_CONSOLE:
		DoRunAsConsole();
		break;
	case RUN_AS_SERVICE:
		DoRunAsService();
		break;
	case SPUTNIK:
		DoSputnikDance();
		break;
	}

#ifdef _DEBUG
//	CON().PrintLn("Press a key...");
//    getch();
#endif

	return 0;
}





// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{

		if (!AfxSocketInit())
		{
			AfxMessageBox("Error init sockets");
			return FALSE;
		}

		nRetCode = main2(argc, argv, envp);		
	}

	return nRetCode;
}
