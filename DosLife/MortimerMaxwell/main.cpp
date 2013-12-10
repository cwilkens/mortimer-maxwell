// windows specific includes (to be changed?)
#include <windows.h>	// WinApi header
#pragma comment(lib, "winmm.lib")

#include <iostream>		// cout and cin
#include <string>
#include "Framerate.h"
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"
#include "ObjectList.h"
#include "GameObject.h"

#include "Player.h"
#include "Buttons.h"

int main()
{
	// random numbers
	srand(5);

	// create the console object
	Console Con(80,40);

	// hide the cursor
	Con.SetMouseVisible(false);

	SetConsoleTitle(L"Mortimer Maxwell");

	int room = 0; // room value (current screen)
	// 0 is main menu
	// 1 is game level
	bool set = false;
	int lastroom = -1;

	// create the pointer to the level
	ConsoleLevel* cLevel;
	std::string level = "tutlevel";
	cLevel = new ConsoleLevel(Con, "data/levels/"+level+".dat");
	// create the object list
	ObjectList ObjList;
	// create the player object
	Player PlayerChar(5.0f, 5.0f, Con);
	int Loffset = 0;

	// some images
	ConsoleImage* titleimage = new ConsoleImage(Con,"data/titlescreen.dat");
	ConsoleImage* bigmallard = new ConsoleImage(Con,"data/mallard.dat");
	bigmallard->SetTransparent(C_BLACK);

	// create stuff for the menus
	ObjectList BList;
	std::vector<GameObject> OnButtons;
	ConsoleImage* pausebg;

	// initialize the timer
	TimerInit();
	float last_time = TimerGetTime();
	float timer_dif;
	bool appActive = true;

	// if escape is not pressed, do stuff (if it is, exit)
	while (appActive)
	{
		// get the time elapsed since the last check
		timer_dif = (TimerGetTime() - last_time)/1000;

		// if it is greater than 1/20th of a second
		if (timer_dif > 0.05)
		{
			last_time = TimerGetTime();
			// ----------------------------------------------------------------
			// put the code stuff here (the stuff that gets done every frame)

			// rooms
			// 0 - main menu
			// 1 - main game
			// 2 - options menu
			// 3 - pause screen
			// 4 - completed level screen
			switch (room)
			{
			case 0: // title screen
				if (lastroom != room)
				{
					BList.Clear();
					Button* tmp = new Button(Con,53,21,"Start");
					BList.AddObject(*tmp);
					tmp = new Button(Con,53,26,"Options");
					BList.AddObject(*tmp);
					tmp = new Button(Con,53,31,"Quit");
					BList.AddObject(*tmp);
				}
				// draw the background
				titleimage->Draw(0,0,0);
				bigmallard->Draw(0,5,24);

				BList.StepAll();

				if (BList.Reference(0)->Pressed() == 1)
				{
					PlayerChar.Load("mmsave.dat");
					room = 1;
				}
				if (BList.Reference(1)->Pressed() == 1)
					room = 2;
				if (BList.Reference(2)->Pressed() == 1)
					appActive = false;

				BList.DrawAll(0,0);
				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			case 1: // main game
				if ((lastroom != room) && (lastroom != 3))
				{
					// create the level
					delete cLevel;
					cLevel = new ConsoleLevel(Con, "data/levels/"+level+".dat");
					// set the level, clear and reset the object list
					Con.SetLevel(cLevel);
					ObjList.Clear();
					ConfigLevel(&Con,cLevel,&ObjList);
					PlayerChar.Reset();
					ObjList.AddObject(PlayerChar);
				}

				// perform the code
				ObjList.StepAll();
				// check for collisions
				ObjList.CollideAll();

				Loffset = (int)PlayerChar.GetX()-20;
				Loffset *= (Loffset >= 0);
				if (Loffset > cLevel->GetW()-Con.GetBufWidth())
					Loffset = cLevel->GetW()-Con.GetBufWidth();
				
				// draw the level
				cLevel->Draw(0,0,Loffset,0);
				// draw the stuff
				ObjList.DrawAll(Loffset,0);

				if (Con.GetKey(VK_ESCAPE) || Con.GetKey('P'))
				{
					pausebg = new ConsoleImage(Con,&Con,0,0,79,39);
					room = 3;
				}
				if ((ObjList.TypeCheck(PLAYER) == false))
				{
					pausebg = new ConsoleImage(Con,&Con,0,0,79,39);
					room = 4;
				}
				if (PlayerChar.GetX()+PlayerChar.GetW() > cLevel->GetW())
				{
					pausebg = new ConsoleImage(Con,&Con,0,0,79,39);
					room = 5;
				}

				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			case 2: // options menu
				if (lastroom != room)
				{
					BList.Clear();
					Button* tmp = new Button(Con,53,21,"Start");
					//BList.AddObject(*tmp);
					//tmp = new Button(Con,53,26,"Options");
					//BList.AddObject(*tmp);
					tmp = new Button(Con,53,35,"Back");
					BList.AddObject(*tmp);
				}
				// draw the background

				Con.SetColor(0,0,C_BLUE,C_DKCYAN,80,20);
				Con.WriteChar(0,0,'\xB1',80,20);
				for(int vCount = 0; vCount < 80; ++vCount)
					titleimage->DrawPart(0,vCount,20,79-vCount,20,79-vCount,40);

				BList.StepAll();

				if (BList.Reference(0)->Pressed() == 1)
					room = 0;

				BList.DrawAll(0,0);
				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			case 3: // pause screen of main game
				if (lastroom != room)
				{
					BList.Clear();
					Button* tmp = new Button(Con,34,14,"Continue");
					BList.AddObject(*tmp);
					tmp = new Button(Con,34,19,"Abort");
					BList.AddObject(*tmp);
					tmp = new Button(Con,34,24,"Quit");
					BList.AddObject(*tmp);
				}

				BList.StepAll();

				if (BList.Reference(0)->Pressed() == 1)
					room = 1;
				if (BList.Reference(1)->Pressed() == 1)
					room = 0;
				if (BList.Reference(2)->Pressed() == 1)
					appActive = false;

				pausebg->Draw(0,0,0);
				Con.SetFColor(20,10,C_DKCYAN,40,20);
				Con.WriteChar(20,10,'\xB0',40,20);
				Con.SetColor(20,10,C_DKBLUE,C_BLACK,40);
				Con.SetColor(20,29,C_DKBLUE,C_BLACK,40);
				Con.SetColor(20,10,C_DKBLUE,C_BLACK,1,20);
				Con.SetColor(59,10,C_DKBLUE,C_BLACK,1,20);
				BList.DrawAll(0,0);

				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			case 4: // 'you died' screen of main game
				if (lastroom != room)
				{
					BList.Clear();
					Button* tmp = new Button(Con,34,15,"Restart");
					BList.AddObject(*tmp);
					tmp = new Button(Con,34,22,"Quit");
					BList.AddObject(*tmp);
				}

				BList.StepAll();

				if (BList.Reference(0)->Pressed() == 1)
					room = 1;
				if (BList.Reference(1)->Pressed() == 1)
					room = 0;

				pausebg->Draw(0,0,0);
				Con.SetFColor(20,10,C_DKRED,40,20);
				Con.WriteChar(20,10,'\xB0',40,20);
				Con.SetColor(20,10,C_DKRED,C_BLACK,40);
				Con.SetColor(20,29,C_DKRED,C_BLACK,40);
				Con.SetColor(20,10,C_DKRED,C_BLACK,1,20);
				Con.SetColor(59,10,C_DKRED,C_BLACK,1,20);
				BList.DrawAll(0,0);

				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			case 5: // completed level screen of main game
				if (lastroom != room)
				{
					BList.Clear();
					Button* tmp = new Button(Con,34,14,"Continue");
					BList.AddObject(*tmp);
					tmp = new Button(Con,34,19,"Save");
					BList.AddObject(*tmp);
					tmp = new Button(Con,34,24,"Quit");
					BList.AddObject(*tmp);
				}

				BList.StepAll();

				if (BList.Reference(0)->Pressed() == 1)
				{
					// stuff for next level
					room = 1;
				}
				if (BList.Reference(1)->Pressed() == 1)
					PlayerChar.Save("mmsave.dat",level);
				
				if (BList.Reference(2)->Pressed() == 1)
					room = 0;

				pausebg->Draw(0,0,0);
				Con.SetFColor(20,10,C_DKGREEN,40,20);
				Con.WriteChar(20,10,'\xB0',40,20);
				Con.SetColor(20,10,C_DKBLUE,C_BLACK,40);
				Con.SetColor(20,29,C_DKBLUE,C_BLACK,40);
				Con.SetColor(20,10,C_DKBLUE,C_BLACK,1,20);
				Con.SetColor(59,10,C_DKBLUE,C_BLACK,1,20);
				BList.DrawAll(0,0);

				// draw to the screen and clear the buffer
				Con.DrawBuffer();
				Con.ClearBuffer(C_BLUE, C_WHITE);
				break;

			}
			// end of -put code stuff here-
			// ----------------------------------------------------------------
			if (set == true)
			{
				lastroom = room;
				set = false;
			}
			if (room != lastroom)
				set = true;
		}

		Sleep(1);
	}

	system("cls");
	return 0;
}