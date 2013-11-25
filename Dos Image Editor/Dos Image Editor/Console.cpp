#define _WIN32_WINNT 0x0550

#include <windows.h>   // WinApi header
#include <iostream>
#include <sstream>
#include <vector>
#include "Console.h"


Console::Console(int width, int height)
{
	// get the handles
	hWindow = GetConsoleWindow();
	hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	hInput = GetStdHandle(STD_INPUT_HANDLE);

	screen_area.Left = 0;
	screen_area.Top = 0;
	screen_area.Right = width-1;
	screen_area.Bottom = height-1;
	size.X = width;
	size.Y = height;

	mouse.x = 0;
	mouse.y = 0;

	// set the screen size
	SetConsoleWindowInfo(hScreen, true, &screen_area);
	SetConsoleScreenBufferSize(hScreen, size);

	// set the buffer
	bufConsole = new CHAR_INFO[size.X*size.Y];

	// clear the buffer
	for(int bCount = 0; bCount < size.X*size.Y; ++bCount)
	{
		bufConsole[bCount].Char.AsciiChar = (char)0;
		bufConsole[bCount].Attributes = C_GRAY+C_BLACK*16;
	}

	charsRead = 0;
}

Console::~Console()
{
	delete[] bufConsole;
}

// set the current level pointer
void Console::SetLevel(ConsoleLevel* level)
{
	CLevel = level;
}

// get the current level pointer
ConsoleLevel* Console::GetLevel()
{
	return CLevel;
}

// copies this object's buffer to the screen
void Console::DrawBuffer()
{
	COORD zero = {0,0};
	WriteConsoleOutputA(hScreen, bufConsole, size, zero, &screen_area);
}

// fills the buffer with a solid color and no characters
void Console::ClearBuffer(int color2, int color1)
{
	// clear the buffer
	for(int bCount = 0; bCount < size.X*size.Y; ++bCount)
	{
		bufConsole[bCount].Char.AsciiChar = 0;
		bufConsole[bCount].Attributes = color2*16+color1;
	}
}

// fills the screen (not the buffer) with a solid color and no chars
void Console::FillScreen(int color2, int color1)
{
	SetConsoleTextAttribute(hScreen, color2*16 + color1);
}


// sets the colors in an area
void Console::SetColor(int _x, int _y, int color1, int color2, int width, int height)
{
	int spot;
	for(int yCount = 0; yCount < height; ++yCount)
	{
		if ((_y+yCount < size.Y) && (_y+yCount >= 0))
		{
			for(int xCount = 0; xCount < width; ++xCount)
			{
				if ((_x+xCount < size.X) && (_x+xCount >= 0))
				{
					// calculate the spot and the color
					spot = size.X*_y + size.X*yCount;
					bufConsole[spot+_x+xCount].Attributes = color1 + color2*16;
				}
			}
		}
	}
	// end of the for loops
}

// sets the colors in one cell
void Console::SetColor(int _x, int _y, int colors)
{
	bufConsole[size.X*_y+_x].Attributes = colors;
}

// sets the foreground color in one cell
void Console::SetFColor(int _x, int _y, int color)
{
	bufConsole[size.X*_y+_x].Attributes = color + (bufConsole[size.X*_y+_x].Attributes/16)*16;
}

// displays a character at x,y with the colors at that spot, repeating if set
void Console::WriteChar(int _x, int _y, char char1, int hrepeat, int vrepeat)
{
	int spot;
	for(int yCount = 0; yCount < vrepeat; ++yCount)
	{
		if ((_y+yCount < size.Y) && (_y+yCount >= 0))
		{
			for(int xCount = 0; xCount < hrepeat; ++xCount)
			{
				if ((_x+xCount < size.X) && (_x+xCount >= 0))
				{
					// calculate the spot and place the char
					spot = size.X*_y + size.X*yCount;
					bufConsole[spot+_x+xCount].Char.AsciiChar = char1;
				}
			}
		}
	}
}

// displays a string at x,y with the colors in those spots
void Console::WriteStr(int _x, int _y, std::string str1)
{
	// go through each character
	for(unsigned int sCount = 0; sCount < str1.length(); ++sCount)
	{
		// put in the cell
		bufConsole[size.X*_y+_x+sCount].Char.AsciiChar = str1[sCount];
	}
}

// gets the foreground color of a spot on the screen
int Console::GetFColor(int _x, int _y)
{
	return (bufConsole[size.X*_y+_x].Attributes % 16);
}

// gets the background color of a spot on the screen
int Console::GetBColor(int _x, int _y)
{
	return (bufConsole[size.X*_y+_x].Attributes / 16);
}

// gets the character at a spot on the screen
int Console::GetChar(int _x, int _y)
{
	return bufConsole[size.X*_y+_x].Char.AsciiChar;
}

