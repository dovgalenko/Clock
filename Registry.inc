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
    invoke RegSetValueEx, hTest, ADDR rgType, 0, REG_BINARY, ADDR AutoBoot, 21
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
       mov    rSize, 21
       invoke RegQueryValueEx, hTest, ADDR rgType, NULL, ADDR rType, ADDR AutoBoot, ADDR rSize
;       mov    rSize, SIZEOF LOGFONT
;       invoke RegQueryValueEx, hTest, ADDR rgFont, NULL, ADDR rType, ADDR logFnt, ADDR rSize

       mov    rType, REG_SZ

       mov    rSize, 512
       invoke RegQueryValueEx, hTest, ADDR rgDir, NULL, ADDR rType, ADDR plugsDir, ADDR rSize
       invoke RegCloseKey, hTest
    .endif
    ret
ReadRegistry  Endp

.data
hkdd          Db  'PerfStats\StatData', 0
usage         Db  'KERNEL\CPUUsage', 0
threads       Db  'KERNEL\Threads', 0

.code
ReadCPU       Proc
    LOCAL  hTest       :DWORD
    LOCAL  rSize       :DWORD
    LOCAL  rType       :DWORD
    LOCAL  cpuLoc      :DWORD
    LOCAL  thrLoc      :DWORD

    mov    rType, REG_BINARY
    mov    rSize, SIZEOF DWORD

    invoke RegOpenKeyEx, HKEY_DYN_DATA, ADDR hkdd, 0, KEY_READ, ADDR hTest
    .if eax == ERROR_SUCCESS
      invoke RegQueryValueEx, hTest, ADDR usage, NULL, ADDR rType, ADDR cpuLoc, ADDR rSize
      invoke RegQueryValueEx, hTest, ADDR threads, NULL, ADDR rType, ADDR thrLoc, ADDR rSize
      invoke RegCloseKey, hTest
    .endif
    mov    eax, cpuLoc
    mov    ebx, thrLoc
    ret
ReadCPU       Endp