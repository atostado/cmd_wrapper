#include "StdAfx.h"
#include "oldatagrammaker.h"



COLDatagramMaker::COLDatagramMaker(void)
{
}

COLDatagramMaker::~COLDatagramMaker(void)
{
}

void COLDatagramMaker::Clear()
{
	m_strDatagram = "<OLDatagram />";
}

void COLDatagramMaker::AddValue(LPCTSTR szValueName, LPCTSTR szValueText)
{
	CString strToken;
	strToken.Format(" %s=\"%s\" />", szValueName, szValueText);

	m_strDatagram.Replace(" />", strToken);
}

CString COLDatagramMaker::ExportToXML()
{
	return m_strDatagram;
}



