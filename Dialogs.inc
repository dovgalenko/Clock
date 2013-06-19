TYPE_WAV  Equ  10
TYPE_EXE  Equ  11

          LangDialog PROTO :DWORD, :DWORD

.code
TakeColor  Proc aInstance :DWORD,
                hwndOwner :DWORD,
                initColor :DWORD
comment~
            LOCAL   col:CHOOSECOLOR

     mov    col.lStructSize, sizeof CHOOSECOLOR
     mov    eax, aInstance
     mov    col.hInstance, eax
     mov    eax, hwndOwner
     mov    col.hwndOwner, eax
     mov    eax, initColor
     mov    col.rgbResult, eax
     lea    eax, custColors
     mov    col.lpCustColors, eax
     mov    col.Flags, CC_FULLOPEN or CC_RGBINIT

     invoke ChooseColor, ADDR col
     mov    ebx, col.rgbResult
~
     ret
TakeColor  Endp

TakeFont   Proc hwndOwner :DWORD,
                lpFntStr  :DWORD

           LOCAL fnt: CHOOSEFONT
comment~
     mov    fnt.lStructSize, sizeof CHOOSEFONT
     mov    eax, hwndOwner
     mov    fnt.hWndOwner, eax
     mov    fnt.Flags, CF_BOTH or CF_INITTOLOGFONTSTRUCT
     mov    fnt.nFontType, REGULAR_FONTTYPE
     m2m    fnt.lpLogFont, lpFntStr
     invoke ChooseFont, ADDR fnt
~
     ret
TakeFont   Endp

TakeFile   Proc hWin     :DWORD, FType:DWORD
comment~
    mov    f.lStructSize, sizeof OPENFILENAME
    mov    eax, hWin
    mov    f.hWndOwner, eax
    mov    eax, hInstance
    mov    f.hInstance, eax
    .if Language == LANG_ENG
      .if FType == TYPE_EXE
        lea    eax, FiltersEng1
      .else
        lea    eax, FiltersEng
      .endif
    .else
      .if FType == TYPE_EXE
        lea    eax, Filters1
      .else
        lea    eax, Filters
      .endif
    .endif
    mov    f.lpstrFilter, eax
    mov    f.lpstrCustomFilter, NULL
    mov    f.nFilterIndex, 0
    lea    eax, iniFile
    mov    f.lpstrFile, eax
    mov    f.nMaxFile, 512
    mov    f.lpstrFileTitle, NULL
    lea    eax, initDir
    mov    f.lpstrInitialDir, eax
    lea    eax, dlgTitle
    mov    f.lpstrTitle, eax
    mov    f.Flags, OFN_FILEMUSTEXIST or OFN_LONGNAMES or OFN_NOCHANGEDIR
    mov    f.nFileOffset, 0
    invoke GetOpenFileName, ADDR f
~
    ret
TakeFile   Endp

.data
canChange  Db   TRUE
Selected   Dd   ?

vOper      Db   'open', 0
getStat    Db   'mailto:', 20 Dup (?)

.data?
email      Dd   ?
nBrush     Dd   ?
nFont      Dd   ?

hOk        Dd   ?
hApply     Dd   ?
hCancel    Dd   ?

hwndPrefs  HWND ?
hwndWnd    HWND ?

.code
           PPrepare    PROTO :DWORD
           PAlarm      PROTO :DWORD, :DWORD, :DWORD, :DWORD
           PAbout      PROTO :DWORD, :DWORD, :DWORD, :DWORD
           PPref       PROTO :DWORD, :DWORD, :DWORD, :DWORD

