// cpp file for ItemSelect.h

#include "../MortimerMaxwell/Console.h"
#include "../MortimerMaxwell/ConsoleImage.h"
#include "ItemSelect.h"

ItemSelect::ItemSelect(Console &parent, int x, int y, int size)
{
	Parent = &parent;
	position.X = x;
	position.Y = y;

	ConsoleImage* tempimg = new ConsoleImage(*Parent,5,5,1,C_BLUE,C_RED);
	for(int i = 0; i < size; ++i)
	{
		sel_char.push_back(0);
		sel_fcolor.push_back(0);
		sel_bcolor.push_back(0);
		menu.push_back("");
		menu_pics.push_back(tempimg);
	}

	//set up the coordinates
	WORD temp = x;
	x_menu.push_back(x);
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
}

//---Step--------------------------------------------------
int ItemSelect::Step()
{
	int returnval = 0;

	// --- sub menu code
	if (menu_down != -1)
	{
		// check and set the values
		for(WORD menu_c = 0; menu_c < menu_pics[menu_down]->GetW(); ++menu_c)
		{
			for(WORD menu_c2 = 0; menu_c2 < menu_pics[menu_down]->GetH(); ++menu_c2)
			{
				if (Parent->MouseArea(x_menu[menu_down]+menu_c,
					position.Y-menu_pics[menu_down]->GetH()+menu_c2,
					x_menu[menu_down]+menu_c,
					position.Y-menu_pics[menu_down]->GetH()+menu_c2) == true)
				{
					if (Parent->GetKey(VK_LBUTTON) == 1)
					{
						returnval = 1;
						// --- set the data selected
						sel_char[menu_down] = menu_pics[menu_down]->GetChar(0,menu_c,menu_c2);
						sel_fcolor[menu_down] = menu_pics[menu_down]->GetFColor(0,menu_c,menu_c2);
						sel_bcolor[menu_down] = menu_pics[menu_down]->GetBColor(0,menu_c,menu_c2);
					}
				}
			}
		}
	}


	// --- menu code
	// reset the values to not clicked
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		menu_status[menu_c] = false;
	}

	// check if the mouse is over a menu
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		if (Parent->MouseArea(x_menu[menu_c],position.Y,x_menu[menu_c+1]-2,position.Y) == true)
		{
			menu_status[menu_c] = true;
			if (menu_down != -1)
				menu_down = menu_c;
			if (Parent->GetKey(VK_LBUTTON) == 1)
				menu_down = menu_c;
		}
	}

	// if clicked and not over a menu
	if (Parent->MouseArea(0,position.Y,x_menu[menu.size()],position.Y) == false)
	{
		if (Parent->GetKey(VK_LBUTTON) == 1)
			menu_down = -1;
	}

	if (menu_down != -1)
		menu_status[menu_down] = true;
	return returnval;
}

//---Draw--------------------------------------------------
void ItemSelect::Draw()
{
	// draw the menu bar
	for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
	{
		if (menu_status[menu_c] == true) //draw this if selected
			Parent->SetColor(x_menu[menu_c],position.Y,C_WHITE,C_DKBLUE,
				x_menu[menu_c+1]-x_menu[menu_c]-1);

		Parent->WriteStr(x_menu[menu_c],position.Y,menu[menu_c]);
	}

	// draw the submenu
	if (menu_down != -1)
	{
		menu_pics[menu_down]->Draw(0,x_menu[menu_down],
			position.Y-menu_pics[menu_down]->GetH());
		
		for(int yCount = position.Y-menu_pics[menu_down]->GetH(); 
			yCount < position.Y; ++yCount)
		{
			Parent->SetColor(x_menu[menu_down]-1,yCount,C_WHITE,C_BLUE);
			Parent->WriteChar(x_menu[menu_down]-1,yCount,'\xBA');
			Parent->SetColor(x_menu[menu_down]+menu_pics[menu_down]->GetW(),
				yCount,C_WHITE,C_BLUE);
			Parent->WriteChar(x_menu[menu_down]+menu_pics[menu_down]->GetW(),
				yCount,'\xBA');
		}
		Parent->SetColor(x_menu[menu_down]-1,position.Y-menu_pics[menu_down]->GetH()-1,
			C_WHITE,C_BLUE,menu_pics[menu_down]->GetW()+2);
		Parent->WriteChar(x_menu[menu_down],position.Y-menu_pics[menu_down]->GetH()-1,
			'\xCD',menu_pics[menu_down]->GetW());
		Parent->WriteChar(x_menu[menu_down]-1,
			position.Y-menu_pics[menu_down]->GetH()-1,'\xC9');
		Parent->WriteChar(x_menu[menu_down]+menu_pics[menu_down]->GetW(),
			position.Y-menu_pics[menu_down]->GetH()-1,'\xBB');
	}
}

// gets the last selected char
int ItemSelect::GetSelectedChar(int menu)
{
	return sel_char.at(menu);
}

// gets the last selected foreground color
int ItemSelect::GetSelectedFColor(int menu)
{
	return sel_fcolor.at(menu);
}

// gets the last selected background color
int ItemSelect::GetSelectedBColor(int menu)
{
	return sel_bcolor.at(menu);
}

void ItemSelect::SetMenu(unsigned int number, std::string title, ConsoleImage *image)
{
	if ((number >= 0) && (number < menu.size()))
	{
		menu[number] = title;
		menu_pics[number] = image;

		x_menu.erase(x_menu.begin(),x_menu.end());
		//set up the coordinates
		WORD temp = position.X;
		x_menu.push_back(position.X);
		for(WORD menu_c = 0; menu_c < menu.size(); ++menu_c)
		{
			x_menu.push_back(temp+1+(WORD)menu[menu_c].length());
			temp = x_menu.at(menu_c+1);
		}
	}
}

// returns if a menu is down
bool ItemSelect::GetActive()
{
	if (menu_down == -1)
		return false;
	else return true;
}