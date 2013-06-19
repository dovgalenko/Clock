include macros.inc

@Start
@Uses gdi32, user32, comctl32, kernel32, winmm, shell32, advapi32, comdlg32

NumAlarms	Equ	25

CPUUSAGE	Equ	1

LANG_RUS	Equ	0
LANG_ENG	Equ	1

DESK_SHOW   Equ     0
DESK_HIDE   Equ     1

HWND_TRAY   Equ     2
HWND_PROG   Equ     3

; Ресурсы
RES_MCURSOR Equ     1002
RES_MICON   Equ     1003
RES_ABOUT   Equ     1004

; Идентификаторы пунктов меню
MNU_PREF    Equ     2000
MNU_EXIT    Equ     2001
MNU_ONTOP   Equ     2002
MNU_ONTASK  Equ     2003
MNU_ICON    Equ     2004
MNU_MOVE    Equ     2005
MNU_DATE    Equ     2006
MNU_START   Equ     2007
MNU_ABOUT   Equ     2008
MNU_SAVER   Equ     2009
MNU_DISP    Equ     2010
MNU_TRSH    Equ     2011
MNU_ICSH    Equ     2012
MNU_LANG    Equ     2013

WM_ICON     Equ     2045

MNU_EXTERN  Equ     2050

EXEC_DESK   Equ     0
EXEC_TIME   Equ     1
EXEC_ITEM   Equ     2

ACT_ADD     Equ     0
ACT_DEL     Equ     1

ID_HOUR     Equ     6001
ID_MINUTE   Equ     6002

COLOR_BACK  Equ     0
COLOR_FORE  Equ     1

WAVE_STD    Equ     0
WAVE_FILE   Equ     1
WAVE_MP3    Equ     2
WAVE_BEEP   Equ     3

        ReadCPU       PROTO

        PrepareText   PROTO
        MakeIcon      PROTO :DWORD
        SetType       PROTO :DWORD, :DWORD

        HookWindow    PROTO
        UnHookWindow  PROTO

        WriteRegistry PROTO
        ReadRegistry  PROTO

        MakeMenu      PROTO
        CloseMenu     PROTO

        InitPlugins   PROTO
        GetLang       PROTO
        SetLang       PROTO :DWORD
        GetString     PROTO :DWORD
        ClearMessages PROTO :DWORD, :DWORD
        PlugMessage   PROTO :DWORD, :DWORD, :DWORD
        FreePlugins   PROTO

        CopyMemory    PROTO :DWORD, :DWORD, :DWORD

        ExecItem      PROTO :DWORD, :DWORD

        UpdateMenus   PROTO

        TakeFile      PROTO :DWORD, :DWORD
        TakeFont      PROTO :DWORD, :DWORD
        TakeColor     PROTO :DWORD, :DWORD, :DWORD
        Alarm         PROTO :DWORD, :DWORD, :DWORD
        MakeRegistry  PROTO :DWORD

        WinMain       PROTO :DWORD, :DWORD, :DWORD, :DWORD
        WndProc       PROTO :DWORD, :DWORD, :DWORD, :DWORD

        Prefs         PROTO :DWORD, :DWORD, :DWORD, :DWORD
        PrefProc      PROTO :DWORD, :DWORD, :DWORD, :DWORD

        SaveAlarms    PROTO
        LoadAlarms    PROTO

@Includes Data, Lang, Plugins, Registry

include Alarms.inc
include Dialogs.inc

