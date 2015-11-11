#include "stdafx.h"
#include "StringEx.h"




// GetField 	- Gets a delimited field
// Returns	- A CStringEx object that contains a copy of 
//		  the specified field
// delim	- The delimiter string
// fieldnum 	- The field index - zero is the first
// NOTE 	- Returns the whole string for field zero
//		  if delim not found
//		  Returns empty string if # of delim found
//		  is less than fieldnum
CStringEx CStringEx::GetField( LPCTSTR delim, int fieldnum)
{
	LPTSTR lpsz;
	LPTSTR lpszRemainder = LPTSTR(operator LPCTSTR());
	LPTSTR lpszret = NULL;
	int retlen = 0, lenDelim = lstrlen( delim );

	while( fieldnum-- >= 0 )
	{
		lpszret = lpszRemainder;
		lpsz = _tcsstr(lpszRemainder, delim);
		if( lpsz )
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + lenDelim;
		}
		else
		{
			retlen = lstrlen( lpszRemainder );
			lpszRemainder += retlen;	// change to empty string
		}
	}

	ASSERT(lpszret != NULL);

	return Mid( lpszret - LPTSTR(operator LPCTSTR()), retlen );
}

// GetField 	- Gets a delimited field
// Returns	- A CStringEx object that contains a copy of 
//		  the specified field
// delim	- The delimiter char
// fieldnum 	- The field index - zero is the first
// NOTE 	- Returns the whole string for field zero
//		  if delim not found
//		  Returns empty string if # of delim found
//		  is less than fieldnum
CStringEx CStringEx::GetField( TCHAR delim, int fieldnum)
{
	LPTSTR lpsz, lpszRemainder = LPTSTR(operator LPCTSTR()), lpszret = NULL;
	int retlen = 0;

	while( fieldnum-- >= 0 )
	{
		lpszret = lpszRemainder;
		lpsz = _tcschr(lpszRemainder, (_TUCHAR)delim);
		if( lpsz )
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + 1;
		}
		else
		{
			retlen = lstrlen( lpszRemainder );
			lpszRemainder += retlen;	// change to empty string
		}
	}

	ASSERT(lpszret != NULL);

	return Mid( lpszret - LPTSTR(operator LPCTSTR()), retlen );
}


// GetFieldCount	- Get number of fields in a string
// Returns		- The number of fields
//			  Is always greater than zero
// delim		- The delimiter character
int CStringEx::GetFieldCount( TCHAR delim )
{
	TCHAR a[2];
	a[0] = delim;
	a[1] = 0;
	return GetFieldCount( a );
}


// GetFieldCount	- Get number of fields in a string
// Returns		- The number of fields
//			  Is always greater than zero
// delim		- The delimiter string
int CStringEx::GetFieldCount( LPCTSTR delim )
{
	LPTSTR lpsz, lpszRemainder = LPTSTR(operator LPCTSTR());
	int lenDelim = lstrlen( delim );

	int iCount = 1;
	while( (lpsz = _tcsstr(lpszRemainder, delim)) != NULL )
	{
		lpszRemainder = lpsz + lenDelim;
		iCount++;
	}

	return iCount;
}


// GetDelimitedField	- Finds a field delimited on both ends
// Returns		- A CStringEx object that contains a copy of 
//			  the specified field
// delimStart		- Delimiter at the start of the field
// delimEnd 		- Delimiter at the end of the field
CStringEx CStringEx::GetDelimitedField( LPCTSTR delimStart, LPCTSTR delimEnd, int fieldnum /*= 0*/)
{
	LPTSTR lpsz, lpszEnd = NULL, lpszRet = NULL, lpszRemainder = LPTSTR(operator LPCTSTR()); 
	int lenDelimStart = lstrlen( delimStart ), lenDelimEnd = lstrlen( delimEnd );

	while( fieldnum-- >= 0 )
	{
		lpsz = _tcsstr(lpszRemainder, delimStart);
		if( lpsz )
		{
			// We did find the Start delim
			lpszRet = lpszRemainder = lpsz + lenDelimStart;
			lpszEnd = _tcsstr(lpszRemainder, delimEnd);
			if( lpszEnd == NULL ) return"";
			lpszRemainder = lpsz + lenDelimEnd;
		}
		else return "";
	}

	ASSERT(lpszEnd != NULL && lpszRet != NULL);

	return Mid( lpszRet - LPTSTR(operator LPCTSTR()), lpszEnd - lpszRet );
}



