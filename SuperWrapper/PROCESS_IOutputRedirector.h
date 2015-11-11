// IOutputRedirector.h: interface for the IOutputRedirector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOUTPUTREDIRECTOR_H__A411608C_3B65_439A_9C9E_5A890EB59A98__INCLUDED_)
#define AFX_IOUTPUTREDIRECTOR_H__A411608C_3B65_439A_9C9E_5A890EB59A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IOutputRedirector  
{
public:
	IOutputRedirector();
	virtual ~IOutputRedirector();

protected:
	CString m_strText;

public:
	CString GetOutput();
	virtual void OnOutputText(LPCTSTR szText);
	virtual void OnProcessTerminated();
};

#endif // !defined(AFX_IOUTPUTREDIRECTOR_H__A411608C_3B65_439A_9C9E_5A890EB59A98__INCLUDED_)
