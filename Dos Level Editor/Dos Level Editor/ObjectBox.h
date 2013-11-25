// text box class to set objects in the level

#include <windows.h>
#include <string>
#include "ConsoleImage.h"

#ifndef OBJECTBOX_H
#define OBJECTBOX_H

class ObjectBox {
public:
	ObjectBox(Console &parent);
	int Step();
	std::string GetObjType();
	std::string GetObjectName();
	std::string GetParams();
	COORD GetObjectSize();

private:
	void DrawRect(int left, int top, int right, int bottom);
	std::vector<std::string> Text, ObjectTypes, ObjTypes;
	std::vector<ConsoleImage*> Images;
	int cObject, cType;
	std::string ObjectStr;
	unsigned int textlength;
	int focus;
	bool leftkey, rightkey, upkey, downkey;
	std::vector<std::string> params;
	Console* Parent;
};

#endif /* TEXTBOX_H */