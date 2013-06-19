.data
IsDebug     Db FALSE
icnAdd      Dd ?
icnDel      Dd ?
;icnForm     Db 'add:%lu, del:%lu', 0

hAccel      Dd ?

hwndPref    Dd NULL

libHand         Dd ?
libProc         Dd ?

dShell          Db 'Shell_TrayWnd', 0
dDesk           Db 'Desktop', 0
dProg           Db 'Progman', 0

.data?

.data?
hwndTray        Dd ?
hwndProg        Dd ?

szAddr          Dd ?
szId            Dd ?

alarmList       Dd  ?

showAbout       Db  ?

hTabCtrl        Dd  ?

pRect           RECT  <?>
hDlg            HWND  ?

curXadd         Dd  ?
curYadd         Dd  ?

;--------------------
tmpTray         Db  ?
tmpTopMost      Db  ?
tmpTasks        Db  ?
tmpDate         Db  ?
tmpSec          Db  ?
tmpRes          Db  ?
tmpMem          Db  ?
tmpBlink        Db  ?
tmpDShort       Db  ?

tmpFore         Dd  ?
tmpBack         Dd  ?
;--------------------

hMenu           Dd  ?
hOption         Dd  ?
hCursor         Dd  ?
hOldCurs        Dd  ?

oldHour         Dw  ?
oldMinute       Dw  ?
oldSecond       Dw  ?
nCount          Dw  ?

hLibs           Dd  256 Dup(?)
hPlugs          Dd  256 Dup(?)

prgWnd          Dd  ?

; Данные Desktop'а
dskWindow       RECT <?>
fntWindow       RECT <?>

;SecondCopy      Dd  ?

CommandLine     Dd  ?

; Хэндл проги
hInstance       Dd  ?
; Хэндл процесса
hProcess        Dd  ?
; Хэндл таймера
hTimer          Dd  ?
; Хэндл DC
hDC             Dd  ?
; Данные шрифта
hFNT            Dd  ?
; Хэндл окна
hWnd            Dd  ?
hmWnd           Dd  ?

; Кисть для рисования окна
hBrush          Dd  ?
hIcon           Dd  ?

strucTime       SYSTEMTIME <?>
f               OPENFILENAME <?>

; Структура Paint
strucPaint      PAINTSTRUCT <?>

mnuDesktop      Dd  ?
mnuDisplay      Dd  ?

; Структура под выбор цвета(вернее дополнительные цвета установленные юзверем)
custColors      Dd   16 Dup(?)

dlgTitle        Db  256 Dup(?)
fKeyBuf         Db  2048 Dup(?)

tmpBuf          Db  256 Dup(?)
iniFile         Db  512 Dup(?)
plugsDir        Db  1024 Dup(?)

tmpFnt          LOGFONT <?>

.data
tmpMod          Db  FALSE
IsActive        Dd  TRUE

clkWindow       RECT <0>

; Движение курсора
Move            Db  FALSE

mTime           Db  50 Dup (0)

icn             NOTIFYICONDATA <0, 0, 5001, NIF_ICON or NIF_TIP or NIF_MESSAGE, \
                               WM_ICON, RES_MICON, ' '>

; Данные об окне, из файла установок
AutoBoot        Db  FALSE

ShowInTray      Db  FALSE
AlwaysOnTop     Db  TRUE
ShowInTasks     Db  TRUE
ShowDate        Db  FALSE
ShowSec         Db  TRUE
ShowRes         Db  FALSE
ShowMem         Db  FALSE
ShowBlink       Db  FALSE
dateShort       Db  TRUE


; Данные о цвете
fntColor        Dd  0FFFFFFh
bkColor         Dd  0

Language1       Db  LANG_ENG

seeTray         Db  TRUE
seeProg         Db  TRUE

Language       Db  LANG_ENG

logFnt          LOGFONT  <25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, \
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, \
                         FF_MODERN or DEFAULT_PITCH, 'Times New Roman'>

; Название диалога и титул
wClass          Db  'Tiny_Clock', 0

; Data for registry.inc
hKey            Db   'Clock', 0
hDir            Db   'Software\Microsoft\Windows\CurrentVersion\Run', 0
hClock          Db   'Software\The CHEMIST\Clock', 0
rgWnd           Db   'Pos', 0
rgType          Db   'Type', 0
rgFont          Db   'Font', 0
rgDir           Db   'Dir', 0

; Data for plugins.inc
numPlugs        Db  0
FMask           Db  '*.DLL', 0
initFunc        Db  'InitPlugin', 0
dispFunc        Db  'Dispatch', 0

; Data for dialogs.inc
Filters         Db  "Файлы звуков (.WAV)", 0, "*.WAV", 0, NULL, NULL
Filters1        Db  "Исполняемые файлы(.EXE)", 0, "*.EXE", 0, NULL, NULL
FiltersEng      Db  "Sound files (.WAV)", 0, "*.WAV", 0, NULL, NULL
FiltersEng1     Db  "Executable files(.EXE)", 0, "*.EXE", 0, NULL, NULL

