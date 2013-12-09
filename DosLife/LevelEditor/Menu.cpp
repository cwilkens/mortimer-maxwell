// cpp file for Menu.h

#include "../MortimerMaxwell/Console.h"
#include "Menu.h"

Menu::Menu(Console &parent)
{
	Parent = &parent;
	// controls the basic menu and backgrounds for them
	// -----------------------------------------------

	//menu. = 4;    // number of menus
	menu.push_back("File");
	menu.push_back("View");
	menu.push_back("Mode");

	std::vector<std::string> tmpV;
	// file menu
	menu_data.push_back(tmpV);
	menu_data[0].push_back("New");
	menu_data[0].push_back("Load");
	menu_data[0].push_back("Save");
	menu_data[0].push_back("Set Obj");
	menu_data[0].push_back("Exit");
	// view menu
	menu_data.push_back(tmpV);
	menu_data[1].push_back("Image");
	menu_data[1].push_back("Solid");
	menu_data[1].push_back("Object");
	menu_data[1].push_back("All");
	// mode menu
	menu_data.push_back(tmpV);
	menu_data[2].push_back("Manual");
	menu_data[2].push_back("Colors");
	menu_data[2].push_back("Text");
	menu_data[2].push_back("Select");
	menu_data[2].push_back("Size");

	//set up the coordinates
	WORD temp = 0;
	x_menu.push_back(0);
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		x_menu.push_back(temp+1+(WORD)menu[menu_c].length());
		temp = x_menu.at(menu_c+1);
	};

	//menu current down (-1 is none)
	menu_down = -1;

	// set the values to not clicked
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
		menu_status.push_back(false);

	// make the drawing colors
	color.push_back(C_WHITE);		// text
	color.push_back(C_BLUE);		// background
	color.push_back(C_DKBLUE);		// highlight

	//set up the coordinates for the submenus(automatic)
	temp = 0;
	// find the longest menu
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		if (menu_data[menu_c].size() > temp)
		{
			temp = (WORD)menu_data[menu_c].size();
			longest_menu = menu_c;
		};
	};

	//find and set the widest width
	temp = 0;
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		for(WORD menu_c2 = 0; menu_c2 < menu_data[menu_c].size(); ++menu_c2)
		{
			if (menu_data[menu_c][menu_c2].length() > (unsigned)temp)
			{
				temp = (WORD)menu_data[menu_c][menu_c2].length()+1;
				submenu_width = (WORD)menu_data[menu_c][menu_c2].length()+1;
			}
		}
	}

	// set the submenu values
	for(WORD menu_c = 0; menu_c < menu_data[longest_menu].size(); ++menu_c)
		submenu_status.push_back(false);

}

//---Step--------------------------------------------------
std::string Menu::Step(int last_menu)
{
	std::string returnval = "";

	// --- sub menu code
	if (menu_down != -1)
	{
		// reset the values
		for(WORD menu_c = 0; menu_c < menu_data[menu_down].size(); menu_c += 1)
		{
			submenu_status[menu_c] = false;
		}
	    
		// check and set the values
		for(WORD menu_c = 0; menu_c < menu_data[menu_down].size(); menu_c += 1)
		{
			if (Parent->MouseArea(x_menu[menu_down],menu_c+1,
						   x_menu[menu_down]+submenu_width,menu_c+1) == true)
			{
				submenu_status[menu_c] = true;
				if (Parent->GetKey(VK_LBUTTON) == 1)
				{
					// --- action code for submenu
					returnval = menu_data[menu_down][menu_c];
				}
			}
		}
	}


	// --- menu code
	// reset the values to not clicked
	for(WORD menu_c = 0; menu_c < menu.size(); menu_c += 1)
	{
		menu_status[menu_c] = false;
	}

	// check if the mouse is over a menu
	for(WORD menu_c = 0; menu_c < last_menu; menu_c += 1)
	{
		if (Parent->MouseArea(x_menu[menu_c],0,x_menu[menu_c+1]-2,0) == true)
		{
			menu_status[menu_c] = true;
			if (menu_down != -1)
				menu_down = menu_c;
			if (Parent->GetKey(VK_LBUTTON) == 1)
				menu_down = menu_c;
		}
	}

	// if clicked and not over a menu
	if (Parent->MouseArea(0,0,x_menu[menu.size()],0) == false)
	{
		if (Parent->GetKey(VK_LBUTTON) == 1)
			menu_down = -1;
	}

	if (menu_down != -1)
		menu_status[menu_down] = true;
	return returnval;
}

//---Draw--------------------------------------------------
void Menu::Draw(int last_menu)
{
	// draw the menu bar
	for(WORD menu_c = 0; menu_c < last_menu; menu_c += 1)
	{
		//draw this always
		Parent->SetColor(x_menu[menu_c],0,color[0],color[1],
			x_menu[menu_c+1]-x_menu[menu_c]);

		if (menu_status[menu_c] == true) //draw this if selected
			Parent->SetColor(x_menu[menu_c],0,color[0],color[2],
				x_menu[menu_c+1]-x_menu[menu_c]-1);

		Parent->WriteStr(x_menu[menu_c],0,menu[menu_c]);
	}

	// draw the rest of the menu bar
	Parent->SetColor(x_menu[menu.size()],0,color[0],color[1],
		Parent->GetBufWidth()-x_menu[menu.size()]);

	// draw the submenu
	if (menu_down != -1)
	{
		for(WORD menu_c = 0; menu_c < menu_data[menu_down].size(); menu_c += 1)
		{
			if (submenu_status[menu_c] == false) //draw this if not selected
				Parent->SetColor(x_menu[menu_down],menu_c+1,
					color[0],color[1],submenu_width);

			if (submenu_status[menu_c] == true) //draw this if selected
				Parent->SetColor(x_menu[menu_down],menu_c+1,
					color[0],color[2],submenu_width);

			Parent->WriteChar(x_menu[menu_down],menu_c+1,' ',submenu_width);
			Parent->WriteStr(x_menu[menu_down],menu_c+1,menu_data[menu_down][menu_c]);
			
		}
		//border stuff here
	}
}

// returns if a menu is down or not (active)
bool Menu::GetActive()
{
	if (menu_down == -1)
		return false;
	else return true;
}