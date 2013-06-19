        getelem MACRO m4
            push eax
            push ecx
            mov  ebx, ALARMS
            mov  eax, m4
            mov  ecx, [ebx + eax * 4]
            mov  ebx, ecx
            pop  ecx
            pop  eax
        ENDM

.data?
TIME         Struc
   Hour      Db         ?
   Minute    Db         ?

   Year      Dw         ?
   Month     Db         ?
   Day       Db         ?
TIME         Ends

ALARM        Struc
   aActive   Db         ?
   aDaily    Db         ?

   IsWave    Db         ?
   IsCommand Db         ?
   IsMessage Db         ?

   aReserved Db         25 Dup(?)

   aTime     TIME       <?>
   aTitle    Db         256 Dup(?)
   aWave     Db         256 Dup(?)
   aCommand  Db         256 Dup(?)
   aMessage  Db         1024 Dup(?)
ALARM        Ends

ALARMS       Dd         ?
alFile       Db         1100 Dup(?)

.data
numAlarms    Dd         0
filAlarms    Db         'ALARMS.DAT', 0

SortAlarms   PROTO
CheckAlarms  PROTO

.code
Alarm   Proc aInstance :DWORD,
             mType     :DWORD,
             lpWave    :DWORD

   .if mType == WAVE_BEEP
     invoke MessageBeep, 0FFFFFFFFh
   .else
     invoke waveOutGetNumDevs
     .if eax > 0
       .if   mType == WAVE_STD
          invoke PlaySound, 1005, aInstance, SND_RESOURCE or SND_ASYNC
       .elseif mType == WAVE_FILE
          invoke PlaySound, lpWave, aInstance, SND_FILENAME or SND_ASYNC
       .endif
     .else
       invoke MessageBeep, 0FFFFFFFFh
     .endif
   .endif
   ret
Alarm   Endp

FillMem  PROTO

LoadAlarms   Proc
    LOCAL tmpRead :DWORD
    LOCAL fHandle :DWORD

    ; Создание имени файла с будильниками
    invoke lstrcat, ADDR alFile, ADDR plugsDir
    invoke lstrcat, ADDR alFile, ADDR filAlarms

    ; Выделение памяти под массив указателей
    invoke LocalAlloc, LMEM_FIXED, SIZEOF DWORD * NumAlarms
    mov    ALARMS, eax

    ; Выделение памяти под массив с будильниками
    mov    ebx, ALARMS
    mov    eax, NumAlarms
    .while eax
      push   eax
      push   ebx
      invoke LocalAlloc, LMEM_FIXED, SIZEOF ALARM
      mov    ecx, eax
      pop    ebx
      pop    eax
      mov    [ebx + eax * 4], ecx
      dec    eax
    .endw

    invoke FillMem

    ; Загрузка будильников
    invoke CreateFile, ADDR alFile, GENERIC_READ, NULL, NULL,
                       OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    mov fHandle, eax

    mov eax, 1
    .repeat
      push    eax
      getelem eax
      invoke  ReadFile, fHandle, ebx, SIZEOF ALARM, ADDR tmpRead, NULL
      pop     eax
      inc     eax
    .until eax > NumAlarms

    invoke CloseHandle, fHandle

    invoke CheckAlarms
    ret
LoadAlarms   Endp

SaveAlarms   Proc
    LOCAL fHandle:DWORD
    LOCAL Written:DWORD

    ; Запись будильников в файл
    invoke CreateFile, ADDR alFile, GENERIC_WRITE, 0, NULL,
                       OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    mov    fHandle, eax

    mov eax, 1
    .repeat
      push    eax
      getelem eax
      invoke  WriteFile, fHandle, ebx, SIZEOF ALARM, ADDR Written, NULL
      pop     eax
      inc     eax
    .until eax > NumAlarms

    invoke CloseHandle, fHandle

    ; Очистка выделенной под будильники памяти
    mov    eax, NumAlarms
    mov    ebx, ALARMS
    .while eax
      pushad
      mov    ecx, [ebx + eax * 4]
      invoke LocalFree, ecx
      popad
      dec    eax
    .endw

    ; Очистка массива указателей
    invoke LocalFree, ALARMS
    ret