.code
Start       Proc
    invoke LoadString, hInstance, 1, ADDR dlgTitle, 256
    invoke FindWindowEx, NULL, NULL, ADDR wClass, ADDR dlgTitle
    .if eax != 0
      push   eax
      invoke GetString, 27
      invoke MessageBox, NULL, eax, ADDR dlgTitle, MB_YESNOCANCEL or MB_ICONQUESTION
      .if eax == IDYES
        pop    eax
        invoke PostMessage, eax, WM_DESTROY, 0, 0
        .while eax != 0
          invoke FindWindowEx, NULL, NULL, ADDR wClass, ADDR dlgTitle
        .endw

      .elseif eax == IDCANCEL
        pop    eax
        invoke PostMessage, eax, WM_DESTROY, 0, 0
        .while eax != 0
          invoke FindWindowEx, NULL, NULL, ADDR wClass, ADDR dlgTitle
        .endw
        invoke ExitProcess, 0

      .elseif eax == IDNO
        pop    eax
      .endif
    .endif

    invoke FindWindowEx, NULL, NULL, ADDR wClass, ADDR dlgTitle
    .if eax == NULL
      mov    hInstance, @Result(GetModuleHandle, NULL)
      mov    CommandLine, @Result(GetCommandLine)
      mov    hProcess, @Result(GetCurrentProcess)

      invoke SetPriorityClass, hProcess, REALTIME_PRIORITY_CLASS

      invoke ReadRegistry
      invoke LoadAlarms

      mov    hwndTray, @Result(FindWindow, ADDR dShell, NULL)
      .if !seeTray
        invoke SetType, HWND_TRAY, DESK_HIDE
      .endif

      invoke FindWindow, ADDR dDesk, NULL
      mov    hwndProg, @Result(FindWindowEx, eax, NULL, ADDR dProg, NULL)
      .if seeProg == FALSE
        invoke SetType, HWND_PROG, DESK_HIDE
      .endif

      invoke GetSystemMetrics, SM_CXSCREEN
      mov    dskWindow.right, eax
      invoke GetSystemMetrics, SM_CYSCREEN
      mov    dskWindow.bottom, eax

      invoke GetString, 10
      mov    ebx, eax
      invoke lstrlen, eax
      invoke CopyMemory, ADDR icn.szTip, ebx, eax

      mov    hBrush, @Result(CreateSolidBrush, bkColor)
      mov    icn.hIcon, @Result(LoadIcon, hInstance, RES_MICON)
      mov    hCursor, @Result(LoadCursor, hInstance, RES_MCURSOR)
      mov    hOldCurs, @Result(LoadCursor, NULL, IDC_ARROW)

      invoke InitCommonControls

      invoke PrepareText

      invoke WinMain, hInstance, NULL, CommandLine, SW_SHOWDEFAULT

      invoke FreePlugins

      invoke DeleteObject, icn.hIcon
      invoke DestroyCursor, hOldCurs
      invoke DestroyCursor, hCursor

      invoke SaveAlarms
      invoke WriteRegistry
    .endif

    .if !seeTray
      invoke SetType, HWND_TRAY, DESK_SHOW
    .endif
    .if !seeProg
      invoke SetType, HWND_PROG, DESK_SHOW
    .endif

    invoke ExitProcess, eax

Start      Endp

WinMain    Proc hInst     :DWORD,
                hPrevInst :DWORD,
                CmdLine   :DWORD,
                CmdShow   :DWORD

    LOCAL  wnd:WNDCLASSEX
    LOCAL  Msg:MSG
    LOCAL  wOption:DWORD

    mov    wnd.cbSize, SIZEOF WNDCLASSEX
    mov    wnd.style, CS_HREDRAW or CS_VREDRAW or CS_DBLCLKS
    mov    wnd.lpfnWndProc, Offset WndProc
    mov    wnd.cbClsExtra, NULL
    mov    wnd.cbWndExtra, NULL
    m2m    wnd.hInstance, hInstance
    m2m    wnd.hIcon, icn.hIcon
    mov    wnd.hCursor, @Result(LoadCursor, NULL, IDC_ARROW)
    m2m    wnd.hbrBackground, hBrush
