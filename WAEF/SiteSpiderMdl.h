#pragma once

#include "NetSpider.h"

class SiteSpiderMdl
{
public:
	SiteSpiderMdl(void);
	~SiteSpiderMdl(void);

public:
	Error_code init(CWnd* powner);

	static void CALLBACK NotifyProc(string url);

private:
	static CWnd* _powner;

	NetSpider m_spider;


};

