// windows specific includes (to be changed?)
#include <windows.h>	// WinApi header
#pragma comment(lib, "winmm.lib")

#include <windows.h>	// WinApi header
#include <iostream>		// cout and cin
#include <string>
#include <fstream>
#include "../MortimerMaxwell/Console.h"
#include "../MortimerMaxwell/ConsoleImage.h"
#include "Menu.h"
#include "../LevelEditor/ItemSelect.h"
#include "../LevelEditor/TextBox.h"
#include "../MortimerMaxwell/Framerate.h"


int main()
{
	// set the title
	SetConsoleTitle(L"Image Editor");

	// create the console object
	Console Con(80,40);
	// turn off the cursor
	Con.SetMouseVisible(false);

	// filenames
	std::string filename = "Untitled.dat";
	std::string load_filename = "Untitled.dat";

	// create the image buffer
	ConsoleImage imageBuffer(Con, 20, 10, 1, C_GRAY, C_BLACK);
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
	ItemSelect Tools(Con,14,39,3);
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
	std::string action; // what was clicked (if anything)
	std::string mode = "Manual";	// current mode
	std::string TypeStr; // characters typed onto the image
	// next actions
	struct NEXT {
		bool all;
		bool save;
		bool load;
		bool newf;
	} next;
	struct KEY {
		bool up;
		bool down;
		bool left;
		bool right;
		bool W;
		bool A;
		bool S;
		bool D;
	} Key;
	next.all = false;
	next.save = false;
	next.load = false;
	next.newf = false;
	Key.up = true;
	Key.down = true;
	Key.left = true;
	Key.right = true;
	Key.W = true;
	Key.A = true;
	Key.S = true;
	Key.D = true;
	TextBox *box;
	int button = 0;

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
			if ((next.save == true) || (next.load == true) || (next.newf == true))
				next.all = true;
			else
				next.all = false;

			// --- update the menu ---
			if (next.all == false)
				action = Titlebar.Step();
			else
				action = "";

			if (action == "Exit")
				appActive = false;
			if ((action == "Manual") || (action == "Colors") || 
				(action == "Text") || (action == "Frame") || (action == "Select"))
			{
				mode = action;
				if (mode == "Manual")
				{
					Tools.SetMenu(0,"F/Color",&mcolors2);
					Tools.SetMenu(1,"B/Color",&mcolors2);
					Tools.SetMenu(2,"Char",&mchars);
				} else if (mode == "Colors")
				{
					Tools.SetMenu(0,"Color",&mcolors);
					Tools.SetMenu(1,"",&mcolors2);
					Tools.SetMenu(2,"",&mchars);
				} else {
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
				box = new TextBox(Con,"Are you sure you want to create a new image?",
								  false,"Yes","No");
			}
			if (action == "Next")
			{
				if (cFrame < imageBuffer.GetFrames()-1)
					cFrame++;
			}
			if (action == "Back")
			{
				if (cFrame > 0)
					cFrame--;
			}
			if (action == "Insert")
			{
				imageBuffer.InsertFrame(cFrame,C_GRAY,C_BLACK);
				cFrame++;
			}
			if (action == "Copy")
			{
				imageBuffer.InsertFrame(cFrame,C_GRAY,C_BLACK);
				imageBuffer.CopyFrame(cFrame,cFrame+1);
				cFrame++;
			}
			if (action == "Delete")
			{
				if (imageBuffer.GetFrames() > 1)
					imageBuffer.RemoveFrame(cFrame);
				if (cFrame == imageBuffer.GetFrames())
					cFrame--;
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
			if (Con.MouseArea(0,1,78,37) && next.all == false)
			{
				if ((mode == "Manual") && clickdownL)
				{
					if (Con.GetKey(VK_SHIFT))
						imageBuffer.WriteFColor(cFrame,cur.X+sX,cur.Y+sY-1,Scolor1);
					else if (Con.GetKey(VK_CONTROL))
						imageBuffer.WriteChar(cFrame,cur.X+sX,cur.Y+sY-1,Schar);
					else
						imageBuffer.WriteBColor(cFrame,cur.X+sX,cur.Y+sY-1,Scolor2);
				}
				if ((mode == "Colors") && clickdownL)
				{
					if (Con.GetKey(VK_CONTROL))
					{
						Scolor1 = imageBuffer.GetFColor(cFrame,cur.X+sX,cur.Y+sY-1);
						Scolor2 = imageBuffer.GetBColor(cFrame,cur.X+sX,cur.Y+sY-1);
						Schar = imageBuffer.GetChar(cFrame,cur.X+sX,cur.Y+sY-1);
					} else {
						imageBuffer.WriteFColor(cFrame,cur.X+sX,cur.Y+sY-1,Scolor1);
						imageBuffer.WriteBColor(cFrame,cur.X+sX,cur.Y+sY-1,Scolor2);
						imageBuffer.WriteChar(cFrame,cur.X+sX,cur.Y+sY-1,Schar);
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
						imageBuffer.WriteChar(cFrame,txtcur.X+(WORD)TypeStr.length(),txtcur.Y,typed);
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
						imageBuffer.WriteImage(cFrame,Selection.pos.X+sX,Selection.pos.Y+sY,
							Selection.image,0);
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
							Selection.image = new ConsoleImage(Con,&imageBuffer,cFrame,
								Selection.first.X+sX,Selection.first.Y+sY,
								Selection.last.X+sX,Selection.last.Y+sY);
							Selection.pos.X = Selection.first.X;
							Selection.pos.Y = Selection.first.Y;
						}
					}
				}
				if (mode == "Frame")
				{
					// --- up key
					if (Con.GetKey(VK_UP) && Key.up)
					{
						Key.up = false;
						if (imageBuffer.GetH() > 1)
							imageBuffer.SetHeight(imageBuffer.GetH()-1);
					} 
					else if (!Con.GetKey(VK_UP))
						Key.up = true;
					// --- down key
					if (Con.GetKey(VK_DOWN) && Key.down)
					{
						Key.down = false;
						imageBuffer.SetHeight(imageBuffer.GetH()+1);
					} 
					else if (!Con.GetKey(VK_DOWN))
						Key.down = true;
					// --- left key
					if (Con.GetKey(VK_LEFT) && Key.left)
					{
						Key.left = false;
						if (imageBuffer.GetW() > 1)
							imageBuffer.SetWidth(imageBuffer.GetW()-1);
					} 
					else if (!Con.GetKey(VK_LEFT))
						Key.left = true;
					// --- right key
					if (Con.GetKey(VK_RIGHT) && Key.right)
					{
						Key.right = false;
						imageBuffer.SetWidth(imageBuffer.GetW()+1);
					} 
					else if (!Con.GetKey(VK_RIGHT))
						Key.right = true;
					// --- W key
					if (Con.GetKey('W') && Key.W)
					{
						Key.W = false;
						imageBuffer.ShiftUp(cFrame);
					} 
					else if (!Con.GetKey('W'))
						Key.W = true;
					// --- A key
					if (Con.GetKey('A') && Key.A)
					{
						Key.A = false;
						imageBuffer.ShiftLeft(cFrame);
					} 
					else if (!Con.GetKey('A'))
						Key.A = true;
					// --- S key
					if (Con.GetKey('S') && Key.S)
					{
						Key.S = false;
						imageBuffer.ShiftDown(cFrame);
					} 
					else if (!Con.GetKey('S'))
						Key.S = true;
					// --- D key
					if (Con.GetKey('D') && Key.D)
					{
						Key.D = false;
						imageBuffer.ShiftRight(cFrame);
					} 
					else if (!Con.GetKey('D'))
						Key.D = true;
				}
			}

			// scroll bars
			if (Con.GetKey(VK_LBUTTON))
			{
				if ((cur.X == 0) && (cur.Y == 38) && (sX > 0))
					sX -= 1;
				if ((cur.X == 78) && (cur.Y == 38) && (sX < imageBuffer.GetW()-78))
					sX += 1;
				if ((cur.X == 79) && (cur.Y == 1) && (sY > 0))
					sY -= 1;
				if ((cur.X == 79) && (cur.Y == 37) && (sY < imageBuffer.GetH()-36))
					sY += 1;
			}

			//-------------------draw the screen------------------
			// clear the buffer
			Con.ClearBuffer(C_BLACK,C_GRAY);
			// fill the background to make it look nice in fullscreen
			Con.FillScreen(C_DKGRAY,C_WHITE);

			// --- draw the screen ---
			// draw the image
			imageBuffer.Draw(cFrame,0-sX,1-sY);
			if (Selection.select == true)
			{
				Selection.image->Draw(0,Selection.pos.X,Selection.pos.Y+1);
				// top border
				Con.SetColor(Selection.pos.X,Selection.pos.Y,C_PURPLE,C_CYAN,
					Selection.last.X-Selection.first.X+1);
				Con.WriteChar(Selection.pos.X,Selection.pos.Y,'\xDC',
					Selection.last.X-Selection.first.X+1);
				// bottom border
				Con.SetColor(Selection.pos.X,Selection.pos.Y+Selection.last.Y-
					Selection.first.Y+2,C_PURPLE,C_CYAN,Selection.last.X-Selection.first.X+1);
				Con.WriteChar(Selection.pos.X,Selection.pos.Y+Selection.last.Y-
					Selection.first.Y+2,'\xDF',Selection.last.X-Selection.first.X+1);
				// left border
				Con.SetColor(Selection.pos.X-1,Selection.pos.Y+1,C_PURPLE,C_CYAN,1,
					Selection.last.Y-Selection.first.Y+1);
				Con.WriteChar(Selection.pos.X-1,Selection.pos.Y+1,'\xDE',1,
					Selection.last.Y-Selection.first.Y+1);
				// right border
				Con.SetColor(Selection.pos.X+Selection.last.X-
					Selection.first.X+1,Selection.pos.Y+1,C_PURPLE,C_CYAN,1,
					Selection.last.Y-Selection.first.Y+1);
				Con.WriteChar(Selection.pos.X+Selection.last.X-
					Selection.first.X+1,Selection.pos.Y+1,'\xDD',1,
					Selection.last.Y-Selection.first.Y+1);
			}

			// draw the frame around the image
			Con.SetColor(imageBuffer.GetW()-sX,1,C_GRAY,C_BLACK,1,imageBuffer.GetH()-sY);
			Con.WriteChar(imageBuffer.GetW()-sX,1,'\xDD',1,imageBuffer.GetH()-sY);
			Con.SetColor(0,imageBuffer.GetH()+1-sY,C_GRAY,C_BLACK,imageBuffer.GetW()-sX,1);
			Con.WriteChar(0,imageBuffer.GetH()+1-sY,'\xDF',imageBuffer.GetW()-sX,1);

			Con.SetColor(0,39,C_WHITE,C_BLUE,80,1);
			Con.WriteChar(0,39,' ',80);
			Con.WriteStr(0,39,"Mode:");
			Con.WriteStr(5,39,mode);
			if (mode != "Frame" && mode != "Select")
				Con.WriteChar(38,39,'\xB3');
			if (mode == "Manual")
			{
				Con.WriteStr(40,39,"F/Color:");
				Con.SetColor(48,39,C_WHITE,Scolor1);
				Con.WriteStr(53,39,"B/Color:");
				Con.SetColor(61,39,C_WHITE,Scolor2);
				Con.WriteStr(66,39,"Char:");
				Con.WriteChar(71,39,Schar);
			}
			if (mode == "Colors")
			{
				Con.WriteStr(40,39,"Color:");
				Con.SetColor(48,39,Scolor1,Scolor2);
				Con.WriteChar(48,39,Schar);
			}
			if (mode == "Text")
				Con.WriteStr(40,39,"Click the image to insert text.");
			if (mode == "Select")
				Con.WriteStr(15,39,"Create a selection and drag it around, or copy it (Frame->Copy).");
			if (mode == "Frame")
				Con.WriteStr(12,39,"Use the arrow keys to change the size, WASD to shift the image.");

			// draw the scroll bars
			Con.SetColor(0,38,C_DKRED,C_GRAY,80,1);
			Con.SetColor(79,1,C_DKRED,C_GRAY,1,37);
			Con.WriteChar(79,1,'\x18');
			Con.WriteChar(79,37,'\x19');
			Con.WriteChar(78,38,'\x1A');
			Con.WriteChar(0,38,'\x1B');
			Con.WriteChar(79,38,' ');
			Con.WriteChar(79,2,'\xB1',1,35);
			Con.WriteChar(1,38,'\xB1',77);
			// draw the sliders on the bars
			float scale = (float)(sX+(sX/(float)(imageBuffer.GetW()-78))*77)/imageBuffer.GetW();
			Con.WriteChar(1+(int)(scale*77),38,'\xDB');
			scale = (float)(sY+(sY/(float)(imageBuffer.GetH()-36))*35)/imageBuffer.GetH();
			Con.WriteChar(79,2+(int)(scale*35),'\xDB');
			
			// draw the titlebar
			Con.WriteChar(0,0,' ',80);
			Titlebar.Draw();
			// write the size and frame number
			Con.WriteStr(50,0,"Size:");
			Con.WriteStr(55,0,Con.IntToStr(imageBuffer.GetW()).append(",").append(
				Con.IntToStr(imageBuffer.GetH())));
			Con.WriteStr(67,0,"Frame:");
			Con.WriteStr(73,0,Con.IntToStr(cFrame+1).append("/").append(
				Con.IntToStr(imageBuffer.GetFrames())));
			// draw the tools
			Tools.Draw();

			// draw the message boxes (if there is one)
			if (next.save == true)
			{
				button = box->Activate();
				// get buttons
				if ((button == 1) || (button == 2))
				{
					if (button == 1)
					{
						std::string file = box->GetText();
						imageBuffer.Save(file);
					}
					delete box;
					next.save = false;
				}
			}
			if (next.load == true)
			{
				button = box->Activate();
				// get buttons
				if ((button == 1) || (button == 2))
				{
					if (button == 1)
					{
						std::string file = box->GetText();
						imageBuffer.Load(file);
					}
					delete box;
					next.load = false;
				}
			}
			if (next.newf == true)
			{
				button = box->Activate();
				// get buttons
				if ((button == 1) || (button == 2))
				{
					if (button == 1)
					{
						imageBuffer.Reset(20,10,1);
					}
					delete box;
					next.newf = false;
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