.code
PrefProc   Proc hWin   :DWORD,
                uMsg   :DWORD,
                wParam :DWORD,
                lParam :DWORD

          LOCAL Item   :TC_ITEM
          LOCAL curWnd :HWND

    .if (uMsg == WM_CLOSE) || ((uMsg == WM_COMMAND) && (wParam == 102))
      mov    hwndPref, NULL
      invoke DestroyWindow, hDlg
      invoke EndDialog, hWin, 0

      .if tmpMod == TRUE
        call   Modify
      .endif

     .elseif uMsg == WM_LBUTTONDOWN
        mov    eax, lParam
        invoke PostMessage, hWin, WM_NCLBUTTONDOWN, HTCAPTION, eax
        invoke SetCursor, hCursor

     .elseif uMsg == WM_LBUTTONUP
        invoke SetCursor, hOldCurs

    .elseif uMsg == WM_INITDIALOG
      m2m    hwndWnd, hWin

      invoke CopyMemory, ADDR tmpFnt, ADDR logFnt, SIZEOF LOGFONT
      invoke CopyMemory, ADDR tmpTray, ADDR ShowInTray, 16

      invoke SetForegroundWindow, hWin
      m2m    hwndPref, hWin

      invoke GetDlgItem, hWin, 101
      mov    hTabCtrl, eax

      invoke GetWindowRect, hTabCtrl, ADDR pRect
      add    pRect.top, 8

      mov    Item.imask, TCIF_TEXT
      mov    Item.iImage, -1
      invoke GetString, 25
      mov    Item.pszText, eax
      invoke SendMessage, hTabCtrl, TCM_INSERTITEM, 0, ADDR Item
      invoke GetString, 26
      mov    Item.pszText, eax
      invoke SendMessage, hTabCtrl, TCM_INSERTITEM, 1, ADDR Item
      invoke GetString, 22
      mov    Item.pszText, eax
      invoke SendMessage, hTabCtrl, TCM_INSERTITEM, 2, ADDR Item

      invoke GetDlgItem, hWin, 102
      mov    hOk, eax
      invoke GetDlgItem, hWin, 103
      mov    hApply, eax
      invoke GetDlgItem, hWin, 104
      mov    hCancel, eax

      invoke GetString, 10
      invoke SendMessage, hwndWnd, WM_SETTEXT, 0, eax

      .if Language == LANG_ENG
        invoke LangDialog, hWin, OFFSET d1eng
      .else
        invoke LangDialog, hWin, OFFSET d1rus
      .endif

      invoke EnableWindow, hCancel, FALSE
      invoke EnableWindow, hApply, FALSE

      mov    eax, dskWindow.right
      mov    ebx, dskWindow.bottom
      shr    eax, 1
      sub    eax, 183

      shr    ebx, 1
      sub    ebx, 157
      invoke SetWindowPos, hWin, HWND_TOP, eax, ebx, 0, 0, SWP_NOSIZE or SWP_NOZORDER

      invoke CreateDialogParam, hInstance, 110, hTabCtrl, ADDR Prefs, 0
      mov    hDlg, eax

      .if showAbout == TRUE
        invoke DestroyWindow, hDlg
        invoke SendMessage, hTabCtrl, TCM_SETCURSEL, 2, 0
        invoke CreateDialogParam, hInstance, 200, hTabCtrl, ADDR PAbout, 0
        mov    hDlg, eax
      .endif

    .elseif uMsg == WM_COMMAND
      .if (wParam == 103) && (tmpMod == TRUE)
        invoke CopyMemory, ADDR logFnt, ADDR tmpFnt, SIZEOF LOGFONT
        invoke CopyMemory, ADDR ShowInTray, ADDR tmpTray, 16
        call   Modify

      .elseif (wParam == 104) && (tmpMod == TRUE)
        call   Cancel
        invoke SendMessage, hwndPrefs, WM_PAINT, 0, 0
      .endif

    .elseif uMsg == WM_NOTIFY
      mov    ebx, lParam
      mov    eax, [ebx + 8]
      .if (eax == TCN_SELCHANGE) && canChange
        invoke DestroyWindow, hDlg

        invoke SendMessage, hTabCtrl, TCM_GETCURSEL, 0, 0
        mov    Selected, eax
        .if eax == 0
          invoke CreateDialogParam, hInstance, 110, hTabCtrl, ADDR Prefs, 0
        .elseif eax == 1
          invoke CreateDialogParam, hInstance, 150, hTabCtrl, ADDR PAlarm, 0
        .elseif eax == 2
          invoke CreateDialogParam, hInstance, 200, hTabCtrl, ADDR PAbout, 0
        .endif
        mov    hDlg, eax

      .elseif (eax == TCN_SELCHANGE) && !canChange
          invoke SendMessage, hTabCtrl, TCM_SETCURSEL, Selected, 0

      .elseif eax == TCN_SELCHANGING

      .endif
    .endif
    ret

