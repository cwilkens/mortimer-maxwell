#include <windows.h>   // WinApi header
#include <iostream>
#include <string>
#include <sstream>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "GameObject.h"
#include "Items.h"

//--------------------------------------------------------------------------------
// --- Item ---
//--------------------------------------------------------------------------------
// constructor
Item::Item(Console &parent, float ox, float oy, std::string fname, std::string params) : 
		GameObject(parent, ITEM, "data/objects/"+fname, ox, oy)
{
	Parent = &parent;

	sprite.SetTransparent(C_BLACK);
	frame = 0;
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();
	contact = false;
	name = fname.substr(0,(int)fname.length()-4);

	if (name == "healthitem")
	{
		std::string tempstr = params;
		int pos = (int)tempstr.find_first_of('|');
		int pos2 = (int)tempstr.find_first_of('|',pos+2);
		if (pos2 == std::string::npos)
			pos2 = (int)tempstr.length();
		std::stringstream (tempstr.substr(0,pos)) >> value;
		//std::string text = tempstr.substr(pos+1,pos2-pos-1);
	}

}

// step event
State Item::Step()
{
	if (contact == false)
		return ACTIVE;
	else
		return REMOVE;
}

// draw to the buffer
void Item::Draw(int offx, int offy)
{
	// draw the sprite
	if (name == "healthitem" && value != 0)
		frame = 1;
	sprite.Draw((int)frame,(int)x-offx,(int)y-offy);
}

// collide with an object
void Item::Collide(GameObject *other)
{
	if (other->GetType() == PLAYER)
	{
		contact = true;
		if (name == "healthitem")
		{
			other->Damage(this,-value-1000*(value == 0));
		}
	}
}

//--------------------------------------------------------------------------------
// --- Powerup ---
//--------------------------------------------------------------------------------
// constructor
Powerup::Powerup(Console &parent, float ox, float oy, std::string fname, std::string params) : 
		GameObject(parent, ITEM, "data/objects/"+fname, ox, oy)
{
	Parent = &parent;

	sprite.SetTransparent(C_BLACK);
	frame = 0;
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();
	contact = false;

	std::string tempstr = params;
	int pos = (int)tempstr.find_first_of('|');
	int pos2 = (int)tempstr.find_first_of('|',pos+2);
	if (pos2 == std::string::npos)
		pos2 = (int)tempstr.length();
	std::stringstream (tempstr.substr(0,pos)) >> type;

}

// step event
State Powerup::Step()
{
	if (contact == false)
		return ACTIVE;
	else
		return REMOVE;
}

// draw to the buffer
void Powerup::Draw(int offx, int offy)
{
	// draw the sprite
	sprite.Draw((int)frame,(int)x-offx,(int)y-offy);
}

// collide with an object
void Powerup::Collide(GameObject *other)
{
	if (other->GetType() == PLAYER)
	{
		contact = true;
		other->Damage(this,type);
	}
}