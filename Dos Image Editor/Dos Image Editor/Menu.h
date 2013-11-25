// menu class for a titlebar (like file, edit, view, etc.)

#include <windows.h>
#include <vector>

#ifndef MENU_H
#define MENU_H

using namespace std;

class Menu {
public:
	Menu(Console &parent);
	std::string Step();
	void Draw();
	bool GetActive();

	Console* Parent;

	//all the stuff here
	int menu_down;
	int submenu_width;
	int longest_menu;

	vector<bool> menu_status;
	vector<bool> submenu_status;
	vector<int> x_menu;
	vector<string> menu;
	vector<vector<string>> menu_data;
	vector<int> color;
};

#endif /* MENU_H */