#include <windows.h>   // WinApi header
#include <iostream>
#include <string>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "GameObject.h"
#include "Scenery.h"

// constructor
Sign::Sign(Console &parent, float ox, float oy, std::string fname, std::string params) : 
	GameObject(parent, SCENERY, "data/objects/"+fname, ox, oy)
{
	Parent = &parent;

	sprite.SetTransparent(C_BLACK);
	frame = 0;
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();

	std::string tempstr = params;
	int pos = (int)tempstr.find_first_of('|');
	int pos2 = (int)tempstr.find_first_of('|',pos+2);
	if (pos2 == std::string::npos)
		pos2 = (int)tempstr.length();
	displaystr = tempstr.substr(0,pos);
	std::string text = tempstr.substr(pos+1,pos2-pos-1);

	// divide up the string
	width = 24;
	int lastSpace = 0;
	int lastBreak = 0;
	int cIndex = 0;
	int letterCount = 0;
	for(std::string::iterator cPos = text.begin(); cPos != text.end(); ++cPos)
	{
		if (*cPos == ' ')
			lastSpace = cIndex;
		if (letterCount > width)
		{
			letterCount = 0;
			Textstr.push_back(text.substr(lastBreak,lastSpace-lastBreak));
			lastBreak = lastSpace+1;
		}
		++letterCount;
		++cIndex;
	}
	Textstr.push_back(text.substr(lastBreak,text.length()-1));
}

// step event
State Sign::Step()
{
	contact = false;
	return ACTIVE;
}

// draw to the buffer
void Sign::Draw(int offx, int offy)
{
	// draw the sprite
	sprite.Draw((int)frame,(int)x-offx,(int)y-offy);
	Parent->WriteStr((int)x-offx+2,(int)y-offy+1,displaystr);

	if (contact)
	{
		int xm = (int)x-offx+sprite.GetW()/2;
		int ym = (int)(y-offy+sprite.GetH()/2-Textstr.size()*1.5-2);
		Parent->SetColor(xm-width/2-2,ym-(int)Textstr.size()/2-1,C_BLACK,C_DKCYAN,
			width+4,(int)Textstr.size()+2);
		Parent->WriteChar(xm-width/2-2,ym-(int)Textstr.size()/2-1,' ',
			width+4,(int)Textstr.size()+2);
		Parent->WriteChar(xm-width/2-2,ym-(int)Textstr.size()/2-1,'\xB1',
			width+4);
		Parent->WriteChar(xm-width/2-2,ym+(int)Textstr.size()/2-
			((int)Textstr.size()%2 == 0)+1,'\xB1',width+4);

		// display the sign's text
		for(int tCount = 0; tCount < (int)Textstr.size(); ++tCount)
			Parent->WriteStr(xm-width/2,ym-(int)Textstr.size()/2+tCount,Textstr.at(tCount));
	}
}

// collide with an object
void Sign::Collide(GameObject *other)
{
	if (other->GetType() == PLAYER)
		contact = true;
	else
		contact = false;
}