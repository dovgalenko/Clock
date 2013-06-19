TCC_PLUG_INIT  Equ 1
TCC_PLUG_EXIT  Equ 2
TCC_PLUG_IDLE  Equ 3
TCC_PLUG_MAKE  Equ 4
TCC_PLUG_PAINT Equ 5
TCC_PLUG_DONE  Equ 6

TCC_MENU_EXEC  Equ 7
TCC_MENU_DEL   Equ 8
TCC_MENU_MAKE  Equ 9

.data
plugMnu        Dd  MNU_EXTERN

.code
InitPlugins    Proc
     LOCAL     fFile   :WIN32_FIND_DATA
     LOCAL     sHandle :DWORD
     LOCAL     hLib    :DWORD

     invoke lstrcat, ADDR plugsDir, ADDR FMask
     invoke FindFirstFile, ADDR plugsDir, ADDR fFile

     .if eax != INVALID_HANDLE_VALUE
       mov    sHandle, eax

       .repeat
         mov    eax, fFile.dwFileAttributes
         and    eax, FILE_ATTRIBUTE_DIRECTORY

         .if eax != FILE_ATTRIBUTE_DIRECTORY
           invoke LoadLibrary, ADDR fFile.cFileName
           mov    hLib, eax

           invoke GetProcAddress, hLib, ADDR initFunc
           .if eax != NULL
             call   eax
             .if eax == TCC_PLUG_DONE
               invoke GetProcAddress, hLib, ADDR dispFunc
               mov    edx, hLib
               xchg   eax, ecx

               lea    ebx, hPlugs
               xor    eax, eax
               mov    al, numPlugs

               mov    [ebx + eax * 4], ecx
               lea    ebx, hLibs
               mov    [ebx + eax * 4], ecx

               inc    numPlugs
             .else
               invoke FreeLibrary, hLib
             .endif
           .else
             invoke FreeLibrary, hLib
           .endif
         .endif
         invoke FindNextFile, sHandle, ADDR fFile
       .until eax == 0

       invoke FindClose, sHandle

       invoke PlugMessage, TCC_PLUG_INIT, NULL, NULL
     .endif
     ret
InitPlugins    Endp

PlugMessage    Proc  uMsg   :DWORD,
                     lParam :DWORD,
                     wParam :DWORD

     LOCAL   PlNum  :BYTE

     .if numPlugs >= 1
       mov     al, numPlugs
       mov     PlNum, al
       .repeat
         dec    PlNum

         lea    ebx, hPlugs
         xor    eax, eax
         mov    al, PlNum
         mov    ecx, [ebx + eax * 4]

         .if uMsg == TCC_MENU_MAKE
           mov    eax, plugMnu
           mov    wParam, eax
         .endif

         Push   wParam
         Push   lParam
         Push   uMsg
         Call   ecx

         .if uMsg == TCC_MENU_MAKE && eax >= 1
           add    plugMnu, eax
         .endif
       .until PlNum == 0
     .endif
     ret
PlugMessage    Endp

FreePlugins    Proc

     .if numPlugs >= 1
       invoke PlugMessage, TCC_PLUG_EXIT, NULL, NULL

       .repeat
         dec    numPlugs

         lea    ebx, hLibs
         xor    eax, eax
         mov    al, numPlugs
         mov    ecx, [ebx + eax * 4]
         invoke FreeLibrary, ecx
       .until numPlugs == 0
     .endif
     ret
FreePlugins    Endp