// ------------------------------------------------------------
// Añadidos a la clase.


long CStringEx::GetFieldLong( TCHAR delim, int fieldnum)
{
	CStringEx str=GetField(delim,fieldnum);
	return atol(str);
}

long CStringEx::GetFieldLong( LPCTSTR delim, int fieldnum)
{
	CStringEx str=GetField(delim,fieldnum);
	return atol(str);
}


double CStringEx::GetFieldDouble( TCHAR delim, int fieldnum)
{
	CStringEx str=GetField(delim,fieldnum);
	return atof(str);
}

double CStringEx::GetFieldDouble( LPCTSTR delim, int fieldnum)
{
	CStringEx str=GetField(delim,fieldnum);
	return atof(str);
}



BOOL CStringEx::CopyToClipboard()
{
	// Si no podemos abrir el clipboard, mejor lo dejamos para mejor ocasión.
    if (!::OpenClipboard(NULL))
	{
		return FALSE;     
	}
	
	// lo limpiamos
	::EmptyClipboard();

	DWORD dwBytes = (1 + GetLength()) * sizeof(TCHAR);

	// Cogemos la memoria necesaria.
	HGLOBAL hMemoria = ::GlobalAlloc(GMEM_DDESHARE, dwBytes);
	if (hMemoria == NULL)         
	{
		::CloseClipboard();
		return FALSE;
	}  

	// La rellenamos...
	LPTSTR sz = (LPTSTR)::GlobalLock(hMemoria); 
		::lstrcpy(sz, (LPCTSTR)(*this)); 
	::GlobalUnlock(hMemoria);

	// Y lo mandamos al cuerno bien empaquetadito.
	HGLOBAL h = ::SetClipboardData(CF_TEXT, hMemoria);

	::CloseClipboard();

	return (h != NULL);
}


BOOL CStringEx::PasteFromClipboard()
{
    HGLOBAL hMemoria;
	LPTSTR sz;
	
	if (!::IsClipboardFormatAvailable(CF_TEXT))
	{
		return FALSE;
	}

	if (!::OpenClipboard(NULL)) 
	{
		return FALSE;  
	}

    hMemoria = ::GetClipboardData(CF_TEXT);
	
	if (hMemoria == NULL) 
	{
		::CloseClipboard();
		return FALSE;
	}

	sz = (LPTSTR)::GlobalLock(hMemoria);
	
	if (sz == NULL) 
	{
		::CloseClipboard();
		return FALSE;
	}

	// Pegamos de verdad.
	(*this) = sz;

	::GlobalUnlock(hMemoria);
	::CloseClipboard();

	return TRUE;
}




// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Encriptación y decriptación.
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


CStringEx CStringEx::GetEncryptedString()
{
	LPCTSTR sz = (LPCTSTR)(*this);
	CStringEx szReturn = "SH";

	for(int i=0; i<GetLength(); i++)
	{
		CString c;
		c.Format("%02X", (long)(char)(sz[i] + (char)i));
		szReturn += c;
	}

	szReturn.Reverse();

	return szReturn;
}



CStringEx CStringEx::GetDecryptedString()
{
	CStringEx s = (LPCTSTR)(*this);
	s.Reverse();

	CStringEx szReturn;

	if ((GetLength() % 2) != 0 || s.Left(2) != "SH")
	{
		return CStringEx("Error");
	}

	s = s.Right(s.GetLength() - 2);

	char sz[] = "X";
	for(int i=0; i<s.GetLength() / 2; i++)
	{
		CString sChar = s.Mid(i * 2, 2);
		LPSTR szDummy;
		sz[0] = (char)((char)(strtol(sChar, &szDummy, 16)) - (char)i);
		szReturn += sz;
	}

	return szReturn;
}