SaveAlarms   Endp

FillElem PROTO :DWORD

FillMem      Proc
  pushad

  mov  eax, NumAlarms
  .while eax
    invoke FillElem, eax
    dec    eax
  .endw

  popad
  ret
FillMem      Endp

FillElem     Proc Elem :DWORD
  pushad
  getelem Elem
  xor     eax, eax
  mov     ecx, SIZEOF ALARM
@2l:
  mov  [ebx], al
  inc  ebx
  loop @2l
  popad
  ret
FillElem     Endp

CheckAlarms  Proc
  mov  numAlarms, 0
  mov  ecx, NumAlarms
  .while ecx
    getelem ecx
    assume ebx:PTR ALARM
    .if [ebx].aActive
      inc numAlarms
    .endif
    assume ebx:NOTHING
    dec  ecx
  .endw
  ret
CheckAlarms  Endp

AlarmPrefs   Proc
    ret
AlarmPrefs   Endp

ListAlarms   Proc
    invoke CheckAlarms

    invoke SendDlgItemMessage, alarmList, 152, LB_GETCOUNT, NULL, NULL
    inc    eax
    .while eax
      dec    eax
      push   eax
      invoke SendDlgItemMessage, alarmList, 152, LB_DELETESTRING, eax, NULL
      pop    eax
    .endw

    mov eax, NumAlarms
    .while eax
      push    eax
      getelem eax
      assume  ebx:PTR ALARM
      .if [ebx].aActive
        invoke SendDlgItemMessage, alarmList, 152, LB_ADDSTRING, NULL,
               ADDR dlgTitle
      .endif
      assume  ebx:NOTHING
      pop  eax
      dec  eax
    .endw
    ret
ListAlarms   Endp

.data
alMsg        Db  'Удалить будильник?', 0

.code
DelAlarm     Proc Num
    pushad
    getelem Num
    assume ebx:PTR ALARM
    .if [ebx].aActive
      ; Запрашиваем разрешение на удаление
      invoke MessageBox, hTabCtrl, ADDR alMsg, ADDR dlgTitle, MB_YESNO
      .if eax == IDYES
        invoke  FillElem, Num
      .endif
    .endif
    assume ebx:NOTHING

    invoke SortAlarms

    popad
    ret
DelAlarm     Endp

AddAlarm     Proc
   pushad
   inc     numAlarms
   invoke  FillElem, numAlarms

   getelem numAlarms
   assume  ebx:PTR ALARM
   mov     [ebx].aActive, TRUE
   assume  ebx:NOTHING
   popad
   ret
AddAlarm     Endp

ChangeAlarms Proc  Src:DWORD, Dst:DWORD
   LOCAL     tmpSave :DWORD

   pushad
   getelem Src
   push    ebx
   getelem Dst
   pop     eax

   assume  eax:PTR ALARM
   assume  ebx:PTR ALARM

   .if ![eax].aActive
     .if [ebx].aActive
       invoke CopyMemory, ADDR fKeyBuf, eax, SIZEOF ALARM
       invoke CopyMemory, eax, ebx, SIZEOF ALARM
       invoke CopyMemory, ebx, ADDR fKeyBuf, SIZEOF ALARM
     .endif
   .endif

   assume  ebx:NOTHING
   assume  eax:NOTHING


   popad
   ret
ChangeAlarms Endp

SortAlarms   Proc
   push   eax
   push   ebx
   push   ecx

   mov    eax, 1
   .repeat
     push   eax
     mov    ebx, NumAlarms
     mov    ecx, eax
     .while ebx != ecx
       invoke ChangeAlarms, eax, ebx
       dec    ebx
     .endw
     pop    eax
     inc    eax
   .until eax == NumAlarms - 1

   pop    ecx
   pop    ebx
   pop    eax
   ret
SortAlarms   Endp