;    mov    wnd.hbrBackground, NULL_BRUSH
    mov    wnd.lpszMenuName, NULL
    mov    wnd.lpszClassName, Offset wClass
    mov    wnd.hIconSm, 0
    invoke RegisterClassEx, ADDR wnd

    .if ShowInTasks
       mov    wOption, WS_EX_APPWINDOW
    .else
       mov    wOption, WS_EX_TOOLWINDOW
    .endif

    mov    clkWindow.left, 10
    mov    clkWindow.top, 10
    mov    clkWindow.right, 400
    mov    clkWindow.bottom, 400
    invoke CreateWindowEx, wOption, ADDR wClass, \
                           ADDR dlgTitle, WS_POPUP or DS_MODALFRAME, \
                           10, 10, 400, 500, NULL, NULL, \
                           hInstance, NULL
    mov    hWnd, eax

    .if eax
      invoke ShowWindow, hWnd, SW_SHOWNORMAL
      invoke UpdateWindow, hWnd

      .while TRUE
        invoke GetMessage, ADDR Msg, NULL, 0, 0
        .break .if (!eax)
        invoke TranslateMessage, ADDR Msg
        invoke DispatchMessage, ADDR Msg
      .endw
    .endif

    invoke DeleteObject, hFNT
    ret
WinMain    Endp

.data?
sfLen         Dd ?
szTemp        Db 50 Dup(?)
szFormat      Db 50 Dup(?)

.data
sTime         Db ' HH:mm', 0
sSec          Db ':ss ', 0
sDate         Db ' ddd, d MMMM ', 0
sFDate        Db ' dddd, d MMMM yyyy', 0
sRes          Db ' %lu%%, %lu%%, %lu%%', 0
sMem          Db ' %luKb (%lu%%)', 0
sCPU          Db ' CPU:%li%%, Threads:%li%', 0

