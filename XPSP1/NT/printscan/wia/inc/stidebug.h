/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    stidebug.h

Abstract:

    Environment independent assertion/logging routines

    Usage:

        ASSERT(exp)     Evaluates its argument.  If "exp" evals to
                        FALSE, then the app will terminate, naming
                        the file name and line number of the assertion
                        in the source.

        UIASSERT(exp)   Synonym for ASSERT.

        ASSERTSZ(exp,sz) As ASSERT, except will also print the message
                        "sz" with the assertion message should it fail.

        REQUIRE(exp)    As ASSERT, except that its expression is still
                        evaluated in retail versions.  (Other versions
                        of ASSERT disappear completely in retail builds.)

    The ASSERT macros expect a symbol _FILENAME_DEFINED_ONCE, and will
    use the value of that symbol as the filename if found; otherwise,
    they will emit a new copy of the filename, using the ANSI C __FILE__
    macro.  A client sourcefile may therefore define __FILENAME_DEFINED_ONCE
    in order to minimize the DGROUP footprint of a number of ASSERTs.

Author:

    Vlad Sadovsky   (vlads) 26-Jan-1997

Revision History:

    26-Jan-1997     VladS       created
    13-Apr-1999     VladS       make UNICODE aware

--*/


#ifndef _STIDEBUG_H_
#define _STIDEBUG_H_

#if defined(DEBUG)
static const char szFileName[] = __FILE__;
#define _FILENAME_DEFINED_ONCE szFileName
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

extern VOID UIAssertHelper( const CHAR* pszFileName, UINT nLine );
extern VOID UIAssertSzHelper( const TCHAR* pszMessage, const CHAR* pszFileName, UINT nLine );

extern VOID AssertHelper( const CHAR* pszFileName, UINT nLine );
extern VOID AssertSzHelper( const TCHAR* pszMessage, const CHAR* pszFileName, UINT nLine );

#if defined(DEBUG)

# ifdef USE_MESSAGEBOX_UI

#  define ASSERT(exp) \
    { if (!(exp)) UIAssertHelper(__FILE__, __LINE__); }

#  define ASSERTSZ(exp, sz) \
    { if (!(exp)) UIAssertSzHelper((sz), __FILE__, __LINE__); }

# else

#  define ASSERT(exp) \
    { if (!(exp)) AssertHelper(__FILE__, __LINE__); }

#  define ASSERTSZ(exp, sz) \
    { if (!(exp)) AssertSzHelper((sz), __FILE__, __LINE__); }

#define EVAL(exp)   \
    ((exp) || AssertHelper(__FILE__, __LINE__))

# endif // USE_MESSAGEBOX_UI

# define UIASSERT(exp)  ASSERT(exp)
# define REQUIRE(exp)   ASSERT(exp)

#else // !DEBUG

# define ASSERT(exp)        ;
# define EVAL(exp)          ;
# define UIASSERT(exp)      ;
# define ASSERTSZ(exp, sz)  ;
# define REQUIRE(exp)       { (exp); }

#endif // DEBUG



//
// Debug mask management.
//

// NOTE: You can #define your own DM_* values using bits in the HI BYTE

#define DM_TRACE    0x0001      // Trace messages
#define DM_WARNING  0x0002      // Warning
#define DM_ERROR    0x0004      // Error
#define DM_ASSERT   0x0008      // Assertions

#define DM_LOG_FILE 0x0100
#define DM_PREFIX   0x0200


#if !defined(StiDebugMsg)

//
// StiDebugMsg(mask, msg, args...) - Generate wsprintf-formatted msg using
// specified debug mask.  System debug mask governs whether message is output.
//

#define REGVAL_STR_DEBUGMASK_A    "DebugMask"
#define REGVAL_STR_DEBUGMASK_W    L"DebugMask"

void __cdecl StiDebugMsg(UINT mask, LPCTSTR psz, ...);

UINT WINAPI  StiSetDebugParameters(PTSTR pszName,PTSTR pszLogFile);
UINT WINAPI  StiSetDebugMask(UINT mask);
UINT WINAPI  StiGetDebugMask(void);

#endif

#ifdef  DEBUG

#define Break()  DebugBreak()
#define DPRINTF  StiDebugMsg

#else

#define Break()

//
// Nb: Following definition is needed to avoid compiler complaining
// about empty function name in expression. In retail builds using this macro
// will cause string parameters not appear in executable
//
#define DPRINTF     1?(void)0 : (void)

#endif

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#ifdef  DEBUG

class DBGTRACE
{
private:
    TCHAR   m_szMessage[200];

public:
    inline DBGTRACE(LPTSTR szMsg) {
        lstrcpy(m_szMessage,szMsg);
        DPRINTF(DM_TRACE,TEXT("ProcTraceEnter:%s At sec =%d "),
                m_szMessage,::GetTickCount()/1000);
    }

    inline ~DBGTRACE() {
        DPRINTF(DM_TRACE,TEXT("ProcTraceExit:%s At sec. =%d "),m_szMessage,::GetTickCount()/1000);
    }
};

#else

class DBGTRACE
{
public:
    inline DBGTRACE(LPTSTR szMsg) {
    }

    inline ~DBGTRACE() {
    }
};

#endif

#endif


//
// Performance monitoring
//

#if defined(__cplusplus)

#ifdef  DEBUG

class TICK_COUNTER
{
private:
    UINT    m_uiStartingCount;
    TCHAR   m_szMessage[200];

public:
    inline TICK_COUNTER(LPTSTR szMsg) {
        lstrcpy(m_szMessage,szMsg);
        m_uiStartingCount = ::GetTickCount();
    }

    inline ~TICK_COUNTER() {
        DPRINTF(DM_TRACE,TEXT("Elapsed time in (%s) is: %d ticks, %d seconds"),
                m_szMessage,
                ::GetTickCount() - m_uiStartingCount,
                (::GetTickCount() - m_uiStartingCount) / 1000
                );
    }

    inline ElapsedTicks(VOID) { return (::GetTickCount() - m_uiStartingCount);}

};

#else

class TICK_COUNTER
{
public:
    inline TICK_COUNTER(LPTSTR szMsg) {
    }

    inline ~TICK_COUNTER() {
    }
};

#endif
#endif


#endif // _STIDEBUG_H_