Modify:
    invoke CopyMemory, ADDR logFnt, ADDR tmpFnt, SIZEOF LOGFONT
    invoke CopyMemory, ADDR ShowInTray, ADDR tmpTray, 16

    invoke GetForegroundWindow
    mov    curWnd, eax

    ; Меняем цвет бэка
    invoke DeleteObject, hBrush
    invoke CreateSolidBrush, bkColor
    mov    hBrush, eax
    invoke SetClassLong, hWnd, GCL_HBRBACKGROUND, hBrush

    invoke SendMessage, HWND_BROADCAST, WM_FONTCHANGE, 0, 0

    .if ShowInTray == TRUE
      invoke MakeIcon, ACT_ADD
    .else
      invoke MakeIcon, ACT_DEL
    .endif

    .if AlwaysOnTop == TRUE
      invoke SetWindowPos, hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE or SWP_NOSIZE
    .else
      invoke SetWindowPos, hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE or SWP_NOSIZE
    .endif

    .if ShowInTasks == FALSE
      invoke SetWindowLong, hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW
    .else
      invoke SetWindowLong, hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW
    .endif

    invoke EnableWindow, hApply, FALSE
    invoke EnableWindow, hCancel, FALSE
    invoke PPrepare, hwndPrefs
    mov    tmpMod, FALSE

    invoke SetForegroundWindow, curWnd
    ret

Cancel:
    invoke CopyMemory, ADDR tmpFnt, ADDR logFnt, SIZEOF LOGFONT
    invoke CopyMemory, ADDR tmpTray, ADDR ShowInTray, 16

    invoke EnableWindow, hApply, FALSE
    invoke EnableWindow, hCancel, FALSE
    invoke PPrepare, hwndPrefs
    mov    tmpMod, FALSE
    ret
PrefProc   Endp

.data
ttst       Db   '00:00', 0

.data?
hFull      Dd   ?
hShort     Dd   ?

