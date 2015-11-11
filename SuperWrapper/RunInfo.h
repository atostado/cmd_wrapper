#pragma once

class CRunInfo
{
public:
	CRunInfo(void);
	~CRunInfo(void);

public:
	CString m_strServiceName;
	CString m_strCmdLine;
	CString m_strStartupPath;
	//CString m_strShutdownMethod;
	CString m_strShutdownCommand;
	CString m_strUser;
	CString m_strPassword;
	CString m_strDomain;

public:
	CString BuildReport();
};
