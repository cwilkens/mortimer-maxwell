#include <windows.h>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include "GameObject.h"
#include "ConsoleLevel.h"

#include "Enemy.h"
#include "Scenery.h"
#include "Items.h"

#include "ObjectList.h"

// add an object to the list
void ObjectList::AddObject(GameObject &obj)
{
	list.push_back(&obj);
	list.at((int)list.size()-1)->SetController(this);
}

// remove an object from the list
void ObjectList::RemoveObject(GameObject &obj)
{
	//create an iterator
	std::vector<GameObject*>::iterator list_iter = list.begin();
	while(list_iter != list.end())
	{
		if (*list_iter == &obj)
			list.erase(list_iter); // erase the object if it is the one specified
		list_iter++;
	}
}

// step all the objects in the list
void ObjectList::StepAll()
{
	State returnval;
	int listsize = (int)list.size();
	
	for(int pos = 0; pos < listsize; ++pos)
	{
		returnval = list.at(pos)->Step();
		if (returnval == REMOVE)
		{
			// subtract one from the iterator because we removed one object
			--pos;
			--listsize;
			list.erase(list.begin()+pos+1);
		}
	}
}

// collide all enemy and projectile objects
void ObjectList::CollideAll()
{
	int listsize = (int)list.size();

	for(int pos = 0; pos < listsize; ++pos)
	{
		if (list.at(pos)->GetType() != PLAYER)
		{
			for(int pos2 = 0; pos2 < listsize; ++pos2)
			{
				if ((pos2 != pos) &&
					(list.at(pos)->GetType() != list.at(pos2)->GetType()))
				{
					if (PreciseCollide(list.at(pos),list.at(pos)->GetPrecise(),
									   list.at(pos2),list.at(pos2)->GetPrecise()))
						list.at(pos)->Collide(list.at(pos2));
				}
			}
			// stuff
		}
	}
}

// draw all the objects in the list
void ObjectList::DrawAll(int ox, int oy)
{
	int listsize = (int)list.size();

	for(int pos = 0; pos != listsize; ++pos)
	{
		list.at(pos)->Draw(ox,oy);
	}
}

// returns a pointer to the object in the list
GameObject* ObjectList::Reference(int number)
{
	return list.at(number);
}

// check to see if there are any objects of the type in the list
bool ObjectList::TypeCheck(ObjectType type)
{
	std::vector<GameObject*>::iterator pos;
	for(pos = list.begin(); pos != list.end(); ++pos)
	{
		if ((*pos)->GetType() == type)
			return true;
	}
	return false;
}

// precision collide with two game objects (by their sprites)
bool ObjectList::PreciseCollide(GameObject* one, bool oneprecise,
								GameObject* two, bool twoprecise)
{
	int x1, y1, x2, y2, x3, y3, x4, y4;
	int xT, yT, s1, s2;
	x1 = (int)one->GetX();
	y1 = (int)one->GetY();
	x2 = (int)one->GetX()+one->GetW()-1;
	y2 = (int)one->GetY()+one->GetH()-1;
	x3 = (int)two->GetX();
	y3 = (int)two->GetY();
	x4 = (int)two->GetX()+two->GetW()-1;
	y4 = (int)two->GetY()+two->GetH()-1;
	if ((min(x2,x4) >= max(x1,x3)) && (min(y2,y4) >= max(y1,y3)))
	{
		int xD = max(x1,x3)-min(x1,x3);
		int yD = max(y1,y3)-min(y1,y3);
		for(int xC = 0; xC <= min(x2,x4)-max(x1,x3); ++xC)
		{
			for(int yC = 0; yC <= min(y2+1,y4+1)-max(y1,y3); ++yC)
			{
				xT = xC+xD*(x3 > x1);
				yT = yC+yD*(y3 > y1);
				if (oneprecise)
				{
					s1 = (one->GetSprite()->GetBColor((int)one->GetFrame(),xT,yT) !=
						one->GetSprite()->GetTransparent());
					if ((s1 == 0) && (one->GetSprite()->GetChar((int)one->GetFrame(),xT,yT) != 0) &&
						(one->GetSprite()->GetChar((int)one->GetFrame(),xT,yT) != 32))
						s1 = 1;
				}
				else
					s1 = 1;

				xT = xC+xD*(x3 < x1);
				yT = yC+yD*(y3 < y1);
				if (twoprecise)
				{
					s2 = (two->GetSprite()->GetBColor((int)two->GetFrame(),xT,yT) !=
						two->GetSprite()->GetTransparent());
					if ((s2 == 0) && (two->GetSprite()->GetChar((int)two->GetFrame(),xT,yT) != 0) &&
						(two->GetSprite()->GetChar((int)two->GetFrame(),xT,yT) != 32))
						s2 = 1;
				}
				else
					s2 = 1;

				if (s1 && s2)
					return true;
			}
		}
	}
	
	return false;
}

// remove all of the objects in the list
void ObjectList::Clear()
{
	int listsize = (int)list.size();
	for(int pos = 0; pos != listsize; ++pos)
	{
		if (list.at(pos)->GetType() != PLAYER)
			delete list.at(pos);
	}
	list.erase(list.begin(), list.end());
}

// adds the objects in the level to the object list
void ConfigLevel(Console* screen, ConsoleLevel* level, ObjectList* list)
{
	// add in the objects from the level
	for(int oCount = 0; oCount < level->GetNumOfObjects(); ++oCount)
	{
		LEVELOBJECT *objdata = level->GetLevelObject(oCount);
		std::string file = objdata->imagename;
		file.append(".dat");
				
		std::string objname = objdata->name;
		std::string Params = objdata->params;
		char* pos;
		if (objname.find(' ',0) != std::string::npos)
		{
			pos = &objname.at(objname.find_last_of(' ',objname.length()-1));
			objname = pos+1;
		}
		if (objname == "enemy")
		{
			Enemy* tmp = new Enemy(*screen,objdata->position.X,objdata->position.Y,file);
			list->AddObject(*tmp);
		}
		if (objname == "sign")
		{
			Sign* tmp = new Sign(*screen,objdata->position.X,objdata->position.Y,file,Params);
			list->AddObject(*tmp);
		}
		if (objname == "item")
		{
			Item* tmp = new Item(*screen,objdata->position.X,objdata->position.Y,file,Params);
			list->AddObject(*tmp);
		}
	}
};

// returns a pointer to the nearest object (of type) to the position
GameObject* ObjectList::Nearest(ObjectType type, float x)
{
	GameObject* tpoint;
	tpoint = NULL;
	float dist = 30000;
	int listsize = (int)list.size();

	for(int pos = 0; pos != listsize; ++pos)
	{
		if (list.at(pos)->GetType() == type)
		{
			if (abs((int)x-(int)list.at(pos)->GetX()) < dist)
			{
				tpoint = list.at(pos);
			}
		}
	}
	return tpoint;
}