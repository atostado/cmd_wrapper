#pragma once


class COLDatagramMaker;
class CAsyncSocket;

class CLOGDatagramSender
{
public:
	CLOGDatagramSender(void);
	~CLOGDatagramSender(void);

protected:
	CAsyncSocket* m_pSocket;
	BOOL m_bReady;

public:
	bool SendDatagram(COLDatagramMaker* pDatagram);
	bool SendRawText(LPCTSTR szTextToSend);

};




