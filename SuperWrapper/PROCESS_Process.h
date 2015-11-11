// Process.h: interface for the CProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESS_H__D290D183_7CD7_412F_8125_477FE4858936__INCLUDED_)
#define AFX_PROCESS_H__D290D183_7CD7_412F_8125_477FE4858936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class IOutputRedirector;

class CProcess  
{
public:
	CProcess(LPCTSTR szUser = NULL, LPCTSTR szPass = NULL, LPCTSTR szDomain = NULL);
	virtual ~CProcess();

// ============================
// Estáticos
// ============================
public:
	static DWORD GetParentPID(DWORD dwPID);
	static void GetChildsPID(DWORD dwPID, CArray<DWORD,DWORD>* pOutPIDS);
	static BOOL TerminateProcessByPID(DWORD dwProcessId, UINT uExitCode);

public:
	HANDLE					m_hPipeReadHandle;
	HANDLE					m_hPipeWriteHandle;

	HANDLE					m_hInputPipeReadHandle;
	HANDLE					m_hInputPipeWriteHandle;

	PROCESS_INFORMATION		m_ProcessInfo;
	SECURITY_ATTRIBUTES		m_SecurityAttributes;
	STARTUPINFO				m_StartupInfo;
	STARTUPINFOW				m_StartupInfoW;

	bool m_bControlC;

	IOutputRedirector* m_pOutputRedirector;

	DWORD m_dwExitCode;

	CString m_strUser, m_strPass, m_strDomain;
	CString m_strLastErrorMessage;

	bool m_bDebugMode;

public:
	bool Exec2(LPCTSTR szCmdLine);
	void SetDebugMode(bool b);
	CString GetLastErrorMessage();

	int Open_WinExec(LPCTSTR szCmdLine, UINT uCmdShow);
	bool Open_Console(
					LPCTSTR szCmdLine, 
					IOutputRedirector* pOutPipe);

	void SendControlC();

	void Kill();

	bool IsRunning();

	void SendTextToConsole(LPCTSTR szText);

	bool GetExitCode(DWORD* dwoutExitCode);
	DWORD WaitForExitCode();

// protected:	// Debería ser protected pero la necesitamos
				// pública para tener acceso a ella desde el
				// thread.
public:	
	void _Clear();


};

#endif // !defined(AFX_PROCESS_H__D290D183_7CD7_412F_8125_477FE4858936__INCLUDED_)
