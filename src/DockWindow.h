#pragma once

#include "AppBar.h"


class CDockWindow :
	public CWindowImpl<CDockWindow>,
	public CAppBar<CDockWindow>
{
public:
	// Constants
	enum 
	{
		BAR_WND_STYLE    = WS_CLIPSIBLINGS | WS_OVERLAPPED,
		BAR_WND_EX_STYLE = WS_EX_PALETTEWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED
	};

	BEGIN_MSG_MAP(CDockWindow)
		// Messages
		MESSAGE_HANDLER(WM_CREATE, OnCreate)

		// Chaining
		CHAIN_MSG_MAP(CAppBar<CDockWindow>)
	END_MSG_MAP()

public:
	CDockWindow(void);
	~CDockWindow(void);

protected:
	// Message handlers
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);

private:
	// Types
	typedef CWinTraits<BAR_WND_STYLE, BAR_WND_EX_STYLE> CAppWinTraits;
};
