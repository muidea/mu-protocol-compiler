#include <Windows.h>
#include "mupconsolehelper.h"

static WORD g_preAttribute = 0;

void changeColor()
{
    HANDLE hCon = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO sInfo;
    ::GetConsoleScreenBufferInfo(hCon,&sInfo);

    g_preAttribute = sInfo.wAttributes;

    ::SetConsoleTextAttribute(hCon, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void restoreColor()
{
    HANDLE hCon = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::SetConsoleTextAttribute(hCon, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

