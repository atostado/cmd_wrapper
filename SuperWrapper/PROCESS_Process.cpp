// Process.cpp: implementation of the CProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PROCESS_Process.h"
#include "PROCESS_IOutputRedirector.h"
#include "PROCESS_IInputRedirector.h"
#include "Console.h"
#include "SendSignal.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE ghJob = CreateJobObject( NULL, NULL); // GLOBAL
//HANDLE ghJob = NULL;





#define NO_EXIT_CODE 0xFEFEFEFE

CString SYSBuildSystemErrorReport(DWORD dwErrorCode)
{
	LPVOID lpMsgBuf = NULL;
	CString strErrorMessage;

	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		strErrorMessage.Format("#Error interno# [0x%08x]", dwErrorCode);
	}
	else
	{
		// Display the string.
		strErrorMessage = (LPCTSTR)lpMsgBuf;
		CString x;
		x.Format("%08x", dwErrorCode);
		strErrorMessage += x;
		strErrorMessage.Replace("\r", "");
		strErrorMessage.Replace("\n", "");
		//strErrorMessage.Format("%s [0x%08x]", lpMsgBuf, dwErrorCode);

		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	
	return strErrorMessage;
}


CString PrintLastError()
{
	DWORD dw = ::GetLastError();
	CString str = ::SYSBuildSystemErrorReport(dw);
	CON().PrintLn(str);
	return str;
}