.code
Prefs      Proc hWin   :DWORD,
                uMsg   :DWORD,
                wParam :DWORD,
                lParam :DWORD

    LOCAL tTest  :RECT
    LOCAL dRect  :RECT
    LOCAL tFnt   :HFONT
    LOCAL bFnt   :HBRUSH
    LOCAL bBack  :HBRUSH
    LOCAL bPaint :PAINTSTRUCT
    LOCAL Item   :TC_ITEM

    .if uMsg == WM_INITDIALOG
      invoke GetDlgItem, hWin, 121
      mov    hFull, eax
      invoke GetDlgItem, hWin, 122
      mov    hShort, eax

      m2m    hwndPrefs, hWin
      mov    eax, dskWindow.right
      mov    ebx, dskWindow.bottom
      shr    eax, 1
      sub    eax, 174

      shr    ebx, 1
      sub    ebx, 110
      invoke SetWindowPos, hWin, HWND_TOP, pRect.left, pRect.top, 0, 0, SWP_SHOWWINDOW or SWP_NOSIZE

      invoke PPrepare, hwndPrefs

      lea    eax, d3rus
      .if Language == LANG_ENG
        lea    eax, d3eng
      .endif
      invoke LangDialog, hWin, eax

    .elseif uMsg == WM_LBUTTONDOWN
      mov    eax, lParam
      invoke PostMessage, hwndPref, WM_NCLBUTTONDOWN, HTCAPTION, eax
      invoke SetCursor, hCursor

    .elseif uMsg == WM_LBUTTONUP
      invoke SetCursor, hOldCurs

    .elseif uMsg == WM_PAINT
      invoke PPrepare, hwndPrefs
      invoke BeginPaint, hWin, ADDR bPaint
      invoke CreateSolidBrush, tmpFore
      mov    bFnt, eax
      invoke SelectObject, bPaint.hdc, bFnt
      invoke Rectangle, bPaint.hdc, 110, 108, 167, 136
      invoke DeleteObject, bFnt
      invoke CreateSolidBrush, tmpBack
      mov    bFnt, eax
      invoke SelectObject, bPaint.hdc, bFnt
      invoke Rectangle, bPaint.hdc, 110, 140, 167, 168
      invoke Rectangle, bPaint.hdc, 110, 172, 167, 200
      invoke DeleteObject, bFnt
      push   tmpFnt.lfHeight
      mov    tmpFnt.lfHeight, 18
      invoke CreateFontIndirect, ADDR tmpFnt
      mov    tFnt, eax
      invoke SelectObject, bPaint.hdc, tFnt
      invoke SetTextColor, bPaint.hdc, tmpFore
      invoke SetBkMode, bPaint.hdc, TRANSPARENT
      mov    tTest.left, 110
      mov    tTest.top, 172
      mov    tTest.right, 167
      mov    tTest.bottom, 200
      invoke DrawTextEx, bPaint.hdc, ADDR ttst, -1, ADDR tTest, DT_CENTER or DT_VCENTER or DT_SINGLELINE, NULL
      invoke DeleteObject, tFnt
      pop    tmpFnt.lfHeight
      invoke EndPaint, hWin, ADDR bPaint

    .elseif uMsg == WM_COMMAND
      mov    eax, wParam
      shl    eax, 16
      shr    eax, 16

      .if eax >= 113 && eax <= 122
        xchg   eax, ebx
        .if ebx >=113 && ebx <= 120
          invoke SendDlgItemMessage, hWin, ebx, BM_GETCHECK, 0, 0
          push   ebx
          .if eax == BST_CHECKED
            invoke SendDlgItemMessage, hWin, ebx, BM_SETCHECK, BST_UNCHECKED, 0
            mov    ecx, FALSE
          .else
            invoke SendDlgItemMessage, hWin, ebx, BM_SETCHECK, BST_CHECKED, 0
            mov    ecx, TRUE
          .endif
          pop    eax
          sub    eax, 113
          lea    ebx, tmpTray
          mov    [ebx + eax], cl

        .elseif ebx > 120 && ebx <= 122
          invoke SendDlgItemMessage, hWin, 121, BM_GETCHECK, 0, 0
          .if eax == BST_UNCHECKED
            mov    tmpDShort, FALSE
          .elseif eax == BST_CHECKED
            mov    tmpDShort, TRUE
          .endif
        .endif

        .if tmpDate == TRUE
          invoke EnableWindow, hFull, TRUE
          invoke EnableWindow, hShort, TRUE
          invoke PPrepare, hWin
        .else
          invoke EnableWindow, hFull, FALSE
          invoke EnableWindow, hShort, FALSE
        .endif

        invoke EnableWindow, hApply, TRUE
        invoke EnableWindow, hCancel, TRUE
        mov    tmpMod, TRUE

      .elseif eax == 123 || eax == 124
        .if eax == 123
          invoke SendDlgItemMessage, hWin, 123, BM_SETCHECK, BST_CHECKED, 0
          invoke SendDlgItemMessage, hWin, 124, BM_SETCHECK, BST_UNCHECKED, 0
          invoke LangDialog, hwndPref, OFFSET d1rus
          invoke LangDialog, hDlg, ADDR d3rus
          invoke SetLang, LANG_RUS
          invoke GetString, 10
          invoke SendMessage, hwndWnd, WM_SETTEXT, 0, eax
          call   ChangeLang
        .else
          invoke SendDlgItemMessage, hWin, 124, BM_SETCHECK, BST_CHECKED, 0
          invoke SendDlgItemMessage, hWin, 123, BM_SETCHECK, BST_UNCHECKED, 0
          invoke LangDialog, hwndPref, OFFSET d1eng
          invoke LangDialog, hDlg, ADDR d3eng
          invoke SetLang, LANG_ENG
          invoke GetString, 10
          invoke SendMessage, hwndWnd, WM_SETTEXT, 0, eax
          call   ChangeLang
        .endif

      .elseif eax == 133
        invoke TakeFont, hWin, ADDR tmpFnt
        .if eax == TRUE
          mov    tmpMod, TRUE
          invoke EnableWindow, hApply, TRUE
          invoke EnableWindow, hCancel, TRUE
          call   DeInit
        .endif

      .elseif eax == 131
        invoke TakeColor, hInstance, hWin, tmpFore
        .if eax == TRUE
          mov    tmpFore, ebx
          mov    tmpMod, TRUE
          invoke EnableWindow, hApply, TRUE
          invoke EnableWindow, hCancel, TRUE
          call   DeInit
        .endif

      .elseif eax == 132
        invoke TakeColor, hInstance, hWin, tmpBack
        .if eax == TRUE
          mov    tmpBack, ebx
          mov    tmpMod, TRUE
          invoke EnableWindow, hApply, TRUE
          invoke EnableWindow, hCancel, TRUE
          call   DeInit
       .endif

      .endif
      xor    eax, eax
    .endif
    ret

