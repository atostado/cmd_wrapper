// IOutputRedirector.cpp: implementation of the IOutputRedirector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PROCESS_IOutputRedirector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IOutputRedirector::IOutputRedirector()
{

}

IOutputRedirector::~IOutputRedirector()
{

}

CString IOutputRedirector::GetOutput()
{
	return m_strText;
}


void IOutputRedirector::OnOutputText(LPCTSTR szText)
{
	// Esto no hace nada.
	// Pensado para derivar...
	m_strText += szText;
}


void IOutputRedirector::OnProcessTerminated()
{
	// ASSERT(false);	// Esto no hace nada.
					// Pensado para derivar...
}

