// header for the objectlist class
// controls a list of objects

#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include "ConsoleLevel.h"
#include "GameObject.h"

class GameObject;

class ObjectList {

public:
	void AddObject(GameObject &obj);
	void RemoveObject(GameObject &obj);
	void StepAll();
	void CollideAll();
	void DrawAll(int ox, int oy);

	// references
	GameObject* Reference(int number);
	bool TypeCheck(ObjectType type);
	void Clear();

	// checks
	GameObject* Nearest(ObjectType type, float x);

private:
	std::vector<GameObject*> list;
	bool PreciseCollide(GameObject* one, bool oneprecise, GameObject* two, bool twoprecise);
};

void ConfigLevel(Console* screen, ConsoleLevel* level, ObjectList* list);

#endif /* OBJECTLIST_H */