.code
; Процедура диалогового окна
WndProc proc hWin   :DWORD,
             uMsg   :DWORD,
             wParam :DWORD,
             lParam :DWORD

        LOCAL wRect    :RECT
        LOCAL sz       :_SIZE
        LOCAL cur      :POINT
        LOCAL PointSize:Dword
        LOCAL dDC      :DWORD
        LOCAL dWin     :DWORD

        LOCAL clk      :POINTS

     .if uMsg == WM_TIMER
        lea    ebx, sTime
        .if ShowBlink && ShowSec
          add    ebx, 7
        .else
          add    ebx, 3
        .endif

        mov    al, Byte Ptr [ebx]
        .if ShowBlink
          .if al == ':'
            mov    Byte Ptr [ebx], ' '
          .else
            mov    Byte Ptr [ebx], ':'
          .endif
        .elseif al == ' '
            mov    Byte Ptr [ebx], ':'
        .endif

        invoke ClearMessages, hWin, WM_TIMER
        invoke PlugMessage, TCC_PLUG_IDLE, 0, 0
        call   Repaint

     .elseif uMsg == WM_FONTCHANGE
        invoke DeleteObject, hFNT
        mov    hFNT, @Result(CreateFontIndirect, ADDR logFnt)

     .elseif uMsg == WM_LBUTTONDOWN
        mov    eax, lParam
        invoke PostMessage, hWin, WM_NCLBUTTONDOWN, HTCAPTION, eax
        invoke SetCursor, hCursor

     .elseif uMsg == WM_LBUTTONUP
        invoke SetCursor, hOldCurs

     .elseif (uMsg == WM_COMMAND) || ((uMsg == WM_ICON) && (lParam == WM_LBUTTONDBLCLK))
        .if wParam == MNU_EXIT
           invoke GetString, 28
           invoke MessageBox, hWin, eax, ADDR dlgTitle, MB_YESNO or MB_ICONQUESTION
           .if eax == IDYES
               invoke MakeIcon, ACT_DEL
               invoke KillTimer, hWin, hTimer
               invoke PostQuitMessage, hWin
           .endif
        .elseif ((wParam == MNU_PREF) || ((uMsg == WM_ICON) && (lParam == WM_LBUTTONDBLCLK)) || (wParam == MNU_ABOUT)) && (hwndPref == NULL)
            .if wParam == MNU_ABOUT
              mov  showAbout, TRUE
            .endif
            invoke EnableWindow, hWin, FALSE
            invoke DialogBoxParam, hInstance, 100, 0, ADDR PrefProc, 0
            invoke EnableWindow, hWin, TRUE
            .if wParam == MNU_ABOUT
              mov  showAbout, FALSE
            .endif

        .elseif wParam == MNU_ONTOP
           .if AlwaysOnTop
             mov    AlwaysOnTop, FALSE
             invoke SetWindowPos, hWin, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE or SWP_NOSIZE
           .else
             mov    AlwaysOnTop, TRUE
             invoke SetWindowPos, hWin, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE or SWP_NOSIZE
           .endif

        .elseif wParam == MNU_TRSH
           .if !seeTray
             invoke SetType, HWND_TRAY, DESK_SHOW
           .else
             invoke SetType, HWND_TRAY, DESK_HIDE
           .endif

        .elseif wParam == MNU_ICSH
           .if !seeProg
             invoke SetType, HWND_PROG, DESK_SHOW
           .else
             invoke SetType, HWND_PROG, DESK_HIDE
           .endif

        .elseif wParam == MNU_ONTASK
           .if ShowInTasks
             mov    ShowInTasks, FALSE
             invoke SetWindowLong, hWin, GWL_EXSTYLE, WS_EX_TOOLWINDOW
           .else
             mov    ShowInTasks, TRUE
             invoke SetWindowLong, hWin, GWL_EXSTYLE, WS_EX_APPWINDOW
           .endif

        .elseif wParam == MNU_ICON
           .if ShowInTray
             mov    ShowInTray, FALSE
             invoke MakeIcon, ACT_DEL
           .else
             mov    ShowInTray, TRUE
             invoke MakeIcon, ACT_ADD
           .endif

        .elseif wParam == MNU_DATE
           invoke ExecItem, EXEC_TIME, SW_SHOW

        .elseif wParam == MNU_LANG
           invoke GetLang
           .if eax == LANG_ENG
             invoke SetLang, LANG_RUS
           .else
             invoke SetLang, LANG_ENG
           .endif

        .elseif wParam == MNU_START
           .if AutoBoot == TRUE
             mov    AutoBoot, FALSE
             invoke MakeRegistry, ACT_DEL
           .else
             mov    AutoBoot, TRUE
             invoke MakeRegistry, ACT_ADD
           .endif

        .elseif wParam == MNU_SAVER
           invoke GetDesktopWindow
           invoke PostMessage, eax, WM_SYSCOMMAND, SC_SCREENSAVE, 0

        .elseif wParam == MNU_DISP
           invoke ExecItem, EXEC_DESK, SW_SHOW

        .elseif wParam >= MNU_EXTERN
           invoke PlugMessage, TCC_MENU_EXEC, wParam, NULL

        .endif

     .elseif ((uMsg == WM_RBUTTONDOWN) || ((uMsg == WM_ICON) && (lParam == WM_RBUTTONDOWN))) && (hwndPref == NULL)
        .if (Move == FALSE)
          invoke MakeMenu
          .if uMsg == WM_ICON
            invoke EnableMenuItem, hMenu, MNU_MOVE, MF_BYCOMMAND or MF_DISABLED or MF_GRAYED
          .endif
          invoke GetCursorPos, ADDR cur
          invoke TrackPopupMenu, hMenu, TPM_LEFTALIGN or TPM_LEFTBUTTON, cur.x, cur.y, 0, hWin, NULL
          invoke CloseMenu
        .endif

     .elseif uMsg == WM_CREATE
        invoke InitPlugins
        m2m    hmWnd, hWin
        mov    hFNT, @Result(CreateFontIndirect, ADDR logFnt)
        .if ShowInTray == TRUE
          invoke MakeIcon, ACT_ADD
        .endif

        .if AlwaysOnTop == TRUE
          mov    eax, HWND_TOPMOST
        .else
          mov    eax, HWND_NOTOPMOST
        .endif

        invoke SetWindowPos, hWin, eax, clkWindow.left, clkWindow.top, \
        clkWindow.right, clkWindow.bottom, SWP_NOACTIVATE

        mov    hTimer, @Result(SetTimer, hWin, 1, 1000, NULL)

     .elseif uMsg == WM_PAINT
        invoke GetWindowRect, hWin, ADDR clkWindow
        invoke PlugMessage, TCC_PLUG_PAINT, hWin, 0

        .if eax != TCC_PLUG_PAINT
          invoke BeginPaint, hWin, ADDR strucPaint
          invoke SelectObject, strucPaint.hdc, hFNT
          invoke SetBkMode, strucPaint.hdc, TRANSPARENT
          invoke SetTextColor, strucPaint.hdc, fntColor
          invoke DrawTextEx, strucPaint.hdc, ADDR mTime, -1, ADDR fntWindow, DT_CENTER or DT_VCENTER or DT_CALCRECT or DT_SINGLELINE, NULL
          .if eax != NULL
            invoke SetWindowPos, hWin, NULL, clkWindow.left, clkWindow.top, fntWindow.right, fntWindow.bottom, SWP_NOZORDER or SWP_NOACTIVATE
          .endif

          invoke DrawTextEx, strucPaint.hdc, ADDR mTime, -1, ADDR fntWindow, DT_CENTER or DT_VCENTER or DT_SINGLELINE, NULL
          invoke EndPaint, hWin, ADDR strucPaint
        .endif

     .elseif (uMsg == WM_DESTROY) || (uMsg == WM_QUERYENDSESSION) || (uMsg == WM_ENDSESSION)
        invoke GetWindowRect, hWin, ADDR clkWindow
        invoke WriteRegistry
        call   Ending
     .endif

