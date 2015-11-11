#pragma once

class COLDatagramMaker
{
public:
	COLDatagramMaker(void);
	~COLDatagramMaker(void);

protected:
	CString m_strDatagram;

public:
	void Clear();
	void AddValue(LPCTSTR szValueName, LPCTSTR szValueText);
	CString ExportToXML();
};
