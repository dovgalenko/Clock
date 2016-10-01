DelayTime   Equ 500

.code
MakeRegistry  Proc  actType :DWORD

    LOCAL  ReturnValue :DWORD
    LOCAL  hTest       :DWORD

    invoke GetModuleFileName, hInstance, ADDR fKeyBuf, 512

    invoke RegOpenKeyEx, HKEY_CURRENT_USER, ADDR hDir, 0, KEY_ALL_ACCESS, ADDR hTest

    .if eax != ERROR_SUCCESS
      invoke RegCreateKeyEx, HKEY_CURRENT_USER, ADDR hDir, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, ADDR hTest, ADDR ReturnValue
    .endif

    .if actType == ACT_DEL
      invoke RegDeleteValue, hTest, ADDR hKey
    .elseif actType == ACT_ADD
      invoke lstrlen, ADDR fKeyBuf
      invoke RegSetValueEx, hTest, ADDR hKey, 0, REG_SZ, ADDR fKeyBuf, eax
    .endif

    invoke RegCloseKey, hTest
    ret
MakeRegistry  Endp


CutPath       Proc    lpName  :DWORD

    invoke lstrlen, lpName
    mov    ebx, lpName
    add    ebx, eax
@DecNext:
    dec    eax
    dec    ebx
    cmp    Byte Ptr [ebx], '\'
    jne    @DecNext
    mov    Byte Ptr [ebx + 1], 0
    inc    eax
    ret
CutPath        Endp

WriteRegistry  Proc

    LOCAL  ReturnValue :DWORD
    LOCAL  hTest       :DWORD

    .if AutoBoot == TRUE
      invoke MakeRegistry, ACT_ADD
    .endif

    invoke RegOpenKeyEx, HKEY_CURRENT_USER, ADDR hClock, 0, KEY_ALL_ACCESS, ADDR hTest

    .if eax != ERROR_SUCCESS
      invoke RegCreateKeyEx, HKEY_CURRENT_USER, ADDR hClock, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, ADDR hTest, ADDR ReturnValue
    .endif

    invoke RegSetValueEx, hTest, ADDR rgWnd, 0, REG_BINARY, ADDR clkWindow, SIZEOF RECT
    invoke RegSetValueEx, hTest, ADDR rgType, 0, REG_BINARY, ADDR AutoBoot, settingsLen
    invoke RegSetValueEx, hTest, ADDR rgFont, 0, REG_BINARY, ADDR logFnt, SIZEOF LOGFONT

    invoke GetModuleFileName, hInstance, ADDR fKeyBuf, 512
    invoke CutPath, ADDR fKeyBuf
    inc    eax
    invoke RegSetValueEx, hTest, ADDR rgDir, 0, REG_SZ, ADDR fKeyBuf, eax

    invoke RegCloseKey, hTest
    ret
WriteRegistry  Endp

ReadRegistry  Proc

    LOCAL  hTest       :DWORD
    LOCAL  rSize       :DWORD
    LOCAL  rType       :DWORD

    mov    rType, REG_BINARY

    invoke RegOpenKeyEx, HKEY_CURRENT_USER, ADDR hClock, 0, KEY_ALL_ACCESS, ADDR hTest
    .if eax == ERROR_SUCCESS
       mov    rSize, SIZEOF RECT
       invoke RegQueryValueEx, hTest, ADDR rgWnd, NULL, ADDR rType, ADDR clkWindow, ADDR rSize
       invoke RegQueryValueEx, hTest, ADDR rgType, NULL, ADDR rType, ADDR AutoBoot, ADDR settingsLen
;       mov    rSize, SIZEOF LOGFONT
;       invoke RegQueryValueEx, hTest, ADDR rgFont, NULL, ADDR rType, ADDR logFnt, ADDR rSize

       mov    rType, REG_SZ

       mov    rSize, 512
       invoke RegQueryValueEx, hTest, ADDR rgDir, NULL, ADDR rType, ADDR plugsDir, ADDR rSize
       invoke RegCloseKey, hTest
    .endif
    ret
ReadRegistry  Endp

ReadCPU       Proc
  ; Возвращает кол-во мегагерц процессора в Eax
  LOCAL TimerHi, TimerLo,
        PriorityClass, Priority,
        hProc, hTd:DWORD

  ; Сохраняем класс приоритета процесса
  mov    eax, @Result(GetCurrentProcess)
  mov    PriorityClass, @Result(GetPriorityClass, eax)
  
  ; Сохраняем приоритет потока
  mov    eax, @Result(GetCurrentThread)
  mov    Priority, @Result(GetThreadPriority, eax)

  ; Берем идентификаторы процесса и потока
  mov    hProc, @Result(GetCurrentProcess)
  mov    hTd, @Result(GetCurrentThread)

  ; Выставляем приоритеты процесса и потока
  invoke SetPriorityClass, hProc, REALTIME_PRIORITY_CLASS
  invoke SetThreadPriority, hTd, THREAD_PRIORITY_TIME_CRITICAL

  ; Ждем немного
  invoke Sleep, 10

  ; Первый замер
  rdtsc
  mov    TimerLo, eax
  mov    TimerHi, edx

  invoke Sleep, DelayTime

  ; Второй замер
  rdtsc
  sub    eax, TimerLo
  sub    edx, TimerHi
  mov    TimerLo, eax
  mov    TimerHi, edx

  push   eax

  ; Восстанавливаем приоритеты процесса и потока
  invoke SetPriorityClass, hProc, PriorityClass
  invoke SetThreadPriority, hTd, Priority

  mov    edx, 1000
  imul   ebx, edx, DelayTime

  xor    edx, edx
  pop    eax
  idiv   ebx
  ret
ReadCPU       Endp