@GoOut:
     xor    eax, eax
     invoke DefWindowProc, hWin, uMsg, wParam, lParam
     ret

Ending:
     invoke KillTimer, hWin, hTimer
     invoke PostQuitMessage, hWin
     jmp    @GoOut

Repaint:
   invoke PrepareText
   invoke InvalidateRect, hWin, ADDR fntWindow, TRUE
   ret
WndProc Endp

ExecItem        Proc    hType    :DWORD,
                        hCommand :DWORD

    .if hType == EXEC_DESK || hType == EXEC_TIME
      invoke GetWindowsDirectory, ADDR tmpBuf, 256
      invoke lstrcat, ADDR tmpBuf, ADDR tmpExec
      .if hType == EXEC_DESK
        invoke lstrcat, ADDR tmpBuf, ADDR tmpDesk
      .elseif
        invoke lstrcat, ADDR tmpBuf, ADDR tmpTime
      .endif
      invoke WinExec, ADDR tmpBuf, hCommand
    .endif
    ret
ExecItem        Endp

UpdateMenus Proc
    LOCAL  scrnSave :DWORD

    invoke SystemParametersInfo, SPI_GETSCREENSAVEACTIVE, 0, ADDR scrnSave, 0
    .if hOption != NULL
      .if AlwaysOnTop == TRUE
         invoke CheckMenuItem, hOption, MNU_ONTOP, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, hOption, MNU_ONTOP, MF_BYCOMMAND or MF_UNCHECKED
      .endif

      .if scrnSave == FALSE
         invoke EnableMenuItem, hMenu, MNU_SAVER, MF_BYCOMMAND or MF_DISABLED or MF_GRAYED
      .else
         invoke EnableMenuItem, hMenu, MNU_SAVER, MF_BYCOMMAND or MF_ENABLED
      .endif

      .if AutoBoot == TRUE
         invoke CheckMenuItem, hMenu, MNU_START, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, hMenu, MNU_START, MF_BYCOMMAND or MF_UNCHECKED
      .endif

      .if ShowInTasks == TRUE
         invoke CheckMenuItem, hOption, MNU_ONTASK, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, hOption, MNU_ONTASK, MF_BYCOMMAND or MF_UNCHECKED
      .endif

      .if ShowInTray == TRUE
         invoke CheckMenuItem, hOption, MNU_ICON, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, hOption, MNU_ICON, MF_BYCOMMAND or MF_UNCHECKED
      .endif

      .if seeTray == FALSE
         invoke CheckMenuItem, mnuDisplay, MNU_TRSH, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, mnuDisplay, MNU_TRSH, MF_BYCOMMAND or MF_UNCHECKED
      .endif

      .if seeProg == FALSE
         invoke CheckMenuItem, mnuDisplay, MNU_ICSH, MF_BYCOMMAND or MF_CHECKED
      .else
         invoke CheckMenuItem, mnuDisplay, MNU_ICSH, MF_BYCOMMAND or MF_UNCHECKED
      .endif
    .endif
    ret
