.data?
Temporary  Db  100 Dup(?)

.code
SetLang   Proc  Lang  :DWORD
   mov    eax, Lang
   mov    Language, al
   ret
SetLang   Endp

GetLang   Proc
   xor    eax, eax
   mov    al, Language
   ret
GetLang   Endp

GetString Proc  StrId :DWORD

   mov    eax, StrId
   .if Language == LANG_ENG
     add    eax, 50
   .endif
   invoke LoadString, hInstance, eax, ADDR Temporary, 100
   lea    eax, Temporary
   ret
GetString Endp

LangDialog Proc hWin :DWORD,
                Ofs  :DWORD

   pushad
   mov    esi, Ofs
@NextItem:
   xor    eax, eax
   lodsb
   cmp    eax, 0
   je     @NoItems
   invoke GetDlgItem, hWin, eax
   xchg   eax, ebx
   lodsd
   invoke SendMessage, ebx, WM_SETTEXT, NULL, eax
   jmp    @NextItem
@NoItems:
   popad
   ret
LangDialog Endp
