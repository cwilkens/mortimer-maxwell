// cpp for textbox.h

#include <windows.h>
#include <string>
#include <algorithm>
#include "Console.h"
#include "TextBox.h"

TextBox::TextBox(Console &parent, std::string text, bool textinput, 
				 std::string yes_btn, std::string no_btn)
{
	Parent = &parent;
	Yes = yes_btn;
	No = no_btn;
	TextIn = textinput;
	TextStr = "";
	textlength = 24;

	// divide up the string
	int lastSpace = 0;
	int lastBreak = 0;
	int cIndex = 0;
	int letterCount = 0;
	for(std::string::iterator cPos = text.begin(); cPos != text.end(); ++cPos)
	{
		if (*cPos == ' ')
			lastSpace = cIndex;
		if (letterCount > 24)
		{
			letterCount = 0;
			Text.push_back(text.substr(lastBreak,lastSpace-lastBreak));
			lastBreak = lastSpace+1;
		}
		++letterCount;
		++cIndex;
	}
	Text.push_back(text.substr(lastBreak,text.length()-1));
	Parent->ClearInput();
}

// activate the text box (display it and get stuff)
int TextBox::Activate()
{
	char tmpchar;
	int buttonreturn = 0;

	RECT location;
	location.bottom = Parent->GetBufHeight()/2+7;
	location.left = Parent->GetBufWidth()/2-15;
	location.right = Parent->GetBufWidth()/2+14;
	location.top = Parent->GetBufHeight()/2-8;

	// draw the box and buttons
	Parent->SetColor(location.left,location.top,C_WHITE,C_BLUE,30,16);
	Parent->WriteChar(location.left,location.top,' ',30,16);
	DrawRect(location.left,location.top,location.right,location.bottom);
	DrawRect(location.left+3,location.bottom-5,
			 location.left+(location.right-location.left)/2-1,
			 location.bottom-3);
	DrawRect(location.left+(location.right-location.left)/2+2,
			 location.bottom-5,location.right-3,location.bottom-3);
	Parent->WriteStr(location.left+4,location.bottom-4,Yes);
	Parent->WriteStr(location.left+(location.right-location.left)/2+3,location.bottom-4,No);

	//draw the text
	for(unsigned int vCount = 0; vCount < Text.size(); ++vCount)
	{
		Parent->WriteStr(location.left+3,location.top+2+vCount,Text.at(vCount));
	}

	// if we were told to get text input
	if (TextIn == true)
	{
		// check for letters
		tmpchar = Parent->InputChar();
		if (tmpchar)
		{
			// backspace
			if (tmpchar == 8)
			{
				if (TextStr.length() > 0)
					TextStr.erase(TextStr.end()-1);
			}
			else if (tmpchar == 13) // enter
			{
				buttonreturn = 1;
			}
			else if (tmpchar > 31) // normal letters
			{
				std::string cstr = Parent->CharToStr(tmpchar);
				TextStr.append(cstr);
			}
		}

		// draw the text string
		Parent->SetColor(location.left+3,location.top+7,C_WHITE,C_DKBLUE,24);

		int tmplength = min((int)TextStr.length(),textlength-1);
		std::string stmp(TextStr,TextStr.length()-tmplength,TextStr.length());
		Parent->WriteStr(location.left+3,location.top+7,stmp);

		Parent->SetMouseVisible(true);
		Parent->SetCursor(location.left+3+min((int)TextStr.length(),textlength-1),
			location.top+7);

	}
	// check for clicking/hovering on buttons
	if (Parent->MouseArea(location.left+3,location.bottom-5,location.left+
		(location.right-location.left)/2-1,location.bottom-3) == true)
	{
		if (Parent->GetKey(VK_LBUTTON))
			buttonreturn = 1;
		Parent->SetColor(location.left+3,location.bottom-5,C_WHITE,C_DKBLUE,location.left+
			(location.right-location.left)/2-location.left-3,3);
	}
	if (Parent->MouseArea(location.left+(location.right-location.left)/2+2,
		location.bottom-5,location.right-3,location.bottom-3) == true)
	{
		if (Parent->GetKey(VK_LBUTTON))
			buttonreturn = 2;
		Parent->SetColor(location.left+(location.right-location.left)/2+2,location.bottom-5,
			C_WHITE,C_DKBLUE,location.right-location.left-(location.right-location.left)/2-4,3);
	}

	return buttonreturn;
}

// get the string in the input box
std::string TextBox::GetText()
{
	return TextStr;
}

// draws a rectangle with double lines
void TextBox::DrawRect(int left, int top, int right, int bottom)
{
	// edges
	Parent->WriteChar(left,top+1,'\xBA',1,bottom-top-1);
	Parent->WriteChar(right,top+1,'\xBA',1,bottom-top-1);
	Parent->WriteChar(left+1,top,'\xCD',right-left-1);
	Parent->WriteChar(left+1,bottom,'\xCD',right-left-1);
	// corners
	Parent->WriteChar(left,top,'\xC9');
	Parent->WriteChar(left,bottom,'\xC8');
	Parent->WriteChar(right,top,'\xBB');
	Parent->WriteChar(right,bottom,'\xBC');
}