DeInit:
    invoke GetClientRect, hWin, ADDR dRect
    invoke InvalidateRect, hWin, ADDR dRect, TRUE
    ret

ChangeLang:
    invoke SendMessage, hTabCtrl, TCM_GETITEM, 0, ADDR Item
    invoke GetString, 25
    mov    Item.pszText, eax
    mov    Item.imask, TCIF_TEXT
    invoke SendMessage, hTabCtrl, TCM_SETITEM, 0, ADDR Item

    invoke SendMessage, hTabCtrl, TCM_GETITEM, 1, ADDR Item
    invoke GetString, 26
    mov    Item.pszText, eax
    mov    Item.imask, TCIF_TEXT
    invoke SendMessage, hTabCtrl, TCM_SETITEM, 1, ADDR Item

    invoke SendDlgItemMessage, hwndPref, 101, TCM_GETITEM, 2, ADDR Item
    invoke GetString, 22
    mov    Item.pszText, eax
    mov    Item.imask, TCIF_TEXT
    invoke SendDlgItemMessage, hwndPref, 101, TCM_SETITEM, 2, ADDR Item
    ret

Prefs      Endp

PPrepare   Proc hWin :DWORD
    xor    ecx, ecx

@DoNext:
    lea    ebx, tmpTray
    mov    edx, 113
    add    edx, ecx
    add    ebx, ecx

    push   ecx
    mov    al, Byte Ptr [ebx]
    .if al == TRUE
      invoke SendDlgItemMessage, hWin, edx, BM_SETCHECK, BST_CHECKED, 0
    .else
      invoke SendDlgItemMessage, hWin, edx, BM_SETCHECK, BST_UNCHECKED, 0
    .endif
    pop    ecx
    inc    ecx
    cmp    ecx, 9
    jne    @DoNext

    .if tmpDate == TRUE
      invoke EnableWindow, hFull, TRUE
      invoke EnableWindow, hShort, TRUE

      .if tmpDShort != TRUE
        invoke SendDlgItemMessage, hWin, 121, BM_SETCHECK, BST_UNCHECKED, 0
        invoke SendDlgItemMessage, hWin, 122, BM_SETCHECK, BST_CHECKED, 0
      .else
        invoke SendDlgItemMessage, hWin, 121, BM_SETCHECK, BST_CHECKED, 0
        invoke SendDlgItemMessage, hWin, 122, BM_SETCHECK, BST_UNCHECKED, 0
      .endif
    .else
      invoke EnableWindow, hFull, FALSE
      invoke EnableWindow, hShort, FALSE
    .endif

    invoke SendDlgItemMessage, hWin, 124, BM_SETCHECK, BST_UNCHECKED, 0
    invoke SendDlgItemMessage, hWin, 123, BM_SETCHECK, BST_UNCHECKED, 0
    .if Language == LANG_RUS
      invoke SendDlgItemMessage, hWin, 123, BM_SETCHECK, BST_CHECKED, 0
    .else
      invoke SendDlgItemMessage, hWin, 124, BM_SETCHECK, BST_CHECKED, 0
    .endif

    ret
PPrepare   Endp

.data?
dlgAlarm   Dd  ?

.data
msgProp    db  'свойства', 0

.code
DelAlarm   PROTO  :DWORD
AddAlarm   PROTO
ListAlarms PROTO

.data?
curAlarm   Dd   ?

