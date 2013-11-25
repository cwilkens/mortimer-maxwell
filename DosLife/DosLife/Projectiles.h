// projetile game objects

#ifndef PROJECTILES_H
#define PROJECTILES_H

struct SONICPARTICLE {
	float x;
	float y;
	int color;
	int chara;
	float yvel;
};

class SonicWave : public GameObject {
public:
	SonicWave(Console &parent, float ox, float oy, bool left, float xvelocity, int dist,
		bool gothrough);
	State Step();
	void Draw(int offx, int offy);
	void Collide(GameObject *other);

private:
	Console* Parent;		// parent
	ConsoleLevel* Level;	// current level, gives access to collision checking
	bool dirleft;
	float speed;
	std::vector<SONICPARTICLE> particles;
	float sonicwaves;
	int distance;
	float distc;
	int damage;
	bool hit;
	bool invincible;
	// holds current position for the next step
	float xtemp, ytemp;
};

#endif /* PROJECTILES_H */