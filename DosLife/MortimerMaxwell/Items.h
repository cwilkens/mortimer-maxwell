// game objects involving items (picked up by player)

#ifndef ITEMS_H
#define ITEMS_H

class Item : public GameObject {

public:
	Item(Console &parent, float ox, float oy, std::string fname, std::string params);
	State Step();
	void Draw(int offx, int offy);
	void Collide(GameObject *other);

private:
	Console* Parent;
	std::string name;
	bool contact;
	// used in different ways depending on the item
	int value;
};

class Powerup : public GameObject {

public:
	Powerup(Console &parent, float ox, float oy, std::string fname, std::string params);
	State Step();
	void Draw(int offx, int offy);
	void Collide(GameObject *other);

private:
	Console* Parent;
	int type;
	bool contact;
};

#endif /* ITEMS_H */