UpdateMenus Endp

MakeMenu        Proc
    invoke CreatePopupMenu
    mov    mnuDisplay, eax
    invoke GetString, 21
    invoke AppendMenu, mnuDisplay, MF_STRING, MNU_DISP, eax
    invoke GetString, 19
    invoke AppendMenu, mnuDisplay, MF_STRING, MNU_TRSH, eax
    invoke GetString, 20
    invoke AppendMenu, mnuDisplay, MF_STRING, MNU_ICSH, eax
    invoke AppendMenu, mnuDisplay, MF_SEPARATOR, NULL, NULL
    invoke GetString, 17
    invoke AppendMenu, mnuDisplay, MF_STRING, MNU_SAVER, eax

    invoke CreatePopupMenu
    mov    hOption, eax

    invoke GetString, 15
    invoke AppendMenu, hOption, MF_STRING, MNU_ONTASK, eax
    invoke GetString, 16
    invoke AppendMenu, hOption, MF_STRING, MNU_ICON, eax
    invoke AppendMenu, hOption, MF_SEPARATOR, NULL, NULL
    invoke GetString, 14
    invoke AppendMenu, hOption, MF_STRING, MNU_ONTOP, eax

    invoke CreatePopupMenu
    mov    hMenu, eax

    invoke GetString, 22
    invoke AppendMenu, hMenu, MF_STRING, MNU_ABOUT, eax
    invoke AppendMenu, hMenu, MF_SEPARATOR, NULL, NULL

    invoke GetString, 10
    invoke AppendMenu, hMenu, MF_STRING, MNU_PREF, eax
    invoke GetString, 11
    invoke AppendMenu, hMenu, MF_STRING, MNU_DATE, eax
    invoke GetString, 13
    invoke AppendMenu, hMenu, MF_POPUP or MF_STRING, hOption, eax
    invoke GetString, 18
    invoke AppendMenu, hMenu, MF_POPUP or MF_STRING, mnuDisplay, eax

    invoke PlugMessage, TCC_MENU_MAKE, hMenu, NULL

    invoke GetString, 24
    invoke AppendMenu, hMenu, MF_STRING, MNU_LANG, eax
    invoke GetString, 12
    invoke AppendMenu, hMenu, MF_STRING, MNU_START, eax
    invoke AppendMenu, hMenu, MF_SEPARATOR, NULL, NULL
    invoke GetString, 23
    invoke AppendMenu, hMenu, MF_STRING, MNU_EXIT, eax

    invoke UpdateMenus
    ret
MakeMenu        Endp

CloseMenu       Proc
    invoke PlugMessage, TCC_MENU_DEL, hMenu, NULL

    invoke DestroyMenu, hMenu
    invoke DestroyMenu, hOption
    invoke DestroyMenu, mnuDisplay
    ret
CloseMenu       Endp

ClearMessages PROC Uses Eax hWin:DWORD, Message:DWORD
    LOCAL   MyMSG :MSG

    mov eax,TRUE
    .while eax==TRUE
      invoke PeekMessage, addr MyMSG, hWin, Message, Message, PM_REMOVE
    .endw
    ret
ClearMessages ENDP

