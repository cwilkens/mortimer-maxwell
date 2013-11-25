// the header for the console level data class
// it handles loading the heightmap and objects, plus drawing the level

#ifndef CONSOLELEVEL_H
#define CONSOLELEVEL_H

class Console;
class ConsoleImage;

struct LEVELOBJECT {
	ConsoleImage *image;
	std::string name;
	std::string imagename;
	COORD position;
	std::string params;
};

const int L_EMPTY = 0;
const int L_SOLID = 1;
const int L_UPONLY = 2;
const int L_DOWNONLY = 3;
const int L_LEFTONLY = 4;
const int L_RIGHTONLY = 5;

class ConsoleLevel {
public:
	// create from file
	ConsoleLevel(Console &parent, std::string filename);
	// create blank level
	ConsoleLevel(Console &parent, int width, int height, int chara, int fcolor, int bcolor);
	// create from existing level
	ConsoleLevel(Console &parent, ConsoleLevel *source, int x1, int y1, int x2, int y2);
	~ConsoleLevel();

	// displaying and settings
	void Draw(int x, int y, int xo, int yo);
	void DrawSolid(int x, int y, int xo, int yo);
	void DrawObjects(int x, int y, int xo, int yo);

	// editing functions
	void SetW(int width);
	void SetH(int height);
	void SetSolid(int x, int y, int solid);
	void SetChar(int x, int y, int chara);
	void SetFColor(int x, int y, int color);
	void SetBColor(int x, int y, int color);
	void Reset(int width, int height);
	void WriteImage(int x, int y, ConsoleImage* source, int sframe);
	void WriteLevel(int x, int y, ConsoleLevel* source);
	void AddObject(std::string name, std::string imagename, int x, int y, std::string params);

	// get functions
	int GetW();
	int GetH();
	int GetSolid(int x, int y);
	bool GetSolidRow(int row, int x1, int x2, int solid);
	bool GetSolidCol(int col, int y1, int y2, int solid);
	int GetChar(int x, int y);
	int GetFColor(int x, int y);
	int GetBColor(int x, int y);
	int GetNumOfObjects();
	LEVELOBJECT* GetLevelObject(int index);

	// saving and loading
	void Save(std::string filename);
	void Load(std::string filename);

private:
	Console* Parent;
	COORD levelSize; // size of level
	char topChar; // the top character
	// pointers to the image data
	WORD* colorData;
	WORD* charData;
	WORD* solidData;
	std::vector<LEVELOBJECT> Objects;
};


#endif /* CONSOLELEVEL_H */