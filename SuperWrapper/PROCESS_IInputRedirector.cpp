// IInputRedirector.cpp: implementation of the IInputRedirector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PROCESS_IInputRedirector.h"
#include "PROCESS_Process.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IInputRedirector::IInputRedirector()
{
	m_pProcess = NULL;
}

IInputRedirector::~IInputRedirector()
{

}

void IInputRedirector::_AttachToProcess(CProcess* pProcess)
{
	ASSERT(m_pProcess == NULL);

	m_pProcess = pProcess;
}


void IInputRedirector::SendText(LPCTSTR szData)
{
	if (m_pProcess == NULL)
	{
		ASSERT(false);
		return;
	}

	if (m_pProcess->IsRunning())
	{
		ASSERT(false);
		return;
	}

	DWORD dw;
	for(int i=0; i<(int)strlen(szData); i++)
	{
		char c = szData[i];
		::WriteFile(m_pProcess->m_hInputPipeWriteHandle, &c, 1, &dw, NULL);
	}

	::WriteFile(m_pProcess->m_hInputPipeWriteHandle, "\n", 1, &dw, NULL);
}





