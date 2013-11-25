#include <windows.h>   // WinApi header
#include <string>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "ObjectList.h"
#include "GameObject.h"
#include "Enemy.h"

// constructor
Enemy::Enemy(Console &parent, float ox, float oy, std::string fname) : 
	GameObject(parent, ENEMY, "data/objects/"+fname, ox, oy)
{
	Parent = &parent;
	// get the current level
	Level = Parent->GetLevel();

	// set transparency
	sprite.SetTransparent(C_BLACK);

	if ((fname == "bluechair.dat") || (fname == "redchair.dat"))
	{
		std::vector<int> tmp;
		// StartWalking
		tmp.resize(7);
		tmp[0] = 0;
		tmp[1] = 1;
		tmp[2] = 2;
		tmp[3] = 1;
		tmp[4] = 2;
		tmp[5] = 1;
		tmp[6] = 3;
		Frames.push_back(tmp);
		// Walking
		tmp.resize(3);
		tmp[0] = 4;
		tmp[1] = 5;
		tmp[2] = 6;
		Frames.push_back(tmp);
	}

	// set some basic variables
	mode = 0;
	frame = 0;
	framec = 0;
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();
	health = 2;
	dmgcounter = 0;
	xspeed = 0;
	dirleft = true;
}

// step event
State Enemy::Step()
{
	dmgcounter -= (dmgcounter > 0);
	fspeed = 0.33f;

	// gravity
	if (!(Level->GetSolidRow((int)y+h,(int)x+1,(int)x+w-2,L_SOLID) || 
		Level->GetSolidRow((int)y+h,(int)x+1,(int)x+w-2,L_DOWNONLY)))
		y += 1;
	else // on the ground
	{
		GameObject* objP = Controller->Nearest(PLAYER, x);
		if (objP != NULL)
		{
			int dist = (int)objP->GetX()-(int)x;
			if (abs(dist) < 40) //player in range
			{
				mode = 1;
				xspeed = dist/(abs(dist)+(dist == 0));
				//
				if (dist == 0)
					mode = -1;
				//
			}
			else
				mode = 0;
		}
		else
		{
			mode = -1;
			xspeed = 0;
		}
	}

	if (xspeed != 0)
		dirleft = (xspeed < 0);
	x += xspeed;

	if (Level->GetSolidRow((int)y+h-1,(int)x+1,(int)x+w-2,L_SOLID) && 
		!Level->GetSolidRow((int)y+h-2,(int)x+1,(int)x+w-2,L_SOLID))
		y -= 1;

	// stuff here
	if (health > 0)
		return ACTIVE;
	else
		return REMOVE;
}

// draw to the buffer
void Enemy::Draw(int offx, int offy)
{
	// draw the sprite
	if (mode != -1)
	{
		framec += fspeed;
		if ((int)framec > (int)Frames.at(mode).size()-1)
			framec = 0;
		frame = (float)Frames.at(mode).at((int)framec);
		// DrawFlip
		sprite.Draw(Frames.at(mode).at((int)framec),(int)x-offx,(int)y-offy,!dirleft);
	}
	else
	{
		frame = 0;
		sprite.Draw(0,(int)x-offx,(int)y-offy,!dirleft);
	}
}

// collide event
void Enemy::Collide(GameObject* other)
{
	if (other->GetType() == PLAYER)
		other->Damage(this,1);
}

// damage event
void Enemy::Damage(GameObject *cause, int dmg)
{
	if (dmgcounter == 0)
	{
		health -= dmg;
		dmgcounter = 12;
	}
}