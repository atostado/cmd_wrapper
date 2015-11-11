#pragma once

class CStringEx : public CString
{

//CONSTRUCCION----------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
public:
	CStringEx() : CString( ){};
	CStringEx( const CString& stringSrc) : CString( stringSrc ){};
	CStringEx( const CStringEx& stringSrc) : CString( stringSrc ){};
	CStringEx( TCHAR ch, int nRepeat = 1 ) : CString( ch, nRepeat ){};
	CStringEx( LPCTSTR lpch, int nLength ) : CString( lpch, nLength ){};
	CStringEx( const unsigned char* psz ) : CString( psz ){};
	CStringEx( LPCSTR lpsz ) : CString( lpsz ){};
	CStringEx( LPCWSTR lpsz ) : CString( lpsz ){};

//OPERACIONES------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
public:
	BOOL PasteFromClipboard	();
	BOOL CopyToClipboard	();

	CStringEx	GetField		(LPCTSTR delim, int fieldnum);
	CStringEx	GetField		(TCHAR delim, int fieldnum);
	long		GetFieldLong	(TCHAR delim, int fieldnum);
	long		GetFieldLong	(LPCTSTR delim, int fieldnum);
	double		GetFieldDouble	(TCHAR delim, int fieldnum);
	double		GetFieldDouble	(LPCTSTR delim, int fieldnum);
	int			GetFieldCount	(LPCTSTR delim );
	int			GetFieldCount	(TCHAR delim );

	CStringEx GetDelimitedField	(LPCTSTR delimStart, LPCTSTR delimEnd, int fieldnum = 0);

	CStringEx GetEncryptedString();
	CStringEx GetDecryptedString();

	CString GetPrintableString	();

	void Reverse();

	bool LoadFromFile			(LPCTSTR szFile, CString* strErrorMessage = NULL);
	bool SaveToFile				(LPCTSTR szFile, CString* strErrorMessage = NULL);

	bool StartBy(LPCTSTR szLeftPart);
	bool StartByNoCase(LPCTSTR szLeftPart);
};
