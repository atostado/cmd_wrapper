#include "StdAfx.h"
#include "Utils.h"
#include <string>

// ------------------------------------------------
// Montaje para coger el directorio de ejecución
// Creamos una clase que instanciamos y en el
// constructor cogemos el directorio inicial.

class SYSConstructor
{
public:
	CString m_strRunDir;

	SYSConstructor()
	{
		char szBuffer[_MAX_PATH];
		::GetCurrentDirectory(_MAX_PATH, szBuffer);
		m_strRunDir = szBuffer;
	}
};

SYSConstructor g_RunDir;

CString SYSGetRunDirectory()
{
	return g_RunDir.m_strRunDir;
}


void SYSSetRunDirectory(LPCTSTR szRunDirectory)
{
	g_RunDir.m_strRunDir = szRunDirectory;
}



CString SYSGetExeDirectory()
{
	char szPath[MAX_PATH + 1];

	::GetModuleFileName(NULL, szPath, MAX_PATH + 1);

	CString strReturn;
	for(int i=lstrlen(szPath)-1; i--; (i >= 0))
	{
		if (szPath[i] == '\\')
		{
			szPath[i] = 0;
			strReturn = szPath;
			break;
		}
	}

	// ¿Qué es esto?
	// ASSERT(strReturn.length() > 0);

	return strReturn;

}



CString SYSGetExeName()
{
	char szPath[MAX_PATH + 1];

	::GetModuleFileName(NULL, szPath, MAX_PATH + 1);

	CString strReturn;
	int l = lstrlen(szPath);
	for(int i=l-1; i--; (i >= 0))
	{
		if (szPath[i] == '\\')
		{
			// ... y quitamos los últimos 4 caracteres.
			szPath[l - 4] = 0;
			strReturn = szPath + i + 1;
			break;
		}
	}

	// ¿Qué es esto?
	// ASSERT(strReturn.length() > 0);

	return strReturn;

}



CString _F_(LPCTSTR szFormatString, ...)
{
	CString str;

	try
	{
	va_list args;
	va_start(args, szFormatString);

	str.FormatV(szFormatString, args);
	va_end(args);
	}
	catch(...)
	{
		str = CString("#ERR:szFormatString");
	}

	return str;
}

 



CString _N_(int i)
{
	CString str;
	str.Format("%d", i);
	return str;
}


CString _GetConfFileName()
{
	return ::SYSPathFile(::SYSGetExeDirectory(), (::SYSGetExeName() + ".ini"));
}

 
long GetInt(const char* szGroup, const char* szValue, long lDefValue, bool /*bSave*/)
{
	long l = ::GetPrivateProfileInt(szGroup, szValue, lDefValue, _GetConfFileName());

	return l;
}

void SetInt(const char* szGroup, const char* szValue, long lNewValue)
{
	::WritePrivateProfileString(szGroup, szValue, _N_(lNewValue), _GetConfFileName());
}


CString GetAlfa(const char* szGroup, const char* szValue, const char* szDefValue, bool bSave)
{
	char szBuffer[1024];

	::GetPrivateProfileString(
			szGroup, 
			szValue, 
			szDefValue, 
			szBuffer, 
			1024, 
			_GetConfFileName());

	if (bSave)
	{
		::SetAlfa(szGroup, szValue, szBuffer);
	}

	return CString(szBuffer);
}


void SetAlfa(const char* szGroup, const char* szValue, const char* szAlfa)
{
	::WritePrivateProfileString(szGroup, szValue, szAlfa, _GetConfFileName());
}

// Se coge un string array y te lo devuelve en una
// sola cadena con el separador... szSeparator.
CString SYSBuildOneStringList(const CStringArray* pSA, LPCTSTR szSeparator)
{
	CString str;
	for(int i=0; i<pSA->GetSize(); i++)
	{
		if (i > 0)
		{
			str += szSeparator;
		}

		str += pSA->GetAt(i);
	}

	return str;
}


//AT_20091119
CString SYSBuildOneUIntList(const CUIntArray* pUIA, LPCTSTR szSeparator)
{
	CString str;
	for(int i=0; i<pUIA->GetSize(); i++)
	{
		if (i > 0)
		{
			str += szSeparator;
		}

		str += _F_("%d", pUIA->GetAt(i));
	}

	return str;
}




// Pone un slash entre el path y el file y
// se cuida de que no sobren.
CString SYSPathFile(LPCTSTR szPath, LPCTSTR szFile)
{
	CString str;

	str = CString(szPath) + CString("\\") + szFile;

	while (str.Replace("\\\\", "\\"));

	// Atención!!! Rutas UNC
	if (*szPath == '\\')
	{
		str = CString("\\") + str;
	}

	return str;
}
