// the header for the player stuff,
// it has some classes and other stuff

#ifndef PLAYER_H
#define PLAYER_H

class Player : public GameObject {

public:
	Player(float ox, float oy, Console &parent);
	State Step();
	void Draw(int offx, int offy);
	void Damage(GameObject *cause, int dmg);
	void Save(std::string filename, std::string level);
	std::string Load(std::string filename);
	void Reset();

private:
	Console* Parent;		// parent, gives access to simple drawing and key check
	ConsoleLevel* Level;	// current level, gives access to collision checking
	int left;				// direction
	int jump, jcounter;		// jump height and rejump counter
	float framec;			// counter for frame movement
	int dmgcounter;			// counter until you can be damaged again

	// game values
	int maxhealth;
	int shoottime, shootc;
	float shootspeed;
	int shootdist;
	bool shootinvince;
};

#endif /* PLAYER_H */