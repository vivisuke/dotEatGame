#include "consoleUtil.h"

//----------------------------------------------------------------------
void coutW(wchar_t ch)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD len = 1;
	WriteConsoleW(hCons, &ch, len, &len, 0);
}
void coutW(const wchar_t *ptr)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD len = wcslen(ptr);
	WriteConsoleW(hCons, ptr, len, &len, 0);
}
void coutW(const wchar_t *ptr, int len)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	WriteConsoleW(hCons, ptr, len, (DWORD *)&len, 0);
}
void coutW(const std::wstring &str)
{
	coutW((const wchar_t *)str.c_str(), (int)str.size());
}
//----------------------------------------------------------------------
//	文字色指定 for Windows Console
void setColor(int col)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	WORD attr = 0;
	if( col & COL_INTENSITY )
		attr |= FOREGROUND_INTENSITY;
	if( col & COL_RED_MASK )
		attr |= FOREGROUND_RED;
	if( col & COL_GREEN_MASK )
		attr |= FOREGROUND_GREEN;
	if( col & COL_BLUE_MASK )
		attr |= FOREGROUND_BLUE;
	SetConsoleTextAttribute(hCons, attr);
}
void setColor(int fg, int bg)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	WORD attr = 0;
	if( fg & COL_INTENSITY )
		attr |= FOREGROUND_INTENSITY;
	if( fg & COL_RED_MASK )
		attr |= FOREGROUND_RED;
	if( fg & COL_GREEN_MASK )
		attr |= FOREGROUND_GREEN;
	if( fg & COL_BLUE_MASK )
		attr |= FOREGROUND_BLUE;
	
	if( bg & COL_INTENSITY )
		attr |= BACKGROUND_INTENSITY;
	if( bg & COL_RED_MASK )
		attr |= BACKGROUND_RED;
	if( bg & COL_GREEN_MASK )
		attr |= BACKGROUND_GREEN;
	if( bg & COL_BLUE_MASK )
		attr |= BACKGROUND_BLUE;
	SetConsoleTextAttribute(hCons, attr);
}
//	カーソル位置指定 for Windows Console
void setCursorPos(int x, int y)
{
	HANDLE hCons = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hCons, pos);
}
//	キー押下チェック
bool isKeyPressed(int vKey)
{
	return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}
//----------------------------------------------------------------------
