// menu class for a selection box (like colors)

#include <windows.h>
#include <vector>

#ifndef ITEMSELECT_H
#define ITEMSELECT_H

using namespace std;

class ConsoleImage;

class ItemSelect {
public:
	ItemSelect(Console &parent, int x, int y, int size);
	int Step();
	void Draw();

	// add choices
	void SetMenu(unsigned int number, std::string title, ConsoleImage* image);

	// get stuff
	bool GetActive();
	int GetSelectedChar(int menu);
	int GetSelectedFColor(int menu);
	int GetSelectedBColor(int menu);

	Console* Parent;

	//all the stuff here
	int menu_down;
	COORD position;
	vector<int> sel_char;
	vector<int> sel_fcolor;
	vector<int> sel_bcolor;

	vector<bool> menu_status;
	vector<int> x_menu;
	vector<string> menu;
	vector<ConsoleImage*> menu_pics;
};

#endif /* MENU_H */