// header file for a button class

#ifndef BUTTONS_H
#define BUTTONS_H

class Button : public GameObject {

public:
	Button(Console &parent, float ox, float oy, std::string displaytext);
	State Step();
	void Draw(int offx, int offy);
	int Pressed();

private:
	Console* Parent;
	std::string text;
	bool contact;
	int button_down;
};

#endif /* BUTTONS_H */