DWORD CProcess::GetParentPID(DWORD dwPID)
{
	DWORD dwParentPID = 0;
	PROCESSENTRY32 pe32;

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	Process32First(hProcessSnap, &pe32);

	do
	{
		//printf("%s %d %d\n", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);
		if (dwPID == pe32.th32ProcessID)
		{
			dwParentPID = pe32.th32ParentProcessID;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle( hProcessSnap );

	return dwParentPID;
}



void CProcess::GetChildsPID(DWORD dwPID, CArray<DWORD,DWORD>* pOutPIDS)
{
	pOutPIDS->RemoveAll();	

	DWORD dwParentPID = 0;
	PROCESSENTRY32 pe32;

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	Process32First(hProcessSnap, &pe32);

	do
	{
		//printf("%s %d %d\n", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);
		if (dwPID == pe32.th32ParentProcessID)
		{
			pOutPIDS->Add(pe32.th32ProcessID);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle( hProcessSnap );
}








//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

			void CProcess::_Clear()
			{
				m_bControlC = false;
				::ZeroMemory(&m_ProcessInfo, sizeof(m_ProcessInfo));
				m_pOutputRedirector = NULL;
			}


CProcess::CProcess(LPCTSTR szUser, LPCTSTR szPass, LPCTSTR szDomain)
{
	m_bDebugMode = false;

	m_strUser = szUser;
	m_strPass = szPass;
	m_strDomain = szDomain;

	_Clear();
}

CProcess::~CProcess()
{

}


int CProcess::Open_WinExec(LPCTSTR szCmdLine, UINT uCmdShow)
{ 
	PROCESS_INFORMATION stProcessInfo;
    STARTUPINFO stStartUpInfo;

    memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));

    stStartUpInfo.cb = sizeof(STARTUPINFO);

    stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
    stStartUpInfo.wShowWindow = uCmdShow;

    if (CreateProcess(NULL, (LPTSTR)szCmdLine, NULL, NULL, FALSE,
        NORMAL_PRIORITY_CLASS, NULL,
        NULL, &stStartUpInfo, &stProcessInfo))
	{
        ::WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
	}

	return 0;
}


	UINT CProcess_ConsoleThread( LPVOID pParam )
	{
		CProcess* _THIS = (CProcess*)pParam;

		DWORD BytesLeftThisMessage = 0;
		DWORD NumBytesRead;
		TCHAR PipeData[8192];	// BUF_SIZE
		DWORD TotalBytesAvailable = 0;
		// ***********************************************************************************
		::SetCurrentDirectory("C:\\");
		MCIERROR err = mciSendString("open C:\\01Solitary.wav alias myFile", NULL, 0, 0);
		char sz[1024] = "";
		mciGetErrorString(
				err, sz, 1024); 
		mciSendString("play myFile", NULL, 0, 0);

		while(true)
		{
			NumBytesRead = 0;

			::PeekNamedPipe(_THIS->m_hPipeReadHandle, PipeData, 1, &NumBytesRead, &TotalBytesAvailable, &BytesLeftThisMessage);
			//::PeekNamedPipe(_THIS->m_hPipeReadHandle, NULL, 0, NULL, &TotalBytesAvailable, NULL);

			if (NumBytesRead)
			{
				::ReadFile(_THIS->m_hPipeReadHandle, PipeData, 8192 - 1, &NumBytesRead, NULL);

				PipeData[NumBytesRead] = '\0';

				for(DWORD ii=0; ii<NumBytesRead; ii++)
				{
					if (PipeData[ii] == '\b')
					{
						PipeData[ii] = ' ';
					}
				}

				// Enviamos los datos a la salida.
				_THIS->m_pOutputRedirector->OnOutputText(PipeData);
			}
			else
			{
				if (WaitForSingleObject(_THIS->m_ProcessInfo.hProcess, 0) == WAIT_OBJECT_0)
				{
					break;
				}

				if (_THIS->m_bControlC)
				{
					::TerminateProcess(_THIS->m_ProcessInfo.hProcess, 0);
					break;
				}

				::Sleep(100);
			}

		}		// Del while

		// Cogemos el ExitCode.
		BOOL b = ::GetExitCodeProcess(_THIS->m_ProcessInfo.hProcess, &(_THIS->m_dwExitCode));


		::CloseHandle(_THIS->m_ProcessInfo.hThread);
		::CloseHandle(_THIS->m_ProcessInfo.hProcess);
		::CloseHandle(_THIS->m_hPipeReadHandle);
		::CloseHandle(_THIS->m_hPipeWriteHandle);

		::CloseHandle(_THIS->m_hInputPipeReadHandle);
		::CloseHandle(_THIS->m_hInputPipeWriteHandle);



		// Esto que nos valga como marca de que
		// ha terminado el proceso.
		_THIS->m_ProcessInfo.hProcess = NULL;
		_THIS->m_pOutputRedirector->OnProcessTerminated();
		_THIS->_Clear();

		mciSendString("close myFile", NULL, 0, 0);

		return 0;
	}

bool CProcess::Exec2(LPCTSTR szCmdLine)
{
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcInfo;
	TOKEN_PRIVILEGES tp;
	memset(&ProcInfo, 0, sizeof(ProcInfo));
	memset(&StartInfo, 0 , sizeof(StartInfo)); 
	StartInfo.cb = sizeof(StartInfo); 
	HANDLE handle = NULL;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ALL_ACCESS, &handle)) 
	{
		printf("\nOpenProcessError");
	}

	if (!LookupPrivilegeValue(NULL,SE_TCB_NAME,
		//SE_TCB_NAME,
		&tp.Privileges[0].Luid)) 
	{
			printf("\nLookupPriv error");
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes =
		SE_PRIVILEGE_ENABLED;//SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(handle, FALSE, &tp, 0, NULL, 0)) 
	{
		printf("\nAdjustToken error");
	}

	bool i = LogonUser(m_strUser, m_strDomain, this->m_strPass, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &handle);
	printf("\nLogonUser return  : %d",i);
	PrintLastError();
	printf("\nLogonUser getlast : %d",i);
	if (! ImpersonateLoggedOnUser(handle) ) 
	{
		printf("\nImpLoggedOnUser!");
	}


	CString sz = szCmdLine;

	CString sz2;

	//sz="c:\\_stack3\\_utils\\curl\\curl.exe";

	i = CreateProcessAsUser(handle, NULL,sz.GetBuffer(2048),NULL, NULL, true, 
		//CREATE_UNICODE_ENVIRONMENT |NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, NULL, 
		0, NULL, NULL, 
		&m_StartupInfo, &m_ProcessInfo);  
	PrintLastError();
	sz.ReleaseBuffer();
	printf("\nCreateProcessAsUser return  : %d",i);

	return true;
}



bool CProcess::Open_Console(
				LPCTSTR szCmdLine, 
				IOutputRedirector* pOutPipe)
{
	m_dwExitCode = NO_EXIT_CODE;
	m_pOutputRedirector = pOutPipe;

	// Localizamos el directorio actual
	// ------------------------------------------------
	CString strCurDir;
	::GetCurrentDirectory(_MAX_PATH, strCurDir.GetBuffer(_MAX_PATH + 1));
	strCurDir.ReleaseBuffer();

	// Preparamos variables y estructuras
	// ------------------------------------------------

	m_bControlC = false;

	ZeroMemory(&m_StartupInfo, sizeof(m_StartupInfo));
	ZeroMemory(&m_StartupInfoW, sizeof(m_StartupInfoW));
	ZeroMemory(&m_ProcessInfo, sizeof(m_ProcessInfo));

	// Los pipes...
	// ------------------------------------------------
	// PIPE 1
	ZeroMemory(&m_SecurityAttributes, sizeof(m_SecurityAttributes));
		m_SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		m_SecurityAttributes.bInheritHandle = TRUE;
		m_SecurityAttributes.lpSecurityDescriptor = NULL;
	::CreatePipe(&m_hPipeReadHandle, &m_hPipeWriteHandle, &m_SecurityAttributes, 0);

	// PIPE 2
	ZeroMemory(&m_SecurityAttributes, sizeof(m_SecurityAttributes));
		m_SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		m_SecurityAttributes.bInheritHandle = TRUE;
		m_SecurityAttributes.lpSecurityDescriptor = NULL;
	::CreatePipe(&m_hInputPipeReadHandle, &m_hInputPipeWriteHandle, &m_SecurityAttributes, 0);


	char szCommand[_MAX_PATH + 1];
	::strcpy(szCommand, szCmdLine);	// Hace falta un puntero sin CONST.

	
	if (m_strUser.IsEmpty())
	{
		// El proceso...
		// ------------------------------------------------
		m_StartupInfo.cb = sizeof(STARTUPINFO);
		m_StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES /*| CREATE_NEW_PROCESS_GROUP*/;
		m_StartupInfo.wShowWindow = SW_HIDE;
		m_StartupInfo.hStdOutput = m_hPipeWriteHandle;
		m_StartupInfo.hStdError = m_hPipeWriteHandle;
		m_StartupInfo.hStdInput = m_hInputPipeReadHandle;

		if (!::CreateProcess(NULL, szCommand, NULL, NULL, TRUE, 0, NULL, strCurDir, &m_StartupInfo, &m_ProcessInfo))
		{
			DWORD dw = ::GetLastError();
			m_strLastErrorMessage = SYSBuildSystemErrorReport(dw);
			return false;
		}

		// ======================
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

		// Configure all child processes associated with the job to terminate when the
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		if( 0 == SetInformationJobObject( ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
		{
			::MessageBox( 0, "Could not SetInformationJobObject", "TEST", MB_OK);
		}

		BOOL bok = AssignProcessToJobObject( ghJob, m_ProcessInfo.hProcess);
		::GetCurrentDirectory(0,NULL);
		// ======================


	}
	else
	{
		Exec2(szCommand);
/*
		STARTUPINFOW si2;
		::ZeroMemory(&si2, sizeof(STARTUPINFOW));
		si2.cb          = sizeof(si2);
		// -------------------------------------------
		// En fins, queda así. No sé cómo conseguir que
		// coja las pipes de StdIn y StdOut. Da error de "Controlador no válido".
		// Tengo prisa y esto parece que funciona...
		// -------------------------------------------
		si2.dwFlags = STARTF_USESHOWWINDOW;

		_bstr_t user = m_strUser;
		_bstr_t pass = m_strPass;
		_bstr_t domain = m_strDomain;
		_bstr_t command = szCommand;
		_bstr_t directory = strCurDir;

		// Si alguien pulsa CONTROL+CONTROL, que aparezca la pantalla
		// negra...
		if (m_bDebugMode)
		{
			CString s;
			s.Format("Usuario: <%s>\r\n"
					"Contraseña: <%s>\r\n"
					"Dominio: <%s>\r\n"
					"Comando: <%s>\r\n"
					"Directorio actual: <%s>\r\n", 
					(LPCTSTR)m_strUser,
					(LPCTSTR)m_strPass, 
					(LPCTSTR)m_strDomain, 
					(LPCTSTR)szCommand, 
					(LPCTSTR)strCurDir);
			::AfxMessageBox(s);

			si2.wShowWindow = SW_SHOW;

			CString strCommand;
			strCommand.Format("cmd.exe /c \"%s\" & pause", szCommand);
			command = strCommand;
		}
		else
		{
			si2.wShowWindow = SW_HIDE;
			command = szCommand;
		}

		BOOL bOk2 = CreateProcessWithLogonW(user, domain, pass, LOGON_WITH_PROFILE,
			NULL, command, CREATE_DEFAULT_ERROR_MODE, NULL, directory, &si2, &m_ProcessInfo);

		if (!bOk2)
		{
			DWORD dw = ::GetLastError();
			m_strLastErrorMessage = SYSBuildSystemErrorReport(dw);
			CON().PrintLn(_F_("ERROR CreateProcessWithLogonW: %s", (LPCTSTR)m_strLastErrorMessage));
			return false;
		}

*/
	}
	::AfxBeginThread(CProcess_ConsoleThread, (LPVOID)this);

	return true;
}





void CProcess::SendControlC()
{
	m_bControlC = true;

	// ---------------
	/*
		// Si hay hijos, hay que eliminarlos también.
		CArray<DWORD, DWORD> childs;
		GetChildsPID(m_ProcessInfo.dwProcessId, &childs);
		for(int i=0; i<childs.GetSize(); i++)
		{
			SendSignal(childs[i]);
		}
	*/
	// ---------------

	// Muy importante!!!
	// Con esto evitamos que al enviar Ctrl-C se lo trague
	// nuestro proceso.
	/*
	BOOL b = SetConsoleCtrlHandler(NULL, TRUE);
	if (!b)
	{
		DWORD dwErrorCode = ::GetLastError();
		CString str = SYSBuildSystemErrorReport(dwErrorCode);
		CON().PrintLn(_F_("Error ConsoleCtrlHandler: %s", (LPCTSTR)str));
	}
	*/
	DWORD dwTargetPID = m_ProcessInfo.dwProcessId;

	try
	{
		// Atención, atención, a todas las unidades!
		// El 0 indica que se envía el evento a la consola
		// actual. Como hemos llamado antes a SetConsoleCtrlHandler
		// el Ctrl-C llega sin problemas a quién lo está esperando.
		//BOOL ret = GenerateConsoleCtrlEvent (CTRL_C_EVENT, 0);
		BOOL ret = GenerateConsoleCtrlEvent (CTRL_C_EVENT, m_ProcessInfo.dwProcessId);
		DWORD dwErrorCode = ::GetLastError();
		CString str = SYSBuildSystemErrorReport(dwErrorCode);
		CON().PrintLn(_F_("CTRL-C: %s", (LPCTSTR)str));
	}
	catch(...)
	{
		CON().PrintLn("Exception in CTRL-C...");
	}
	return;

	DWORD timeout = 1000;	// 1 segundo.

	// Esperamos un segundo. Si no se ha parado, lo paramos.
	DWORD dwTime = ::GetTickCount();
	while(IsRunning() || (GetTickCount() > (dwTime + timeout)))
	{
		::Sleep(100);
	}

	if (IsRunning())
	{
		::TerminateProcess(m_ProcessInfo.hProcess, 0);
	}
}


void CProcess::Kill()
{
	if (IsRunning())
	{
		// Si hay hijos, hay que eliminarlos también.
		CArray<DWORD, DWORD> childs;
		GetChildsPID(m_ProcessInfo.dwProcessId, &childs);
		for(int i=0; i<childs.GetSize(); i++)
		{
			CProcess::TerminateProcessByPID(childs[i], 33);
		}
		::TerminateProcess(m_ProcessInfo.hProcess, 33);

		_Clear();
	}
}


bool CProcess::IsRunning()
{
	return (m_ProcessInfo.hProcess != NULL);
}




void CProcess::SendTextToConsole(LPCTSTR szData)
{
	if (IsRunning())
	{
		DWORD dw;
		for(int i=0; i<(int)strlen(szData); i++)
		{
			char c = szData[i];
			::WriteFile(m_hInputPipeWriteHandle, &c, 1, &dw, NULL);
		}

		::WriteFile(m_hInputPipeWriteHandle, "\n", 1, &dw, NULL);
	}
}


CString CProcess::GetLastErrorMessage()
{
	return m_strLastErrorMessage;
}


void CProcess::SetDebugMode(bool b)
{
	m_bDebugMode = b;
}


bool CProcess::GetExitCode(DWORD* dwoutExitCode)
{
	if (m_dwExitCode == NO_EXIT_CODE)
	{
		return false;
	}

	*dwoutExitCode = m_dwExitCode;

	return true;
}

DWORD CProcess::WaitForExitCode()
{
	while(IsRunning())
	{
		::Sleep(50);
	}

	::Sleep(20);	// De propina

	return m_dwExitCode;
}










/*
void Childs()
{
	DWORD ProcessesIDs[1024], cbNeeded, cProcesses;
	unsigned int i;

	// The default of <unknown> is given so that if GetModuleBaseName does not return
	// the base name of the module then <unknown> will be printed instead of the base name.
	TCHAR szProcessName[50] = TEXT("<unknown>");

	// if Enumprocess returns zero (fails) then quit the program.
	if ( !EnumProcesses( ProcessesIDs, sizeof(ProcessesIDs), &cbNeeded ) )
		return;

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// This for loop will be enumerating each process.
	for ( i = 0; i < cProcesses; i++ )
	{
		// Get a handle to the process. The process to which the handle will be returned 
		// will depend on the variable i.
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, ProcessesIDs[i] );



		// Get the process name.
		if (NULL != hProcess )
		{
			GetModuleBaseName( hProcess, NULL, szProcessName,
				sizeof(szProcessName)/sizeof(TCHAR) );
		}
		// Print the process name and identifier.
		_tprintf( TEXT("Processname = %s, PID = %u \n\n"), szProcessName, ProcessesIDs[i]);
		//Every handle is to be closed after its use is over.
		CloseHandle( hProcess );
		//End of for loop.
	}
}
*/



BOOL CProcess::TerminateProcessByPID(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle  = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}
