#include "stdafx.h"
#include ".\logdatagramsender.h"
#include "Afxsock.h"
#include "OLDatagramMaker.h"





CLOGDatagramSender::CLOGDatagramSender(void)
{
	m_pSocket = new CAsyncSocket;

	m_bReady = m_pSocket->Socket(
				SOCK_DGRAM,
				FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECT,
				0,
				PF_INET);

	if (!m_bReady)
	{
		ASSERT(false);
		return;
	}

	m_bReady = m_pSocket->Connect("224.168.100.2", 65432);
}

CLOGDatagramSender::~CLOGDatagramSender(void)
{
	delete m_pSocket;
}


bool CLOGDatagramSender::SendDatagram(COLDatagramMaker* pDatagram)
{
	CString strText = pDatagram->ExportToXML();

	return SendRawText(strText);
}

bool CLOGDatagramSender::SendRawText(LPCTSTR szTextToSend)
{
	if (!m_bReady)
	{
		return false;
	}

	long l = strlen(szTextToSend);
	return m_pSocket->Send(szTextToSend, l, 0);
}




