#pragma once

class CSputnikWorker
{
public:
	CSputnikWorker(void);
	~CSputnikWorker(void);

	void Run();
    static UINT Thread(LPVOID lParam);

	bool m_bStopping;
};