.code
PAlarm     Proc hWin   :DWORD,
                uMsg   :DWORD,
                wParam :DWORD,
                lParam :DWORD

        LOCAL   hList  :HWND

    .if uMsg == WM_INITDIALOG
      mov    eax, dskWindow.right
      mov    ebx, dskWindow.bottom
      shr    eax, 1
      sub    eax, 174

      shr    ebx, 1
      sub    ebx, 110
      invoke SetWindowPos, hWin, HWND_TOP, pRect.left, pRect.top, 0, 0, SWP_SHOWWINDOW or SWP_NOSIZE

      mov    eax, hWin
      mov    alarmList, eax

      invoke ListAlarms

    .elseif uMsg == WM_LBUTTONDOWN
      mov    eax, lParam
      invoke PostMessage, hwndPref, WM_NCLBUTTONDOWN, HTCAPTION, eax
      invoke SetCursor, hCursor

    .elseif uMsg == WM_LBUTTONUP
      invoke SetCursor, hOldCurs

    .elseif uMsg == WM_COMMAND
      .if wParam == 153
        .if numAlarms < NumAlarms
          invoke AddAlarm
          invoke ListAlarms
        .endif

      .elseif wParam == 154
        invoke SendDlgItemMessage, hWin, 152, LB_GETCOUNT, NULL, NULL
        .if eax >= 0
          invoke SendDlgItemMessage, hWin, 152, LB_GETCURSEL, NULL, NULL
          .if eax != LB_ERR
            inc    eax
            invoke DelAlarm, eax
            invoke ListAlarms
          .endif
        .endif

      .elseif wParam == 155
          invoke SendDlgItemMessage, hWin, 152, LB_GETCURSEL, NULL, NULL
          .if eax != LB_ERR
            inc     eax
            mov     curAlarm, eax

            getelem eax
            assume ebx :PTR ALARM
            .if [ebx].aActive

              invoke ShowWindow, hWin, SW_HIDE
              invoke CreateDialogParam, hInstance, 250, hTabCtrl, ADDR PPref, 0
              invoke ShowWindow, eax, SW_SHOW

              mov    eax, hWin
              mov    dlgAlarm, eax
            .endif
            assume ebx:NOTHING
          .endif
      .endif
    .endif

    ret
PAlarm     Endp

PAbout     Proc hWin   :DWORD,
                uMsg   :DWORD,
                wParam :DWORD,
                lParam :DWORD

           LOCAL hBmp      :DWORD
           LOCAL sPaint    :PAINTSTRUCT
           LOCAL sDC       :HDC
           LOCAL dDC       :HDC
           LOCAL nFnt      :LOGFONT
           LOCAL Buff[100] :BYTE

    .if uMsg == WM_DESTROY
      invoke DeleteObject, nBrush
      invoke DeleteObject, nFont

    .elseif uMsg == WM_LBUTTONDOWN
      mov    eax, lParam
      invoke PostMessage, hwndPref, WM_NCLBUTTONDOWN, HTCAPTION, eax
      invoke SetCursor, hCursor

    .elseif uMsg == WM_LBUTTONUP
      invoke SetCursor, hOldCurs

    .elseif uMsg == WM_COMMAND
      mov   eax, wParam
      shr   eax, 16
      .if eax == STN_CLICKED
        mov    eax, wParam
        lea    ebx, Buff
        shl    eax, 16
        shr    eax, 16
        .if eax == 207
          mov    eax, lParam
          shl    eax, 16
          shr    eax, 16
          invoke GetWindowText, eax, ebx, 255
          invoke lstrcat, ADDR getStat, ebx
          invoke ShellExecute, hWin, ADDR vOper, eax, 0, 0, SW_SHOW

        .elseif eax == 208
          mov    eax, lParam
          shl    eax, 16
          shr    eax, 16
          invoke GetWindowText, eax, ebx, 255
          invoke ShellExecute, hWin, ADDR vOper, ebx, 0, 0, SW_SHOW
        .endif
      .endif

    .elseif uMsg == WM_CTLCOLORSTATIC
      invoke GetDlgCtrlID, lParam
      .if (eax == 207) || (eax == 208)
        invoke SetBkMode, wParam, TRANSPARENT
        invoke SetTextColor, wParam, 0FF0000h
        mov    eax,nBrush
      .endif

    .elseif uMsg == WM_INITDIALOG
      mov    eax, dskWindow.right
      mov    ebx, dskWindow.bottom
      shr    eax, 1
      sub    eax, 174

      shr    ebx, 1
      sub    ebx, 110
      invoke SetWindowPos, hWin, HWND_TOP, pRect.left, pRect.top, 0, 0, SWP_SHOWWINDOW or SWP_NOSIZE

      invoke SendDlgItemMessage, hWin, 205, WM_GETFONT, 0, 0
      mov    nFont, eax
      invoke GetObject, nFont, SIZEOF LOGFONT, ADDR nFnt

      mov    nFnt.lfWeight, FW_BOLD
      invoke CreateFontIndirect, ADDR nFnt
      mov    nFont, eax
      invoke SendDlgItemMessage, hWin, 207, WM_SETFONT, nFont, 0
      invoke SendDlgItemMessage, hWin, 208, WM_SETFONT, nFont, 0

      invoke GetDlgItem, hWin, 207
      invoke SetClassLong, eax, GCL_HCURSOR, hCursor
      invoke GetDlgItem, hWin, 208
      invoke SetClassLong, eax, GCL_HCURSOR, hCursor

      invoke GetSysColor, COLOR_BTNFACE
      invoke CreateSolidBrush, eax
      mov    nBrush, eax

      .if Language == LANG_ENG
        invoke LangDialog, hWin, OFFSET d2eng
      .else
        invoke LangDialog, hWin, OFFSET d2rus
      .endif

    .elseif uMsg == WM_PAINT
      invoke BeginPaint, hWin, ADDR sPaint
      mov    dDC, eax

      invoke LoadIcon, hInstance, 1004
      invoke DrawIconEx, dDC, 16, 16, eax , 48, 48, NULL, NULL, DI_NORMAL

      invoke EndPaint, hWin, ADDR sPaint

      invoke GetDlgItem, hWin, 202
      invoke GetDlgCtrlID, eax
      invoke SendDlgItemMessage, hWin, eax, WM_SETTEXT, NULL, ADDR dlgTitle
    .endif
    ret
