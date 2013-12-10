// windows specific includes (to be changed?)
#include <windows.h>	// WinApi header
#pragma comment(lib, "winmm.lib")

#include <iostream>		// cout and cin
#include <string>
#include <fstream>
#include "../MortimerMaxwell/Console.h"
#include "../MortimerMaxwell/ConsoleImage.h"
#include "../MortimerMaxwell/ConsoleLevel.h"
#include "Menu.h"
#include "ItemSelect.h"
#include "TextBox.h"
#include "ObjectBox.h"
#include "../MortimerMaxwell/Framerate.h"

void DrawOutline(Console *parent, int x, int y, int w, int h);

int main()
{
	// set the title
	SetConsoleTitle(L"Mortimer Maxwell Level Editor");

	// create the console object
	Console Con(80,43);
	// turn off the cursor
	Con.SetMouseVisible(false);

	// filenames
	std::string filename = "Untitled.dat";
	std::string load_filename = "Untitled.dat";

	// create the level buffer
	ConsoleLevel levelBuf(Con, 300, 40, ' ', C_GRAY, C_BLACK);

	// create a structure for the selection
	struct {
		bool select;
		bool clear;
		bool drag;
		ConsoleImage* image;
		COORD first;
		COORD last;
		COORD pos;
		COORD opos;
	} Selection;
	Selection.select = false;
	
	// create the menu system
	Menu Titlebar(Con);

	// create the edit system
	ItemSelect Tools(Con,14,42,3);
	//load the images for the edit system
	ConsoleImage mcolors(Con,"data/colordata.dat");
	ConsoleImage mcolors2(Con,"data/colordata2.dat");
	ConsoleImage mchars(Con,"data/chardata.dat");

	// set the menus in tools
	Tools.SetMenu(0,"F/Color",&mcolors2);
	Tools.SetMenu(1,"B/Color",&mcolors2);
	Tools.SetMenu(2,"Char",&mchars);

	// create the cursor position
	COORD cur = {0,0};
	COORD txtcur = {0,0};
	COORD Icur = cur;

	// current settings
	int Scolor1 = 0;
	int Scolor2 = 0;
	int Schar = 0;

	int cFrame = 0;
	int sX = 0;
	int sY = 0;
	std::string ObjectName = "none";
	std::string ObjTypeName = "sign";
	std::string ObjectParams;
	COORD ObjectSize = {5,5};
	std::string action; // what was clicked (if anything)
	std::string mode = "Manual";	// current mode
	std::string view = "Image";
	std::string TypeStr; // characters typed onto the image
	// next actions
	struct NEXT {
		bool all;
		bool save;
		bool load;
		bool newf;
		bool obj;
	} next;
	struct KEY {
		bool up;
		bool down;
		bool left;
		bool right;
	} Key;
	next.all = false;
	next.save = false;
	next.load = false;
	next.newf = false;
	next.obj = false;
	Key.up = true;
	Key.down = true;
	Key.left = true;
	Key.right = true;
	TextBox *box;
	ObjectBox *sObj = new ObjectBox(Con);
	int button = 0;
	bool tabup = true;

	// the coveted clicky stuff
	int firstclickL = 0;
	int lastclickL = 0;
	int clickdownL = 0;
	int clickL = 0;

	// initialize the timer
	TimerInit();
	float last_time = TimerGetTime();
	float timer_dif;
	
	bool appActive = true;
	// if we are active, do stuff (if we aren't, exit)
	while (appActive)
	{
		// get the time elapsed since the last check
		timer_dif = (TimerGetTime() - last_time)/1000;

		// if it is greater than 1/20th of a second
		if (timer_dif > 0.05)
		{
			last_time = TimerGetTime();

			// ----------------------------------------------------------------
			if (Con.GetKey(VK_ESCAPE) == 1)
				appActive = false;

			// update and set the cursor
			cur.X = Con.GetMouseCellX();
			cur.Y = Con.GetMouseCellY();
			if (cur.X < 0)
				cur.X = 0;
			if (cur.Y < 0)
				cur.Y = 0;
			Con.SetCursor(cur.X,cur.Y);
			Icur.X = cur.X+sX;
			Icur.Y = cur.Y-1+sY;

			// check for clicking
			if (Con.GetKey(VK_LBUTTON) == 1)
			{
				firstclickL = 0;
				if ((clickL == 0) && !(Titlebar.GetActive()) && !(Tools.GetActive()) 
					&& (next.all == false))
				{
					firstclickL = 1;
					clickdownL = 1;
				}
				clickL = 1;
			}
			else
			{
				lastclickL = 0;
				if (clickdownL == 1)
					lastclickL = 1;
				clickL = 0;
				firstclickL = 0;
				clickdownL = 0;
			}

			// process next
			if ((next.save == true) || (next.load == true) || (next.newf == true) ||
				(next.obj == true))
				next.all = true;
			else
				next.all = false;

			// change views if tab is tapped
			if (Con.GetKey(VK_TAB) && tabup && (next.all == false))
			{
				if (view == "Image")
					view = "Solid";
				else if (view == "Solid")
					view = "Object";
				else if (view == "Object")
					view = "All";
				else if (view == "All")
					view = "Image";
				tabup = false;
			}
			else if ((Con.GetKey(VK_TAB) == false)  && (next.all == false))
				tabup = true;
			// --- update the menu ---
			if (next.all == false)
				action = Titlebar.Step(3-(view != "Image"));
			else
				action = "";

			if (action == "Exit")
				appActive = false;
			if ((action == "Image") || (action == "Solid") || 
				(action == "Object") || (action == "All"))
			{
				view = action;
				if (action != "Image")
				{
					Tools.SetMenu(0,"",&mcolors2);
					Tools.SetMenu(1,"",&mcolors2);
					Tools.SetMenu(2,"",&mcolors2);
				}
				else
				{
					if (mode == "Manual")
					{
						Tools.SetMenu(0,"F/Color",&mcolors2);
						Tools.SetMenu(1,"B/Color",&mcolors2);
						Tools.SetMenu(2,"Char",&mchars);
					}
					if (mode == "Colors")
					{
						Tools.SetMenu(0,"Color",&mcolors);
						Tools.SetMenu(1,"",&mcolors2);
						Tools.SetMenu(2,"",&mchars);
					}
				}
			}
			if ((action == "Manual") || (action == "Colors") || 
				(action == "Text") || (action == "Select") || (action == "Size"))
			{
				mode = action;
				if (mode == "Manual")
				{
					Tools.SetMenu(0,"F/Color",&mcolors2);
					Tools.SetMenu(1,"B/Color",&mcolors2);
					Tools.SetMenu(2,"Char",&mchars);
				}
				else if (mode == "Colors")
				{
					Tools.SetMenu(0,"Color",&mcolors);
					Tools.SetMenu(1,"",&mcolors2);
					Tools.SetMenu(2,"",&mchars);
				} 
				else
				{
					Con.ClearInput();
					Tools.SetMenu(0,"",&mcolors2);
					Tools.SetMenu(1,"",&mcolors2);
					Tools.SetMenu(2,"",&mcolors2);
				}
			}
			if (action == "Save")
			{
				next.save = true;
				box = new TextBox(Con,"Please enter the name to save the file as:",
								  true,"Save","Cancel");
			}
			if (action == "Load")
			{
				next.load = true;
				box = new TextBox(Con,"Please enter the name of the file to load:",
								  true,"Load","Cancel");
			}
			if (action == "New")
			{
				next.newf = true;
				box = new TextBox(Con,"Are you sure you want to create a new level?",
								  false,"Yes","No");
			}
			if (action == "Set Obj")
			{
				next.obj = true;
			}

			// --- update the tools ---
			if (next.all == false)
			{
				if (Tools.Step())
				{
					if (mode == "Manual")
					{
						Scolor1 = Tools.GetSelectedBColor(0);
						Scolor2 = Tools.GetSelectedBColor(1);
						Schar = Tools.GetSelectedChar(2);
					}
					if (mode == "Colors")
					{
						Scolor1 = Tools.GetSelectedFColor(0);
						Scolor2 = Tools.GetSelectedBColor(0);
						Schar = Tools.GetSelectedChar(0);
					}
				}
			}

			// drawing on the image
			if (Con.MouseArea(0,1,78,40) && (next.all == false) && view == "Image")
			{
				if ((mode == "Manual") && clickdownL)
				{
					if (Con.GetKey(VK_SHIFT))
						levelBuf.SetFColor(cur.X+sX,cur.Y+sY-1,Scolor1);
					else if (Con.GetKey(VK_CONTROL))
						levelBuf.SetChar(cur.X+sX,cur.Y+sY-1,Schar);
					else
						levelBuf.SetBColor(cur.X+sX,cur.Y+sY-1,Scolor2);
				}
				if ((mode == "Colors") && clickdownL)
				{
					if (Con.GetKey(VK_CONTROL))
					{
						Scolor1 = levelBuf.GetFColor(cur.X+sX,cur.Y+sY-1);
						Scolor2 = levelBuf.GetBColor(cur.X+sX,cur.Y+sY-1);
						Schar = levelBuf.GetChar(cur.X+sX,cur.Y+sY-1);
					} else {
						levelBuf.SetFColor(cur.X+sX,cur.Y+sY-1,Scolor1);
						levelBuf.SetBColor(cur.X+sX,cur.Y+sY-1,Scolor2);
						levelBuf.SetChar(cur.X+sX,cur.Y+sY-1,Schar);
					}
				}
				if (mode == "Text")
				{
					Con.SetMouseVisible(true);
					char typed = Con.InputChar();
					if ((typed == 8) && (TypeStr.length() > 0))
						TypeStr.erase(TypeStr.end()-1);
					else if (typed != 0)
					{
						levelBuf.SetChar(txtcur.X+sX+(WORD)TypeStr.length(),txtcur.Y+sY,typed);
						TypeStr.append(Con.CharToStr(typed));
					}
					if (clickdownL)
					{
						txtcur.X = cur.X;
						txtcur.Y = cur.Y-1;
						TypeStr.erase();
						Con.ClearInput();
					}
					Con.SetCursor(txtcur.X+(WORD)TypeStr.length(),txtcur.Y+1);
				}
				else
					Con.SetMouseVisible(false);
				if (mode == "Select" && Selection.select == true)
				{
					if (Con.MouseArea(Selection.pos.X,Selection.pos.Y+1,
						Selection.pos.X+Selection.last.X-Selection.first.X,
						Selection.pos.Y+Selection.last.Y-Selection.first.Y+1) && firstclickL)
					{
						Selection.opos.X = cur.X;
						Selection.opos.Y = cur.Y;
						Selection.drag = true;
					} else if (firstclickL)
					{
						// paste the image
						levelBuf.WriteImage(Selection.pos.X+sX,Selection.pos.Y+sY,Selection.image,0);
						Selection.select = false;
						Selection.clear = false;
					}
					if (clickdownL && Selection.drag)
					{
						Selection.pos.X += cur.X-Selection.opos.X;
						Selection.pos.Y += cur.Y-Selection.opos.Y;
						Selection.opos.X = cur.X;
						Selection.opos.Y = cur.Y;
					}
					if (lastclickL)
						Selection.drag = false;
				} else if (mode == "Select")
				{
					if (firstclickL)
					{
						Selection.clear = true;
						Selection.first.X = cur.X;
						Selection.first.Y = cur.Y-1;
					}
					if (lastclickL && Selection.clear)
					{
						Selection.last.X = cur.X;
						Selection.last.Y = cur.Y-1;
						if ((Selection.last.X-Selection.first.X > 0) &&
							(Selection.last.Y-Selection.first.Y > 0))
						{
							Selection.select = true;
							Selection.image = new ConsoleImage(Con,&levelBuf,
								Selection.first.X+sX,Selection.first.Y+sY,
								Selection.last.X+sX,Selection.last.Y+sY);
							Selection.pos.X = Selection.first.X;
							Selection.pos.Y = Selection.first.Y;
						}
					}
				}
				if (mode == "Size")
				{
					// --- up key
					if (Con.GetKey(VK_UP) && Key.up)
					{
						Key.up = false;
						if (levelBuf.GetH() > 1)
							levelBuf.SetH(levelBuf.GetH()-1);
					} 
					else if (!Con.GetKey(VK_UP))
						Key.up = true;
					// --- down key
					if (Con.GetKey(VK_DOWN) && Key.down)
					{
						Key.down = false;
						levelBuf.SetH(levelBuf.GetH()+1);
					} 
					else if (!Con.GetKey(VK_DOWN))
						Key.down = true;
					// --- left key
					if (Con.GetKey(VK_LEFT) && Key.left)
					{
						Key.left = false;
						if (levelBuf.GetW() > 1)
							levelBuf.SetW(levelBuf.GetW()-1);
					} 
					else if (!Con.GetKey(VK_LEFT))
						Key.left = true;
					// --- right key
					if (Con.GetKey(VK_RIGHT) && Key.right)
					{
						Key.right = false;
						levelBuf.SetW(levelBuf.GetW()+1);
					} 
					else if (!Con.GetKey(VK_RIGHT))
						Key.right = true;
				}
			}
			else if ((view != "Image") && (next.all == false))
				Con.SetMouseVisible(false);

			if (Con.MouseArea(0,1,78,40) && (next.all == false) && view == "Solid")
			{
				if (clickdownL)
				{
					levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,1);
					if (Con.GetKey(VK_SHIFT))
						levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,0);
					if (Con.GetKey(VK_UP))
						levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,2);
					if (Con.GetKey(VK_DOWN))
						levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,3);
					if (Con.GetKey(VK_LEFT))
						levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,4);
					if (Con.GetKey(VK_RIGHT))
						levelBuf.SetSolid(cur.X+sX,cur.Y+sY-1,5);
				}
			}

			if (Con.MouseArea(0,1,78,40) && (next.all == false) && view == "Object")
			{
				if (firstclickL && Con.GetKey(VK_CONTROL))
				{
					int objs = levelBuf.GetNumOfObjects();
					for(int objC = 0; objC < objs; ++objC)
					{
						LEVELOBJECT check = * levelBuf.GetLevelObject(objC);
						if ((check.position.X <= cur.X+sX) &&
							(check.position.Y <= cur.Y+sY-1) &&
							(check.position.X+check.image->GetW()-1 >= cur.X+sX) &&
							(check.position.Y+check.image->GetH()-1 >= cur.Y+sY-1))
						{
							// remove the object
							levelBuf.RemoveObject(objC);
							objs -= 1;
						}
					}
				}
				else if (firstclickL)
					levelBuf.AddObject(ObjTypeName,ObjectName,cur.X+sX,cur.Y+sY-1,ObjectParams);
			}


			// scroll bars
			if (Con.GetKey(VK_LBUTTON))
			{
				if ((cur.X == 0) && (cur.Y == 41) && (sX > 0))
					sX -= 1;
				if ((cur.X == 78) && (cur.Y == 41) && (sX < levelBuf.GetW()-79))
					sX += 1;
				if ((cur.X == 79) && (cur.Y == 1) && (sY > 0))
					sY -= 1;
				if ((cur.X == 79) && (cur.Y == 40) && (sY < levelBuf.GetH()-40))
					sY += 1;
			}

			//-------------------draw the screen------------------
			// clear the buffer
			Con.ClearBuffer(C_BLACK,C_GRAY);
			// fill the background to make it look nice in fullscreen
			Con.FillScreen(C_DKGRAY,C_WHITE);

			// --- draw the screen ---
			// draw the image
			levelBuf.Draw(0,1,sX,sY);
			if ((view == "Image") && (Selection.select == true))
			{
				Selection.image->Draw(0,Selection.pos.X,Selection.pos.Y+1);
				// border
				DrawOutline(&Con,Selection.pos.X,Selection.pos.Y+1,
					Selection.last.X-Selection.first.X,
					Selection.last.Y-Selection.first.Y);
			}
			if (view == "Solid" || view == "All")
				levelBuf.DrawSolid(0,1,sX,sY);
			if (view == "Object" || view == "All")
				levelBuf.DrawObjects(0,1,sX,sY);
			if ((view == "Object") && (next.all == false) && Con.GetKey(VK_CONTROL) == false)
				DrawOutline(&Con,cur.X,cur.Y,ObjectSize.X-1,ObjectSize.Y-1);

			// draw the frame around the image
			Con.SetColor(levelBuf.GetW()-sX,1,C_GRAY,C_BLACK,1,levelBuf.GetH()-sY);
			Con.WriteChar(levelBuf.GetW()-sX,1,'\xDD',1,levelBuf.GetH()-sY);
			Con.SetColor(0,levelBuf.GetH()+1-sY,C_GRAY,C_BLACK,levelBuf.GetW()-sX,1);
			Con.WriteChar(0,levelBuf.GetH()+1-sY,'\xDF',levelBuf.GetW()-sX,1);

			Con.SetColor(0,42,C_WHITE,C_BLUE,80,1);
			Con.WriteChar(0,42,' ',80);
			if (view == "Image")
			{
				Con.WriteStr(0,42,"Mode:");
				Con.WriteStr(5,42,mode);
				Con.WriteChar(38,42,'\xB3');
				if (mode == "Manual")
				{
					Con.WriteStr(40,42,"Color 1:");
					Con.SetColor(48,42,C_WHITE,Scolor1);
					Con.WriteStr(53,42,"Color 2:");
					Con.SetColor(61,42,C_WHITE,Scolor2);
					Con.WriteStr(66,42,"Char:");
					Con.WriteChar(71,42,Schar);
				}
				if (mode == "Colors")
				{
					Con.WriteStr(40,42,"Color:");
					Con.SetColor(48,42,Scolor1,Scolor2);
					Con.WriteChar(48,42,Schar);
				}
				if (mode == "Text")
					Con.WriteStr(40,42,"Click the image to insert text.");
				if (mode == "Size")
					Con.WriteStr(40,42,"Use the arrow keys to change the size.");
			}

			if (view == "Object")
			{
				Con.WriteStr(0,42,"Object:");
				Con.WriteStr(7,42,ObjectName);
				if (ObjectName == "none")
					next.obj = true;
			}

			// draw the scroll bars
			Con.SetColor(0,41,C_DKRED,C_GRAY,80,1);
			Con.SetColor(79,1,C_DKRED,C_GRAY,1,41);
			Con.WriteChar(79,1,'\x18');
			Con.WriteChar(79,40,'\x19');
			Con.WriteChar(78,41,'\x1A');
			Con.WriteChar(0,41,'\x1B');
			Con.WriteChar(79,41,' ');
			Con.WriteChar(79,2,'\xB1',1,38);
			Con.WriteChar(1,41,'\xB1',77);
			// draw the sliders on the bars
			float scale = (float)(sX+(sX/(float)(levelBuf.GetW()-79))*78)/levelBuf.GetW();
			if (levelBuf.GetW() > 79)
				Con.WriteChar(1+(int)(scale*77),41,'\xDB');
			scale = (float)(sY+(sY/(float)(levelBuf.GetH()-40))*39)/levelBuf.GetH();
			if (levelBuf.GetH() > 40)
				Con.WriteChar(79,2+(int)(scale*38),'\xDB');
			
			// draw the titlebar
			Con.WriteChar(0,0,' ',80);
			Con.SetColor(0,0,C_WHITE,C_BLUE,80);
			Titlebar.Draw(3-(view != "Image"));
			// write the size
			Con.WriteStr(65,0,"Size:");
			Con.WriteStr(70,0,Con.IntToStr(levelBuf.GetW()).append(",").append(
				Con.IntToStr(levelBuf.GetH())));
			// write the view
			Con.WriteStr(50,0,"View:");
			Con.WriteStr(55,0,view);
			// draw the tools
			Tools.Draw();

			// draw the message boxes (if there is one)
			if (next.all == true)
			{
				if (!next.obj)
				{
					button = box->Activate();
					// get buttons
					if ((button == 1) || (button == 2))
					{
						if (button == 1)
						{
							std::string file = box->GetText();
							if (next.save)
								levelBuf.Save(file);
							if (next.load)
								levelBuf.Load(file);
							if (next.newf)
								levelBuf.Reset(120,40);
						}
						delete box;
						next.load = false;
						next.save = false;
						next.newf = false;
					}
				} else {
					button = sObj->Step();
					if ((button == 1) || (button == 2))
					{
						if (button == 1)
						{
							ObjTypeName = sObj->GetObjType();
							ObjectName = sObj->GetObjectName();
							ObjectParams = sObj->GetParams();
							ObjectSize = sObj->GetObjectSize();
						}
						next.obj = false;
					}
				}
			}

			// draw the screen
			Con.DrawBuffer();
		}

		Sleep(1);
	}

	system("cls");
	return 0;
}

void DrawOutline(Console *parent, int x, int y, int w, int h)
{
	// top border
	parent->SetColor(x,y-1,C_PURPLE,C_CYAN,w+1);
	parent->WriteChar(x,y-1,'\xDC',w+1);
	// bottom border
	parent->SetColor(x,y+h+1,C_PURPLE,C_CYAN,w+1);
	parent->WriteChar(x,y+h+1,'\xDF',w+1);
	// left border
	parent->SetColor(x-1,y,C_PURPLE,C_CYAN,1,h+1);
	parent->WriteChar(x-1,y,'\xDE',1,h+1);
	// right border
	parent->SetColor(x+w+1,y,C_PURPLE,C_CYAN,1,h+1);
	parent->WriteChar(x+w+1,y,'\xDD',1,h+1);
}