void CStringEx::Reverse()
{
	long l = GetLength();
	for(int i=0; i < (l / 2); i++)
	{
		char c = GetAt(i);
		SetAt(i, GetAt(l - i - 1));
		SetAt(l - i - 1, c);
	}
}



// Función que devuelve la misma cadena que la que
// hay pero sustituyendo los caracteres no imprimibles (<32)
// por cadenas hexadecimales.
CString CStringEx::GetPrintableString()
{
	CString str;

	for(int i=0; i<GetLength(); i++)
	{
		if (GetAt(i) < (TCHAR)32)
		{
			str += GetAt(i);
		}
		else
		{
			CString strChar;
			strChar.Format("[%02x]", (long)GetAt(i));
			str += strChar;
		}
	}

	return str;
}


// Función que rellena el string con un fichero de texto.
// Si devuelve true, la cadena devuelta es vacía.
// Si hay error, se devuelve la cadena con el error.
bool CStringEx::LoadFromFile(LPCTSTR szFile, CString* pstrErrorMessage)
{
	CFile f;
	CFileException fEx;

	if (pstrErrorMessage)
	{
		pstrErrorMessage->Empty();
	}

	if (!f.Open(szFile, CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary, &fEx))
	{
		if (pstrErrorMessage)
		{
			char szError[1000];
			fEx.GetErrorMessage(szError, 999, NULL);
			*pstrErrorMessage = szError;
		}
		return false;
	}

	DWORD lBytes = f.GetLength();
	LPSTR sz = new char[lBytes + 1];

	try
	{
		f.Read(sz, lBytes);
	}
	catch(CFileException* pEx)
	{
		if (pstrErrorMessage)
		{
			char szError[1000];
			fEx.GetErrorMessage(szError, 999, NULL);
			*pstrErrorMessage = szError;
		}
		pEx->Delete();
		delete[] sz;
		return false;
	}

	sz[lBytes] = 0; // Terminador

	*this = sz;	// Cargamos la cadena en esta cadena.
	delete[] sz; // Liberamos la memoria.

	f.Close();
	return true;
}

// Función que graba a fichero el contenido del string.
// Ver LoadFromFile para explicación del bool de salida.
bool CStringEx::SaveToFile(LPCTSTR szFile, CString* pstrErrorMessage)
{
	CFile f;
	CFileException fEx;

	if (pstrErrorMessage)
	{
		pstrErrorMessage->Empty();
	}

	if (!f.Open(szFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary, &fEx))
	{
		if (pstrErrorMessage)
		{
			char szError[1000];
			fEx.GetErrorMessage(szError, 999, NULL);
			*pstrErrorMessage = szError;
		}
		return false;
	}

	try
	{
		f.Write((LPCTSTR)(*this), GetLength());
	}
	catch(CFileException* pEx)
	{
		if (pstrErrorMessage)
		{
			char szError[1000];
			fEx.GetErrorMessage(szError, 999, NULL);
			*pstrErrorMessage = szError;
		}
		pEx->Delete();
		return false;
	}

	f.Close();
	return true;
}



bool CStringEx::StartBy(LPCTSTR szLeftPart)
{
	if ((int)strlen(szLeftPart) > GetLength())
	{
		return false;
	}

	if (Left(strlen(szLeftPart)) == szLeftPart)
	{
		return true;
	}

	return false;
}

bool CStringEx::StartByNoCase(LPCTSTR szLeftPart)
{
	if ((int)strlen(szLeftPart) > GetLength())
	{
		return false;
	}

	if (Left(strlen(szLeftPart)).CompareNoCase(szLeftPart) == 0)
	{
		return true;
	}

	return false;
}


