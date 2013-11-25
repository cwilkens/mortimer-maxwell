#include <windows.h>   // WinApi header
#include <fstream>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "ObjectList.h"
#include "Player.h"
#include "Projectiles.h"


// constructor
Player::Player(float ox, float oy, Console &parent) : 
	GameObject(parent, PLAYER, "data/mallardsmall.dat", ox, oy)
{
	Parent = &parent;
	// get the current level
	Level = Parent->GetLevel();

	// set transparency
	sprite.SetTransparent(C_BLACK);
	// set default facing direction
	left = 0;

	// set some basic variables
	x = ox;
	y = oy;
	w = sprite.GetW();
	h = sprite.GetH();
	jump = -1;
	jcounter = 0;
	frame = 0;
	framec = 0;
	dmgcounter = 0;

	// game variables
	maxhealth = 6;
	shoottime = 20;
	shootspeed = 2.0f;
	shootdist = 60;
	shootinvince = false;

	health = maxhealth;
	shootc = 0;
}

// step event
State Player::Step()
{
	// check for keypresses
	int leftkey = Parent->GetKey(VK_LEFT);
	int rightkey = Parent->GetKey(VK_RIGHT);

	// update counters
	dmgcounter -= (dmgcounter > 0);
	dmgcounter += (dmgcounter < 0);
	shootc -= (shootc > 0);

	// jumping
	if (Parent->GetKey(VK_UP))
	{
		if (Level->GetSolidRow((int)y+h,(int)x+1,(int)x+w-2,1) && (jcounter == 0))
		{
			jcounter = 5;
			jump = 3;
		}
	}

	// if we are jumping
	if (jump >= 0)
	{
		int tjump = jump;
		tjump += (tjump == 0);

		for(int jCount = tjump; jCount >= 0; --jCount)
		{
			if (Level->GetSolidRow((int)y-jCount,(int)x+1,(int)x+w-2,1) ||
				Level->GetSolidRow((int)y-jCount,(int)x+1,(int)x+w-2,3))
				tjump = jCount;
		}
		y -= tjump;

		if (jump != 0)
			jump = tjump-1;
		else
			jump = -1;
	}
	else if (jcounter > 0)
		jcounter -= 1;

	// movement
	if (rightkey)
	{
		left = 0;
		if (!(Level->GetSolidCol((int)x+w-1,(int)y,(int)y+h-1,1) ||
			Level->GetSolidCol((int)x+w-1,(int)y,(int)y+h-1,4)))
			x += 1;
	}

	if (leftkey)
	{
		left = 1;
		if (!(Level->GetSolidCol((int)x,(int)y,(int)y+h-1,1) ||
			Level->GetSolidCol((int)x,(int)y,(int)y+h-1,5)))
			x -= 1;
	}

	// if either was pressed, but not both
	if ((leftkey || rightkey) && !(leftkey && rightkey))
	{
		// change the walking animation's frame
		framec += .5;
		if (framec == 2)
			framec = 0;
		frame = left*2+framec;
	}

	// gravity
	if (!(Level->GetSolidRow((int)y+h,(int)x+1,(int)x+w-2,1) || 
		Level->GetSolidRow((int)y+h,(int)x+1,(int)x+w-2,2)))
	{
		frame = (float)left*2;
		y += 1;
	}

	if (Parent->GetKey(VK_SPACE) && shootc == 0)
	{
		shootc = shoottime;
		SonicWave* tmp = new SonicWave(*Parent,x-4+(sprite.GetW()+7)*(!left),y,
			(left != 0),shootspeed,shootdist,shootinvince);
		Controller->AddObject(*tmp);
	}

	// stuff here
	if (health > 0)
		return ACTIVE;
	else
		return REMOVE;
}

// draw to the buffer
void Player::Draw(int offx, int offy)
{
	// draw the awesome sprite
	if (dmgcounter > 8 || dmgcounter < -8)
	{
		sprite.InsertFrame(sprite.GetFrames()-1,C_BLUE,sprite.GetTransparent());
		sprite.CopyFrame((int)frame,sprite.GetFrames()-1);
		if (dmgcounter > 0)
			sprite.Colorize(sprite.GetFrames()-1,C_RED);
		else
			sprite.Colorize(sprite.GetFrames()-1,C_GREEN);
		sprite.Draw(sprite.GetFrames()-1,(int)x-offx,(int)y-offy);
		sprite.RemoveFrame(sprite.GetFrames()-1);
	}
	else
		sprite.Draw((int)frame,(int)x-offx,(int)y-offy);

	// health bar
	Parent->SetColor(2,1,C_WHITE,C_DKBLUE,maxhealth+2,2);
	Parent->SetColor(3,1,C_RED,C_DKBLUE,maxhealth);
	Parent->WriteChar(2,1,' ',maxhealth+2,2);
	Parent->WriteChar(3,2,'\xCD',maxhealth);
	Parent->WriteChar(2,1,'\xC9');
	Parent->WriteChar(2,2,'\xC8');
	Parent->WriteChar(3+maxhealth,1,'\xBB');
	Parent->WriteChar(3+maxhealth,2,'\xBC');
	for(int c = 0; c < health; ++c)
		Parent->WriteChar(3+c,1,'\x03');
}

// damaged
void Player::Damage(GameObject *cause, int dmg)
{
	if (cause->GetType() != POWERUP)
	{
		if (dmgcounter == 0)
		{
			health -= dmg;
			health *= (health >= 0);
			if (health > maxhealth)
				health = maxhealth;
			if (dmg > 0)
				dmgcounter = 12;
			else
				dmgcounter = -12;
		}
	} else {
		switch (dmg)
		{
		case 0:	maxhealth += 1;			break;
		case 1:	shoottime -= 5;			break;
		case 2: shootspeed += 0.5;		break;
		case 3: shootdist += 20;		break;
		case 4: maxhealth += 1;			break;
		case 5:	shootinvince = true;	break;
		}
	}
}

// saves the player's values to a file
void Player::Save(std::string filename, std::string level)
{
	std::ofstream file;
	file.open(filename.c_str(),std::ofstream::trunc | std::ofstream::binary);
	if (file.is_open())
	{
		file.clear();
		file << maxhealth << '\n';
		file << shoottime << '\n';
		file << shootspeed << '\n';
		file << shootdist << '\n';
		file << shootinvince << '\n';
		file << level.c_str() << '\n';
	}
	file.close();
}

// loads the player's values from a file
std::string Player::Load(std::string filename)
{
	std::string level = "";
	std::ifstream file;
	file.open(filename.c_str(),std::fstream::in | std::fstream::binary);
	if (file.is_open())
	{
		file >> maxhealth;
		file >> shoottime;
		file >> shootspeed;
		file >> shootdist;
		file >> shootinvince;
		//file >> level.c_str();
	}
	file.close();
	return level;
}

// reset the level, health, etc.
void Player::Reset()
{
	Level = Parent->GetLevel();
	health = maxhealth;
	dmgcounter = 0;
	left = 0;
	x = 5;
	y = 5;
}