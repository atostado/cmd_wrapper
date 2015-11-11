#pragma once

CString SYSGetRunDirectory();
CString SYSGetExeDirectory();
CString SYSGetExeName();
long GetInt(const char* szGroup, const char* szValue, long lDefValue, bool /*bSave*/);
void SetInt(const char* szGroup, const char* szValue, long lNewValue);
CString GetAlfa(const char* szGroup, const char* szValue, const char* szDefValue, bool /*bSave*/);
void SetAlfa(const char* szGroup, const char* szValue, const char* szAlfa);
CString SYSBuildOneUIntList(const CUIntArray* pUIA, LPCTSTR szSeparator);
CString SYSBuildOneStringList(const CStringArray* pSA, LPCTSTR szSeparator);
CString _F_(LPCTSTR szFormatString, ...);
CString _N_(int i);
CString SYSPathFile(LPCTSTR szPath, LPCTSTR szFile);

