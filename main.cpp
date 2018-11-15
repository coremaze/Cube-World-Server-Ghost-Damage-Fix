#include <iostream>
#include <windows.h>

unsigned int base;

unsigned int JMP_back;
void __declspec(naked) __declspec(noinline) filterguids(){
    asm("cmp dword ptr [edx+0x8], 0"); //jump if attacker GUID is null
    asm("ja 0f");
    asm("cmp dword ptr [edx+0xC], 0");
    asm("ja 0f");

    //If attacker isn't null, set damage to 0
    asm("mov dword ptr [edx+0x18], 0");

    asm("0:");
    asm("mov [eax], edx");//original code
    asm("mov eax, [ebp+0xC]");
    asm("jmp [_JMP_back]");
}

void WriteJMP(BYTE* location, BYTE* newFunction){
    DWORD dwOldProtection;
    VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
    VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (unsigned int)GetModuleHandle(NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            JMP_back = base + 0x20F72;
            WriteJMP((BYTE*)(base + 0x20F6D), (BYTE*)&filterguids);
            break;
    }
    return true;
}
