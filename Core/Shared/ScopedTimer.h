#ifndef AFX_SCOPEDTIME_H__155791A3_22AC_4083_B933_F39E9EBDADEF__INCLUDED_
#define AFX_SCOPEDTIME_H__155791A3_22AC_4083_B933_F39E9EBDADEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////////////////////////

class CScopedTimer
{
public:
	virtual ~CScopedTimer();

protected:
	DWORD m_dwTickStart, m_dwIntermediateStart;
	CString m_sScope;

protected:
	CScopedTimer();
	CScopedTimer(LPCTSTR szScope, LPCTSTR szArg1 = _T(""), LPCTSTR szArg2 = _T(""), LPCTSTR szArg3 = _T(""));

	CString FormatStart() const;

protected:
	static CString FormatTimeElapsed(LPCTSTR szScope, DWORD& dwTickFrom);
	static CString FormatScope(LPCTSTR szScope, LPCTSTR szArg1, LPCTSTR szArg2, LPCTSTR szArg3);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class CScopedLogTimer : public CScopedTimer
{
public:
	CScopedLogTimer();
	CScopedLogTimer(LPCTSTR szScope, LPCTSTR szArg1 = _T(""), LPCTSTR szArg2 = _T(""), LPCTSTR szArg3 = _T(""));
	~CScopedLogTimer();

	void LogStart();
	void LogTimeElapsed(LPCTSTR szSubScope, LPCTSTR szArg1 = _T(""), LPCTSTR szArg2 = _T(""), LPCTSTR szArg3 = _T(""));
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class CScopedTraceTimer : public CScopedTimer
{
public:
	CScopedTraceTimer();
	CScopedTraceTimer(LPCTSTR szScope, LPCTSTR szArg1 = _T(""), LPCTSTR szArg2 = _T(""), LPCTSTR szArg3 = _T(""));
	~CScopedTraceTimer();

	void TraceStart();
	void TraceTimeElapsed(LPCTSTR szSubScope, LPCTSTR szArg1 = _T(""), LPCTSTR szArg2 = _T(""), LPCTSTR szArg3 = _T(""));
};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // AFX_SCOPEDTIME_H__155791A3_22AC_4083_B933_F39E9EBDADEF__INCLUDED_