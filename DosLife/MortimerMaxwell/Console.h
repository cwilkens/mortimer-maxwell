// the header for the Console class
// the console class handles input and output to and from the console

#include <windows.h>
#include <vector>

#ifndef CONSOLE_H
#define CONSOLE_H

class ConsoleLevel;

class Console {
public:
	Console(int width, int height);
	~Console();

	// object handling stuff
	void SetLevel(ConsoleLevel* level);
	ConsoleLevel* GetLevel();

	//  buffer-related stuff
	void DrawBuffer();
	void ClearBuffer(int color2, int color1 = 0);
	void FillScreen(int color2, int color1 = 0);

	// setting colors and characters
	void SetColor(int _x, int _y, int color1, int color2, int width = 1, int height = 1);
	void SetColor(int _x, int _y, int colors);
	void SetFColor(int _x, int _y, int color, int width = 1, int height = 1);
	void SetBColor(int _x, int _y, int color, int width = 1, int height = 1);
	void WriteChar(int _x, int _y, char char1, int hrepeat = 1, int vrepeat = 1);
	void WriteStr(int _x, int _y, std::string str1);

	// getting colors and characters
	int GetFColor(int _x, int _y);
	int GetBColor(int _x, int _y);
	int GetChar(int _x, int _y);

	// input
	int	GetKey(int v_key);
	int	GetMouseCellX();
	int	GetMouseCellY();
	bool MouseArea(int x1, int y1, int x2, int y2);
	bool MouseZone(int x, int y, int width = 1, int height = 1);
	char InputChar();
	void ClearInput();

	// conversions
	std::string	IntToStr(int number);
	std::string CharToStr(char letter);

	// console settings
	void SetSize(unsigned width, unsigned height);
	void SetCursor(int _x, int _y);
	void SetMouseVisible(BOOL visible);

	// getting buffer stuff
	int GetBufHeight();
	int GetBufWidth();

private:
	CHAR_INFO* bufConsole; // pointer to the char_info array
	COORD size; // size of screen
	tagPOINT cur_point; // point where the cursor is
	HWND hWindow; // handle to the window
	HANDLE hScreen; // handle to the screen
	HANDLE hInput;  // handle to console input
	tagPOINT windowpoint; // window position
	struct mousepos_t {
		int x;
		int y;
	} mouse; // position of mouse
	CONSOLE_FONT_INFO font; // current font
	CONSOLE_SCREEN_BUFFER_INFO ScrBufInfo; // buffer info used in fullscreen
	HWND fullscr_h; // handle to the destop window
	RECT fullscr_size; // size of fullscreen
	COORD fontSize; // size of current font
	INPUT_RECORD InputRecords[256]; // buffer for InputChar
	DWORD charsRead; // chars read for InputChar
	SMALL_RECT screen_area; // area of screen

	// current level pointer
	ConsoleLevel* CLevel;

};

// constants (the colors for display)
const int C_BLACK = 0;
const int C_DKBLUE = 1;
const int C_DKGREEN = 2;
const int C_DKCYAN = 3;
const int C_DKRED = 4;
const int C_DKPURPLE = 5;
const int C_DKYELLOW = 6;
const int C_GRAY = 7;
const int C_DKGRAY = 8;
const int C_BLUE = 9;
const int C_GREEN = 10;
const int C_CYAN = 11;
const int C_RED = 12;
const int C_PURPLE = 13;
const int C_YELLOW = 14;
const int C_WHITE = 15;

#endif /* CONSOLE_H */