PrepareText   PROC
    LOCAL stProc   :DWORD
    LOCAL stHand   :DWORD
    LOCAL stMem    :MEMORYSTATUS

    LOCAL sMemo[50]:BYTE
    LOCAL slCPU[50]:BYTE

    invoke GetLocalTime, ADDR strucTime

    invoke lstrcpy, ADDR szFormat, ADDR sTime
    .if ShowSec == TRUE
      invoke lstrcat, ADDR szFormat, ADDR sSec
    .else
      invoke lstrcat, ADDR szFormat, ADDR szSpace
    .endif

    .if ShowDate
      .if dateShort
        invoke lstrcat, ADDR szFormat, ADDR sDate
      .else
        invoke lstrcat, ADDR szFormat, ADDR sFDate
      .endif
    .endif

    invoke GetTimeFormat, NULL, TIME_FORCE24HOURFORMAT, ADDR strucTime,
                          ADDR szFormat, ADDR szTemp, 50

    .if ShowDate == TRUE
      invoke GetDateFormat, NULL, NULL, ADDR strucTime,
                            ADDR szTemp, ADDR szFormat, 50
    .else
      invoke lstrcpy, ADDR szFormat, ADDR szTemp
    .endif

    .if ShowRes == TRUE
      invoke lstrcpy, ADDR mTime, ADDR szFormat
    .else
      invoke lstrcpy, ADDR mTime, ADDR szFormat
    .endif

    .if ShowMem == TRUE
      mov    stMem.dwLength, SIZEOF MEMORYSTATUS
      invoke GlobalMemoryStatus, ADDR stMem
      pushad
      xor    edx, edx
      mov    eax, stMem.dwAvailPhys
      mov    ecx, 1024
      div    ecx
      invoke wsprintf, ADDR sMemo, ADDR sMem, eax, stMem.dwMemoryLoad
      invoke lstrcat, ADDR mTime, ADDR sMemo
      popad
    .else
      .if ShowDate == FALSE && ShowRes == FALSE
        invoke lstrcat, ADDR mTime, ADDR szSpace
      .endif
    .endif

    IFDEF CPUUSAGE
      invoke ReadCPU
      invoke wsprintf, ADDR slCPU, ADDR sCPU, eax, ebx
      invoke lstrcat, ADDR mTime, ADDR slCPU
    ENDIF

;    .if IsDebug == TRUE
;      invoke wsprintf, ADDR sMemo, ADDR icnForm, icnAdd, icnDel
;      invoke lstrcat, ADDR mTime, ADDR sMemo
;    .endif

    invoke lstrlen, ADDR mTime
    mov    sfLen, eax
    inc    sfLen
    ret
PrepareText   ENDP

SetType       PROC hwnd:DWORD, act:DWORD

    LOCAL  hwndOur :DWORD

    .if hwnd == HWND_TRAY
      m2m  hwndOur, hwndTray
      .if act == DESK_HIDE
        mov    seeTray, FALSE
      .ELSE
        mov    seeTray, TRUE
      .endif
    .elseif hwnd == HWND_PROG
      m2m  hwndOur, hwndProg
      .if act == DESK_HIDE
        mov    seeProg, FALSE
      .ELSE
        mov    seeProg, TRUE
      .endif
    .endif

    .if act == DESK_HIDE
      invoke SetWindowPos, hwndOur, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER or \
                           SWP_NOMOVE or SWP_NOSIZE or SWP_HIDEWINDOW
    .else
      invoke SetWindowPos, hwndOur, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER or \
                           SWP_NOMOVE or SWP_NOSIZE or SWP_SHOWWINDOW
    .endif
    ret
SetType       ENDP

CopyMemory Proc lpDest :DWORD,
                lpSour :DWORD,
                cBytes :DWORD

    pushad
    mov    ecx, cBytes
    mov    esi, lpSour
    mov    edi, lpDest
    rep    movsb
    popad

    ret
CopyMemory Endp

MakeIcon   Proc  Action :DWORD

    mov    eax, icnDel
    .if Action == ACT_DEL
      .if icnAdd > eax
        mov    icn.cbSize, sizeof NOTIFYICONDATA
        mov    eax, hmWnd
        mov    icn.hwnd, eax
        invoke Shell_NotifyIcon, NIM_DELETE, ADDR icn
        inc icnDel
      .endif
    .else
      .if icnAdd <= eax
        mov    icn.cbSize, sizeof NOTIFYICONDATA
        mov    eax, hmWnd
        mov    icn.hwnd, eax
        invoke Shell_NotifyIcon, NIM_ADD, ADDR icn
        inc icnAdd
      .endif
    .endif
    ret
MakeIcon   Endp

            end Start
