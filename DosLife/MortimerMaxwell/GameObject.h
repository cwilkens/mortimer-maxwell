// generic game object

//#include <windows.h>
#include "ConsoleImage.h"

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class ObjectList;

enum State {
	ACTIVE,
	REMOVE
};

enum ObjectType {
	PLAYER,
	ENEMY,
	ITEM,
	POWERUP,
	PROJECTILE,
	SCENERY
};

class GameObject {
public:
	// overridden by child
	virtual State Step()							{return REMOVE;};
	virtual void Draw(int offx, int offy)			{};
	virtual void Damage(GameObject *cause, int dmg)	{};
	virtual void Collide(GameObject *other)			{};

	GameObject(Console &parent, ObjectType ntype, std::string sfile, float nx, float ny) : sprite(parent, sfile)
	{
		type = ntype;
		x = nx;
		y = ny;
		health = 1;
		spritecollide = true;
	};

	// get functions
	float GetX()	{return x;};
	float GetY()	{return y;};
	int GetW()		{return w;};
	int GetH()		{return h;};
	bool GetPrecise()			{return spritecollide;};
	ConsoleImage* GetSprite()	{return &sprite;};
	ObjectType GetType()		{return type;};
	float GetFrame()			{return frame;};

	void SetController(ObjectList* controller)	{Controller = controller;};

	// specific virtuals
	virtual int Pressed()	{return 1;};

protected:
	ObjectType type;
	ObjectList* Controller;
	ConsoleImage sprite;

	bool spritecollide;
	float x, y;
	float frame;
	int w, h;
	int health;
	int damage;
};

#endif /* GAMEOBJECT_H */