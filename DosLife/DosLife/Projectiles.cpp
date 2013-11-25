#include <cmath>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "GameObject.h"
#include "Projectiles.h"

//--------------------------------------------------------------------------------
// --- SonicWave ---
//--------------------------------------------------------------------------------

// constructor
SonicWave::SonicWave(Console &parent, float ox, float oy, bool left, float xvelocity,
		int dist, bool gothrough) : GameObject(parent, PROJECTILE, "data/mallardsmall.dat", ox, oy)
{
	Parent = &parent;
	// get the current level
	Level = Parent->GetLevel();

	// set transparency
	sprite.SetTransparent(C_BLACK);

	// turn off precise collision
	spritecollide = false;

	// set some basic variables
	frame = 0;
	x = ox;
	y = oy;
	w = 1;
	h = 1;
	xtemp = x;
	ytemp = y;
	hit = false;
	invincible = gothrough;

	dirleft = left;
	speed = xvelocity;
	distance = dist;
	distc = 0;
	damage = 1;

	particles.resize(rand()%3+3);
	for(int pos = 0; pos < (int)particles.size(); ++pos)
	{
		particles.at(pos).x = x + (rand()%3-1);
		particles.at(pos).y = y + (rand()%3-1);
		particles.at(pos).color = 9+rand()%6;
		particles.at(pos).chara = 13+rand()%2;
		particles.at(pos).yvel = (float)(rand()%11)/50-0.1f;
	}
	sonicwaves = 1;
}

// step event
State SonicWave::Step()
{
	x = xtemp;
	y = ytemp;

	if ((sonicwaves < 4) && (hit == false))
	{
		sonicwaves += speed/3;
		int oldsize = (int)particles.size();
		particles.resize(particles.size()+rand()%3);
		for(int pos = oldsize; pos < (int)particles.size(); ++pos)
		{
			particles.at(pos).x = x-sonicwaves*3*((dirleft == false)-1*dirleft)+(rand()%2*2-1);
			particles.at(pos).y = y+(rand()%3-1);
			particles.at(pos).color = 9+rand()%6;
			particles.at(pos).chara = 13+rand()%2;
			particles.at(pos).yvel = (float)(rand()%11)/50-0.1f;
		}
	}
	if (hit)
	{
		sonicwaves -= speed/2;
	}


	x += speed*((dirleft == false)-1*dirleft)/(1+(hit)*2);
	distc += speed/(1+(hit)*2);

	if ((distc >= distance) || (Level->GetSolid((int)xtemp,(int)ytemp) && !invincible))
		hit = true;

	for(int pos = 0; pos < (int)particles.size(); ++pos)
	{
		particles.at(pos).x += speed*((dirleft == false)-1*dirleft)/(1+(hit)*2);
		particles.at(pos).y += particles.at(pos).yvel;
	}

	w = (int)(sonicwaves)*3;
	h = (int)sonicwaves;
	xtemp = x;
	ytemp = y;
	x -= (dirleft == false)*sonicwaves*3;
	y -= sonicwaves/2;

	if (hit && (sonicwaves <= 0))
		return REMOVE;
	else
		return ACTIVE;
}

// draw to the buffer
void SonicWave::Draw(int offx, int offy)
{
	x = xtemp;
	y = ytemp;

	// draw the waves
	int nx = (int)x;
	for(int wave = (int)sonicwaves; wave >= 0; --wave)
	{
		// 47 / and 92 \ and 179 |
		int wsize = (int)(sonicwaves-wave)+(int)(sonicwaves-wave)%2;
		int curve = 1;
		for(int ywave = wsize/2; ywave >= 0; --ywave)
		{
			int tx = (ywave-wsize/4)-1;
			tx *= (tx >= 0);
			if (dirleft)
			{
				if (!Level->GetSolid(nx+wave*3+tx-offx,(int)y-ywave-offy))
				{
					Parent->SetFColor(nx+wave*3+tx-offx,(int)y-ywave-offy,C_WHITE);
					Parent->WriteChar(nx+wave*3+tx-offx,(int)y-ywave-offy,(char)(179-132*curve));
				}
				if (!Level->GetSolid(nx+wave*3+tx-offx,(int)y+ywave-offy))
				{
					Parent->SetFColor(nx+wave*3+tx-offx,(int)y+ywave-offy,C_WHITE);
					Parent->WriteChar(nx+wave*3+tx-offx,(int)y+ywave-offy,(char)(179-87*curve));
					if (wave == (int)sonicwaves)
						Parent->WriteChar(nx+wave*3-offx,(int)y-offy,'(');
				}
			} else {
				if (!Level->GetSolid(nx-wave*3-tx,(int)y-ywave-offy))
				{
					Parent->SetFColor(nx-wave*3-tx-offx,(int)y-ywave-offy,C_WHITE);
					Parent->WriteChar(nx-wave*3-tx-offx,(int)y-ywave-offy,(char)(179-87*curve));
				}
				if (!Level->GetSolid(nx-wave*3-tx,(int)y+ywave-offy))
				{
					Parent->SetFColor(nx-wave*3-tx-offx,(int)y+ywave-offy,C_WHITE);
					Parent->WriteChar(nx-wave*3-tx-offx,(int)y+ywave-offy,(char)(179-132*curve));
					if (wave == (int)sonicwaves)
						Parent->WriteChar(nx-wave*3-offx,(int)y-offy,')');
				}
			}
			curve = (tx > 0);
		}
	}

	// draw the particles
	for(int pos = 0; pos < (int)particles.size(); ++pos)
	{
		if (!Level->GetSolid((int)particles.at(pos).x,(int)particles.at(pos).y))
		{
			Parent->WriteChar((int)particles.at(pos).x-offx,(int)particles.at(pos).y-offy,
				particles.at(pos).chara);
			Parent->SetFColor((int)particles.at(pos).x-offx,(int)particles.at(pos).y-offy,
				particles.at(pos).color);
		}
	}

	xtemp = x;
	ytemp = y;
	x -= (dirleft == false)*(floor(sonicwaves)+(dirleft))*3;
	y -= (int)(sonicwaves/2)*2/2;
}

void SonicWave::Collide(GameObject *other)
{
	if (other->GetType() == ENEMY)
	{
		if (invincible == false)
			hit = true;
		other->Damage(this,damage);
	}
}