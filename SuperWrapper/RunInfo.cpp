#include "StdAfx.h"
#include "RunInfo.h"
#include "Utils.h"

CRunInfo::CRunInfo(void)
{
}

CRunInfo::~CRunInfo(void)
{
}

CString CRunInfo::BuildReport()
{
	CStringArray out;
	out.Add("Parameters:");
	out.Add(_F_("Service name: %s", m_strServiceName));
	out.Add(_F_("CmdLine: %s", m_strCmdLine));
	out.Add(_F_("StartupPath: %s", m_strStartupPath));
	//out.Add(_F_("ShutdownMethod: %s", m_strShutdownMethod));
	out.Add(_F_("ShutdownCommand: %s", m_strShutdownCommand));
	out.Add(_F_("User: %s", m_strUser));
	out.Add(_F_("Domain: %s", m_strDomain));
	return ::SYSBuildOneStringList(&out, "\n");
}
