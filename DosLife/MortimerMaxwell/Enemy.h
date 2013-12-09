// enemy game object

#ifndef ENEMY_H
#define ENEMY_H

class Enemy : public GameObject {

public:
	Enemy(Console &parent, float ox, float oy, std::string fname);
	State Step();
	void Draw(int offx, int offy);
	void Collide(GameObject *other);
	void Damage(GameObject *cause, int dmg);

private:
	Console* Parent;		// parent, gives access to simple drawing and key check
	ConsoleLevel* Level;	// current level, gives access to collision checking
	int dmgcounter;
	int xspeed;
	float fspeed, framec;
	bool dirleft;
	int xcounter;
	std::vector<std::vector<int>> Frames;
	// StartWalking, Walking, Attack, Die
	int mode;
	// stand, walk, attack
};

#endif /* ENEMY_H */