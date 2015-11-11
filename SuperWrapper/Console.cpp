#include "StdAfx.h"
#include "Console.h"
#include "Utils.h"
#include "OLDatagramMaker.h"

CConsole g_SingletonConsole;

CConsole& CON()
{
	return g_SingletonConsole;
}


CConsole::CConsole()
{
	m_pDatagramSender = NULL;
}

CConsole::~CConsole(void)
{
	if (m_pDatagramSender)
	{
		delete m_pDatagramSender;
	}
}

CFile cfile_object;


void CConsole::Start(LPCTSTR szLogPath, LPCTSTR szSeed)
{
	m_strLogPath = ::SYSPathFile(szLogPath, ".log");
	COleDateTime odt = COleDateTime::GetCurrentTime();
	m_strFileName = _F_("%s_%d%d%d_%d%d%d.log", szSeed, 
						odt.GetYear(), odt.GetMonth(), odt.GetDay(), 
						odt.GetHour(), odt.GetMinute(), odt.GetSecond());

	::CreateDirectory(m_strLogPath, NULL);

	try
	{
		m_pDatagramSender = new CLOGDatagramSender();
	}
	catch(...)
	{
		puts("WARNING: No datagram sender running");
	}

	CString strFullFile = ::SYSPathFile(m_strLogPath, m_strFileName);

	CFileException E;
	if (!cfile_object.Open(strFullFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyWrite, &E))
	{
		puts("WARNING: Can't create log file. Additional info:");
		CString s;
		E.GetErrorMessage(s.GetBuffer(4096), 4096);
		s.ReleaseBuffer();
		puts(s);
	}
	
}



void CConsole::Print(LPCTSTR sz)
{
	// On console
	printf(sz);
	_flushall();

	// On file
	if (cfile_object.m_hFile != CFile::hFileNull)
	{
		cfile_object.Write(sz, strlen(sz)); 
		cfile_object.Flush();
	}

	// On net
	if (m_pDatagramSender)
	{
		COLDatagramMaker m_datagram;
		m_datagram.AddValue("text", sz);
		m_pDatagramSender->SendDatagram(&m_datagram);
	}
}

void CConsole::PrintLn(LPCTSTR sz)
{
	CString str = _F_("%s\n", sz);
	Print(str);
}
