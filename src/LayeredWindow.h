#pragma once


template <class T>
class ATL_NO_VTABLE CLayeredWindow
{
public:
	typedef CLayeredWindow<T> _Myt;
	typedef void (T::*fp_anim_complete)();

public:
	enum 
	{ 
		WND_OPAQUE			= 100,
		WND_TRANSPARENT		= 0,
		DEFAULT_DURATION	= 200,	// msc
		DEFAULT_COLOR_KEY	= RGB(0xfe, 0xfd, 0xfc)
	};

private:
	COLORREF			m_crKey;
	BYTE				m_nCurOpacity;		// Current opacity
	BYTE				m_nNewOpacity;		// Target opacity (for animation)
	HANDLE				m_hThreadAnim;
	HANDLE				m_hTerminateEvent;
	DWORD				m_nAnimDuration;
	fp_anim_complete	m_fpCallback;

public:
	BEGIN_MSG_MAP(CMouseEnterEvent)
		// Messages
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

public:
	CLayeredWindow(COLORREF aColorKey = DEFAULT_COLOR_KEY)
	{
		m_crKey			= aColorKey;
		m_nCurOpacity	= 255;
		m_nNewOpacity	= 255;
		m_hThreadAnim	= NULL;
		m_fpCallback	= NULL;

		// Generate unique event name
		GUID guid = {0};
		HRESULT hr = ::CoCreateGuid(&guid);
		ATLASSERT(SUCCEEDED(hr));
		TCHAR* pszUid = NULL;  
		::UuidToString(&guid, &pszUid);
		CString sEventName = pszUid;
		::RpcStringFree(&pszUid);  

		// Create non-signaled manual-reset event
		m_hTerminateEvent = ::CreateEvent(NULL,	TRUE, FALSE, sEventName);
	}

	virtual ~CLayeredWindow(void)
	{
		StopAnimation();
	}

	// Sets window opacity in percents [0, 100]
	void SetWindowOpacity(
		DWORD aOpacity, 
		DWORD aDuration = DEFAULT_DURATION, 
		fp_anim_complete aCallback = NULL)
	{
		T* pT = static_cast<T*>(this);
		_ASSERTE(pT);

		StopAnimation();

		// Get current opacity
		COLORREF crCurKey = 0;
		BYTE nCurAlpha = 0;
		DWORD nCurFlags = 0;
		BOOL rv = ::GetLayeredWindowAttributes(pT->m_hWnd, &crCurKey, &nCurAlpha, &nCurFlags);
		if (rv)
		{
			// Update opacity
			m_nCurOpacity = nCurAlpha; 
		}

		// Normalize opacity to [0, 255]
		m_nNewOpacity = aOpacity * 255 / 100;

		if (m_nCurOpacity != m_nNewOpacity)
		{
			m_nAnimDuration = aDuration;
			m_fpCallback = aCallback;
			m_hThreadAnim = ::CreateThread(NULL, 0, ThreadProcAnimate, pT, 0, NULL);
		}
	}

protected:
	// Message handlers
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		_ASSERTE(pT);

		BOOL rv = pT->ModifyStyleEx(0, WS_EX_LAYERED);
		_ASSERTE(rv);

		bHandled = FALSE;
		return 0;
	}

	void StopAnimation()
	{
		T* pT = static_cast<T*>(this);
		_ASSERTE(pT);

		// Stop animation
		::SetEvent(pT->m_hTerminateEvent);
		DWORD nWait = ::WaitForSingleObject(pT->m_hThreadAnim, 5000);
		if (WAIT_OBJECT_0 != nWait)
		{
			::TerminateThread(pT->m_hThreadAnim, 0);
		}
		::ResetEvent(pT->m_hTerminateEvent);
		pT->m_hThreadAnim = NULL;
	}

	static DWORD WINAPI ThreadProcAnimate(LPVOID lpParam)
	{
		T* pT = static_cast<T*>(lpParam);
		_ASSERTE(pT);

		// Animation frequency in milliseconds
		static enum { ANIM_FREQUENCY = 10 };

 		DWORD nAnimInterval = pT->m_nAnimDuration / ANIM_FREQUENCY;

		// Get animation step
		int nStep = (float)(pT->m_nNewOpacity - pT->m_nCurOpacity) / nAnimInterval;
		if (0 == nStep)
		{
			nStep = (pT->m_nNewOpacity > pT->m_nCurOpacity) ? 1 : -1;
		}

		while (WAIT_OBJECT_0 != ::WaitForSingleObject(pT->m_hTerminateEvent, 0) &&
			   pT->m_nCurOpacity != pT->m_nNewOpacity)
		{
			pT->m_nCurOpacity += nStep;

			// Restrict opacity value to [0, 255]
			if (pT->m_nCurOpacity < 0)
			{
				pT->m_nCurOpacity = 0;
			}
			else if (pT->m_nCurOpacity > 255)
			{
				pT->m_nCurOpacity = 255;
			}
			else if (abs(pT->m_nCurOpacity - pT->m_nNewOpacity) < abs(nStep))
			{
				pT->m_nCurOpacity = pT->m_nNewOpacity;
			}

			// Update window
			if (::IsWindow(pT->m_hWnd))
			{
				BOOL rv = ::SetLayeredWindowAttributes(
					pT->m_hWnd, 
					pT->m_crKey,		// Key - transparency
					pT->m_nCurOpacity,	// Alpha - translucency
					LWA_COLORKEY | LWA_ALPHA);

				// TODO: "SetLayeredWindowAttributes" doesn't play nice with "WM_ACTIVATE".
				//       This assertion is not terribly important, however we should figure
				//       out why it fails when "WM_ACTIVATE" is called at app start.
				//_ASSERTE(rv);
			}
			::Sleep(ANIM_FREQUENCY);
		}

		if (pT->m_nCurOpacity == pT->m_nNewOpacity && pT->m_fpCallback)
		{
			(pT->*(pT->m_fpCallback))();
		}

		/*
		// Set final window opacity
		if (::IsWindow(pT->m_hWnd))
		{
			BOOL rv = ::SetLayeredWindowAttributes(
				pT->m_hWnd, 
				pT->m_crKey,		// Key - transparency
				pT->m_nNewOpacity,	// Alpha - translucency
				LWA_COLORKEY | LWA_ALPHA);
		}
		*/

		return 1;
	}
};