PAbout     Endp

.data?
wndActive   Dd  ?

.code
PPref      Proc hWin   :DWORD,
                uMsg   :DWORD,
                wParam :DWORD,
                lParam :DWORD

    .if uMsg == WM_INITDIALOG
      mov    eax, dskWindow.right
      mov    ebx, dskWindow.bottom
      shr    eax, 1
      sub    eax, 174

      shr    ebx, 1
      sub    ebx, 110
      invoke SetWindowPos, hWin, HWND_TOP, pRect.left, pRect.top, 0, 0, SWP_SHOWWINDOW or SWP_NOSIZE

      invoke IsWindowEnabled, hApply
      mov    wndActive, eax
      .if wndActive
        invoke EnableWindow, hApply, FALSE
        invoke EnableWindow, hCancel, FALSE
      .endif

      mov    canChange, FALSE
      invoke SetForegroundWindow, hWin

      invoke EnableWindow, hOk, FALSE

    .elseif uMsg == WM_LBUTTONDOWN
      mov    eax, lParam
      invoke PostMessage, hwndPref, WM_NCLBUTTONDOWN, HTCAPTION, eax
      invoke SetCursor, hCursor

    .elseif uMsg == WM_LBUTTONUP
      invoke SetCursor, hOldCurs

    .elseif uMsg == WM_COMMAND
      .if wParam == 252
        invoke EnableWindow, hOk, TRUE
   
        .if wndActive
          invoke EnableWindow, hApply, TRUE
          invoke EnableWindow, hCancel, TRUE
        .endif

        invoke DestroyWindow, hWin

        invoke ShowWindow, dlgAlarm, SW_SHOW
        invoke SetForegroundWindow, dlgAlarm
        mov    canChange, TRUE

      .elseif wParam == 262
        invoke TakeFile, hWin, TYPE_WAV
        invoke GetDlgItem, hWin, 259
        invoke SendMessage, eax, WM_SETTEXT, NULL, f.lpstrFile

      .elseif wParam == 263
        invoke TakeFile, hWin, TYPE_EXE
        invoke GetDlgItem, hWin, 260
        invoke SendMessage, eax, WM_SETTEXT, NULL, f.lpstrFile

      .endif

    .endif

    ret
PPref      Endp
