// IInputRedirector.h: interface for the IInputRedirector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IINPUTREDIRECTOR_H__8AA14F97_BED4_4283_8D79_9DEB5B3D6DCF__INCLUDED_)
#define AFX_IINPUTREDIRECTOR_H__8AA14F97_BED4_4283_8D79_9DEB5B3D6DCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProcess;

class IInputRedirector  
{
	friend CProcess;

public:
	IInputRedirector();
	virtual ~IInputRedirector();

	void SendText(LPCTSTR szData);


private:	// private, no protected!!!!!
	CProcess* m_pProcess;
	void _AttachToProcess(CProcess* pProcess);

};

#endif // !defined(AFX_IINPUTREDIRECTOR_H__8AA14F97_BED4_4283_8D79_9DEB5B3D6DCF__INCLUDED_)
