#pragma once
#include "LOGDatagramSender.h"
#include "Utils.h"

class CConsole
{
public:
	CConsole();
	~CConsole(void);

protected:
	CString m_strLogPath;
	CString m_strFileName;
	CLOGDatagramSender* m_pDatagramSender;

public:
	void Start(LPCTSTR szLogPath, LPCTSTR szSeed);
	void Print(LPCTSTR sz);
	void PrintLn(LPCTSTR sz);
};

CConsole& CON();