// gets the status of the key, 1 down, 0 up
int Console::GetKey(int v_key)
{
	return ((GetAsyncKeyState(v_key) & 0x8000) == 0x8000);
}

// gets the x coordinate of the cell the mouse is in
int Console::GetMouseCellX()
{
	GetCursorPos(&cur_point);
	windowpoint.x = 0;
	windowpoint.y = 0;
	ClientToScreen(hWindow,&windowpoint);
	GetCurrentConsoleFont(hScreen, false, &font);
	fontSize = GetConsoleFontSize(hScreen, font.nFont);
	if (fontSize.X != 0)
		mouse.x = (cur_point.x - windowpoint.x)/fontSize.X;
	else
	{
		// else it is in fullscreen
		GetConsoleScreenBufferInfo(hScreen,&ScrBufInfo);
		// get the window handle to the whole screen
		fullscr_h = GetDesktopWindow();
		// get the size of the whole screen
		GetClientRect(fullscr_h, &fullscr_size);
		mouse.x = cur_point.x/((fullscr_size.right/ScrBufInfo.dwSize.X)-4);
	}
	return mouse.x;

}

// gets the y coordinate of the cell the mouse is in
int Console::GetMouseCellY()
{
	GetCursorPos(&cur_point);
	windowpoint.x = 0;
	windowpoint.y = 0;
	ClientToScreen(hWindow,&windowpoint);
	GetCurrentConsoleFont(hScreen, false, &font);
	fontSize = GetConsoleFontSize(hScreen, font.nFont);
	if (fontSize.Y != 0)
		mouse.y = (cur_point.y - windowpoint.y)/fontSize.Y;
	else
	{
		// else it is in fullscreen
		GetConsoleScreenBufferInfo(hScreen,&ScrBufInfo);
		// get the window handle to the whole screen
		fullscr_h = GetDesktopWindow();
		// get the size of the whole screen
		GetClientRect(fullscr_h, &fullscr_size);
		mouse.y = cur_point.y/((fullscr_size.bottom/ScrBufInfo.dwSize.Y)-3);
	}
	return mouse.y;
}

// returns if the mouse is in the given area
bool Console::MouseArea(int x1, int y1, int x2, int y2)
{
	if ((mouse.x >= x1) && (mouse.x <= x2) && (mouse.y >= y1) && (mouse.y <= y2))
		return true;
	else
		return false;
}

// clears the input buffer for the console
void Console::ClearInput()
{
	FlushConsoleInputBuffer(hInput);
}

// returns a char of the current key pressed
char Console::InputChar()
{
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT);
	// check to see if there are any input records
	GetNumberOfConsoleInputEvents(hInput,&charsRead);
	// get input
	if (charsRead > 0)
	{
		ReadConsoleInput(hInput,&InputRecords[0],256,&charsRead);
		for(WORD Count = 0; Count < charsRead; ++Count)
		{
			if (InputRecords[Count].EventType == KEY_EVENT)
			{
				if (InputRecords[Count].Event.KeyEvent.bKeyDown == TRUE)
					return InputRecords[Count].Event.KeyEvent.uChar.AsciiChar;
			}
		}
		// if we didn't get anything
		return 0;
	}
	else
		return 0;
}

// sets the size of the window, if the width (not height) is 
// wider than the user's settings there will be scroll bars
void Console::SetSize(unsigned width, unsigned height)
{
	screen_area.Left = 0;
	screen_area.Top = 0;
	screen_area.Right = width-1;
	screen_area.Bottom = height-1;
	size.X = width;
	size.Y = height;

	// set the screen size
	SetConsoleWindowInfo(hScreen, true, &screen_area);
	SetConsoleScreenBufferSize(hScreen, size);

	// set the buffers
	delete[] bufConsole;
	bufConsole = new CHAR_INFO[size.X*size.Y];
}

// enables or disables underscore (the console cursor) visibility
void Console::SetMouseVisible(BOOL visible)
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 1;
	cursor.bVisible = visible;
	SetConsoleCursorInfo(hScreen, &cursor);
}

// sets the position of the console cursor
void Console::SetCursor(int _x, int _y)
{
	COORD cursor_p = {_x, _y};
	SetConsoleCursorPosition(hScreen, cursor_p);
}

// converts an integer to a string
std::string Console::IntToStr(int number)
{
	std::ostringstream outstr;
	outstr << number;
	return outstr.str();
}

// converts a char to a string
std::string Console::CharToStr(char letter)
{
	std::ostringstream outstr;
	outstr << letter;
	return outstr.str();
}

// returns the size of the buffer
int Console::GetBufWidth()
{
	return size.X;
}

// returns the size of the buffer
int Console::GetBufHeight()
{
	return size.Y;
}