// game objects involving scenery (signs, breakable walls, etc.)

#ifndef SCENERY_H
#define SCENERY_H

class Sign : public GameObject {

public:
	Sign(Console &parent, float ox, float oy, std::string fname, std::string params);
	State Step();
	void Draw(int offx, int offy);
	void Collide(GameObject *other);

private:
	Console* Parent;
	std::string displaystr;
	std::vector<std::string> Textstr;
	bool contact;
	int width;
};


#endif /* SCENERY_H */