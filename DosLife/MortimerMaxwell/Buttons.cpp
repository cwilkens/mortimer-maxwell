#include <windows.h>   // WinApi header
#include <iostream>
#include <string>
#include "Console.h"
#include "ConsoleImage.h"
#include "GameObject.h"
#include "Buttons.h"

// constructor
Button::Button(Console &parent, float ox, float oy, std::string displaytext) : 
	GameObject(parent, SCENERY, "data/button.dat", ox, oy)
{
	Parent = &parent;

	sprite.SetTransparent(C_BLACK);
	frame = 1;
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();
	text = displaytext;
	contact = false;
}

// step event
State Button::Step()
{
	int mx = Parent->GetMouseCellX() - (int)x;
	int my = Parent->GetMouseCellY() - (int)y;
	contact = false;
	if ((mx >= 0) && (my >= 0) && (mx < sprite.GetW()) && (my < sprite.GetH()))
	{
		if (sprite.GetBColor((int)contact,mx,my) != C_BLACK)
			contact = true;
		else if ((sprite.GetChar((int)!contact,mx,my) != 0) &&
			(sprite.GetChar((int)!contact,mx,my) != 32))
			contact = true;
	}

	button_down = false;
	if ((contact == true) && Parent->GetKey(VK_LBUTTON))
		button_down = true;

	frame = contact;
	return ACTIVE;
}

// draw to the buffer
void Button::Draw(int offx, int offy)
{
	// draw the sprite
	sprite.Draw((int)frame,(int)x-offx,(int)y-offy);
	Parent->WriteStr((int)x-offx+sprite.GetW()/2-(int)text.length()/2,(int)y-offy+1,text);
}

// returns the status of the button
int Button::Pressed()
{
	return button_down;
}