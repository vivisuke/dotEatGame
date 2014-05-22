#pragma once
//----------------------------------------------------------------------
//			DblBuffer ver 0.001
//			Copyright (C) 2014 by N.Tsuda
//			Description: 画面チラチE��を無くすためのダブルバッファクラス
//			License: CDDL 1.0 (http://opensource.org/licenses/CDDL-1.0)
//----------------------------------------------------------------------

#include <Windows.h>
#include <string>
class DblBuffer
{
public:
	enum {
		BLACK = 0x00,
		DARK_BLUE = 0x01,
		DARK_GREEN = 0x02,
		DARK_CYAN = 0x03,
		DARK_RED = 0x04,
		DARK_VIOLET = 0x05,
		DARK_YELLOW = 0x06,
		GRAY = 0x07,
		LIGHT_GRAY = 0x08,
		BLUE = 0x09,
		GREEN = 0x0a,
		CYAN = 0x0b,
		RED = 0x0c,
		VIOLET = 0x0d,
		YELLOW = 0x0e,
		WHITE = 0x0f,
		INTENSITY = 0x08,		//	高輝度マスク
		RED_MASK = 0x04,
		GREEN_MASK = 0x02,
		BLUE_MASK = 0x01,
	};
	
public:
	DblBuffer();
	~DblBuffer();

public:
	void setColor(int);
	void setColor(int, int);
	void setCursorPos(int, int);
	void write(const char *);
	void write(const wchar_t *);
	void write(const std::string &);
	void write(const std::wstring &);
	void swap();
	
private:
	bool			m_swapped;
	HANDLE		m_hCons1;		//	コンソールハンドルその１
	HANDLE		m_hCons2;		//	コンソールハンドルその２
};
