// cpp for objectbox.h

#include <windows.h>
#include <string>
#include <algorithm>
#include "../MortimerMaxwell/Console.h"
#include "ObjectBox.h"

ObjectBox::ObjectBox(Console &parent)
{
	Parent = &parent;
	std::string text = "Choose an object and set the parameters, then place it in the level.";
	textlength = 24;
	cObject = 0;

	// list of objects
	ObjectTypes.push_back("blue chair");
	ObjectTypes.push_back("red chair");
	//ObjectTypes.push_back("dining table");
	//ObjectTypes.push_back("card table");
	//ObjectTypes.push_back("desk");
	ObjectTypes.push_back("sign");
	ObjectTypes.push_back("health item");

	// type of objects
	ObjTypes.push_back("sign");
	ObjTypes.push_back("enemy");
	ObjTypes.push_back("item");
	
	
	// load images for the object list
	for(int oCount = 0; (unsigned)oCount < ObjectTypes.size(); ++oCount)
	{
		std::string file = ObjectTypes.at(oCount);
		while (file.find(' ',0) != std::string::npos) {
			file.erase(file.find(' ',0),1);
		}
		file.append(".dat");
		// load it
		ConsoleImage* tmpImage = new ConsoleImage(*Parent, "data/objects/"+file);
		// create another image that cuts it (in the case it's too big) and put it in the vector
		Images.push_back(new ConsoleImage(*Parent, tmpImage,0,0,0,textlength+1,20));
	}
	
	for(int C = 0; C < 4; ++C)
		params.push_back("");

	focus = 0;
	cObject = 0;
	cType = 0;
	leftkey = false;
	rightkey = false;
	upkey = false;
	downkey = false;

	// divide up the string
	int lastSpace = 0;
	int lastBreak = 0;
	int cIndex = 0;
	unsigned int letterCount = 0;
	for(std::string::iterator cPos = text.begin(); cPos != text.end(); ++cPos)
	{
		if (*cPos == ' ')
			lastSpace = cIndex;
		if (letterCount > textlength)
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

// code for step event (display it and get stuff)
int ObjectBox::Step()
{
	char tmpchar;
	int buttonreturn = 0;

	RECT location;
	location.bottom = 34;
	location.left = 9;
	location.right = 69;
	location.top = 4;

	// draw the box and buttons
	Parent->SetColor(location.left,location.top,C_WHITE,C_BLUE,61,31);
	Parent->WriteChar(location.left,location.top,' ',61,31);
	DrawRect(location.left,location.top,location.right,location.bottom);
	DrawRect((location.right+location.left)/2,location.bottom-4,
			 (location.right+location.left)/2+
			 (location.right-location.left)/4-3, location.bottom-2);
	DrawRect((location.right+location.left)/2+
			 (location.right-location.left)/4-1,location.bottom-4,
			 location.right-3,location.bottom-2);
	Parent->WriteStr((location.right+location.left)/2+1,location.bottom-3,"OK");
	Parent->WriteStr((location.right+location.left)/2+
					 (location.right-location.left)/4,location.bottom-3,"Cancel");
	// draw the input fields
	Parent->WriteStr(location.left+3,location.top+7,"Param 1:");
	Parent->SetColor(location.left+3,location.top+8,C_WHITE,C_DKBLUE,textlength);
	Parent->WriteStr(location.left+3,location.top+10,"Param 2:");
	Parent->SetColor(location.left+3,location.top+11,C_WHITE,C_DKBLUE,textlength);
	Parent->WriteStr(location.left+3,location.top+13,"Param 3:");
	Parent->SetColor(location.left+3,location.top+14,C_WHITE,C_DKBLUE,textlength);
	Parent->WriteStr(location.left+3,location.top+16,"Param 4:");
	Parent->SetColor(location.left+3,location.top+17,C_WHITE,C_DKBLUE,textlength);
	// draw the object and selection menu
	Parent->WriteStr((location.right+location.left)/2+1,location.top+2,"Arrow keys to change.");
	if (Parent->GetKey(VK_LEFT))
	{
		if (leftkey == false)
		{
			if (cObject > 0)
				cObject -= 1;
		}
		leftkey = true;
	} else
		leftkey = false;

	if (Parent->GetKey(VK_RIGHT))
	{
		if (rightkey == false)
		{
			if ((unsigned)cObject < ObjectTypes.size()-1)
				cObject += 1;
		}
		rightkey = true;
	} else
		rightkey = false;

	if (Parent->GetKey(VK_UP))
	{
		if (upkey == false)
		{
			if (cType > 0)
				cType -= 1;
		}
		upkey = true;
	} else
		upkey = false;

	if (Parent->GetKey(VK_DOWN))
	{
		if (downkey == false)
		{
			if ((unsigned)cType < ObjTypes.size()-1)
				cType += 1;
		}
		downkey = true;
	} else
		downkey = false;


	Images.at(cObject)->Draw(0,(location.right+location.left)/2+1,location.top+3);
	Parent->WriteStr((location.right+location.left)/2+1,
		location.bottom-6,ObjectTypes.at(cObject));
	Parent->WriteStr(location.right-3-(int)ObjTypes.at(cType).length(),
		location.bottom-6,ObjTypes.at(cType));

	//draw the text
	for(unsigned int vCount = 0; vCount < Text.size(); ++vCount)
	{
		Parent->WriteStr(location.left+3,location.top+2+vCount,Text.at(vCount));
	}

	// check for letters
	tmpchar = Parent->InputChar();
	if (tmpchar)
	{
		// backspace
		if (tmpchar == 8)
		{
			if (params.at(focus).length() > 0)
				params.at(focus).erase(params.at(focus).end()-1);
		}
		else if (tmpchar == 13) // enter
		{
			focus += 1;
			focus *= (focus < 4);
		}
		else if (tmpchar > 31) // normal letters
		{
			std::string cstr = Parent->CharToStr(tmpchar);
			params.at(focus).append(cstr);
		}
	}

	// draw the text and number params
	Parent->SetMouseVisible(true);
	Parent->SetCursor(location.left+3+min((int)params.at(focus).length(),textlength-1),
		location.top+8+focus*3);
	int tmplength;
	for(int pCount = 0; pCount < 4; ++pCount)
	{
		tmplength = min((int)params.at(pCount).length(),textlength-1);
		std::string stmp(params.at(pCount),params.at(pCount).length()-tmplength,
			params.at(pCount).length());
		Parent->WriteStr(location.left+3,location.top+8+3*pCount,stmp);
	}

	// check for clicking in the input fields
	if (Parent->GetKey(VK_LBUTTON))
	{
		for(int pCount = 0; pCount < 4; ++pCount)
		{
			if (Parent->MouseZone(location.left+3,location.top+8+3*pCount,textlength))
				focus = pCount;
		}
	}

	// check for clicking/hovering on buttons
	if (Parent->MouseArea((location.right+location.left)/2,location.bottom-4,
						  (location.right+location.left)/2+
						  (location.right-location.left)/4-3,location.bottom-2) == true)
	{
		if (Parent->GetKey(VK_LBUTTON))
			buttonreturn = 1;
		Parent->SetColor((location.right+location.left)/2,location.bottom-4,
			C_WHITE,C_DKBLUE,(location.right+location.left)/2+
			(location.right-location.left)/4-2-(location.right+location.left)/2,3);
	}
	if (Parent->MouseArea((location.right+location.left)/2+
						  (location.right-location.left)/4-1,location.bottom-4,
						  location.right-3,location.bottom-2) == true)
	{
		if (Parent->GetKey(VK_LBUTTON))
			buttonreturn = 2;
		Parent->SetColor((location.right+location.left)/2+
			(location.right-location.left)/4-1,location.bottom-4,C_WHITE,C_DKBLUE,
			location.right-((location.right+location.left)/2+
			(location.right-location.left)/4)-1,3);
	}

	return buttonreturn;
}

// returns the type of the object
std::string ObjectBox::GetObjType()
{
	return ObjTypes.at(cType);
}

// returns the name of the object
std::string ObjectBox::GetObjectName()
{
	return ObjectTypes.at(cObject);
}

// returns all of the parameters
std::string ObjectBox::GetParams()
{
	return params.at(0)+"|"+params.at(1)+"|"+params.at(2)+"|"+params.at(3);
}

// returns the size of the current object
COORD ObjectBox::GetObjectSize()
{
	std::string file = ObjectTypes.at(cObject);
	while (file.find(' ',0) != std::string::npos)
		file.erase(file.find(' ',0),1);
	
	file.append(".dat");
	// load it
	ConsoleImage* tmpImage = new ConsoleImage(*Parent, "data/objects/"+file);
	COORD size;
	size.X = tmpImage->GetW();
	size.Y = tmpImage->GetH();
	return size;
}

// draws a rectangle with double lines
void ObjectBox::DrawRect(int left, int top, int right, int bottom)
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