initDir         Db  "C:\", 0
fntChanged      Db  FALSE

tmpExec         Db  '\control.exe ', 0
tmpTime         Db  'timedate.cpl', 0
tmpDesk         Db  'desk.cpl', 0

d1rus1          Db  'Ok', 0
d1rus2          Db  'Отмена', 0
d1rus3          Db  'Применить', 0
d1eng2          Db  'Cancel', 0
d1eng3          Db  'Apply', 0
d2rus1          Db  'Программа написана полностью на языке Ассемблер... Любые пожелания приветствуются.', 0
d2eng1          Db  'This program was written using Assembly language. All suggestions are welcome!', 0
d3rus1          Db  'Свойства окна', 0
d3rus2          Db  'в трее', 0
d3rus3          Db  'поверх всех', 0
d3rus4          Db  'на панели задач', 0
d3rus5          Db  'Показывать', 0
d3rus6          Db  'дату', 0
d3rus7          Db  'секунды', 0
d3rus8          Db  'ресурсы', 0
d3rus9          Db  'Внешний вид', 0
d3rus10         Db  'Цвет текста', 0
d3rus11         Db  'Цвет фона', 0
d3rus12         Db  'Шрифт', 0
d3rus13         Db  'кратко', 0
d3rus14         Db  'полностью', 0
d3rus15         Db  'память', 0
d3rus16         Db  'Язык', 0
d3rus17         Db  'мигать', 0

d3eng1          Db  'Window settings', 0
d3eng2          Db  'icon in tray', 0
d3eng3          Db  'always on top', 0
d3eng4          Db  'show on taskbar', 0
d3eng5          Db  'Show', 0
d3eng6          Db  'date', 0
d3eng7          Db  'seconds', 0
d3eng8          Db  'resources', 0
d3eng9          Db  'View settings', 0
d3eng10         Db  'ForeColor', 0
d3eng11         Db  'BackColor', 0
d3eng12         Db  'Font', 0
d3eng13         Db  'short', 0
d3eng14         Db  'full', 0
d3eng15         Db  'memory', 0
d3eng16         Db  'Language', 0
d3eng17         Db  'blink', 0

d1rus           Db  102
                Dd  OFFSET d1rus1
                Db  104
                Dd  OFFSET d1rus2
                Db  103
                Dd  OFFSET d1rus3
                Db  0

d1eng           Db  102
                Dd  OFFSET d1rus1
                Db  104
                Dd  OFFSET d1eng2
                Db  103
                Dd  OFFSET d1eng3
                Db  0

d2rus           Db  160
                Dd  OFFSET d2rus1
                Db  0

d2eng           Db  160
                Dd  OFFSET d2eng1
                Db  0

d3rus           Db  112
                Dd  OFFSET d3rus1
                Db  113
                Dd  OFFSET d3rus2
                Db  114
                Dd  OFFSET d3rus3
                Db  115
                Dd  OFFSET d3rus4
                Db  134
                Dd  OFFSET d3rus5
                Db  116
                Dd  OFFSET d3rus6
                Db  117
                Dd  OFFSET d3rus7
                Db  118
                Dd  OFFSET d3rus8
                Db  121
                Dd  OFFSET d3rus13
                Db  122
                Dd  OFFSET d3rus14
                Db  130
                Dd  OFFSET d3rus9
                Db  131
                Dd  OFFSET d3rus10
                Db  132
                Dd  OFFSET d3rus11
                Db  133
                Dd  OFFSET d3rus12
                Db  135
                Dd  OFFSET d3rus16
                Db  119
                Dd  OFFSET d3rus15
                Db  120
                Dd  OFFSET d3rus17
                Db  0

d3eng           Db  112
                Dd  OFFSET d3eng1
                Db  113
                Dd  OFFSET d3eng2
                Db  114
                Dd  OFFSET d3eng3
                Db  115
                Dd  OFFSET d3eng4
                Db  134
                Dd  OFFSET d3eng5
                Db  116
                Dd  OFFSET d3eng6
                Db  117
                Dd  OFFSET d3eng7
                Db  118
                Dd  OFFSET d3eng8
                Db  121
                Dd  OFFSET d3eng13
                Db  122
                Dd  OFFSET d3eng14
                Db  130
                Dd  OFFSET d3eng9
                Db  131
                Dd  OFFSET d3eng10
                Db  132
                Dd  OFFSET d3eng11
                Db  133
                Dd  OFFSET d3eng12
                Db  135
                Dd  OFFSET d3eng16
                Db  119
                Dd  OFFSET d3eng15
                Db  120
                Dd  OFFSET d3eng17
                Db  0

szSpace         Db  ' ', 0

.code
