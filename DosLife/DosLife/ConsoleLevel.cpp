#include <windows.h>	// WinApi header
#include <iostream>		// cout for errors
#include <fstream>		// file input and output
#include <sstream>		// string stream, for conversions
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"

// loads the image
ConsoleLevel::ConsoleLevel(Console &parent, std::string filename)
{
	// get a pointer to the parent
	Parent = &parent;

	// create a file input stream
	std::ifstream image_file;

	// open the file in binary with read access
	image_file.open (filename.c_str(), std::fstream::in | std::fstream::binary);

	if (image_file.is_open())
	{
		// string to read stuff
		char file_str[10];
		// position in the string (for seperation etc.)
		char* position;

		// get the first line
		image_file.getline(file_str, 10);
		// get the position of the comma, replace it with a null
		position = strchr(file_str, ',');
		position[0] = '\0';
		// put the two chunks into the size
		std::stringstream (file_str) >> levelSize.X;
		std::stringstream (position+1) >> levelSize.Y;

		// create the level data
		colorData = new WORD[levelSize.X*levelSize.Y];
		charData = new WORD[levelSize.X*levelSize.Y];
		solidData = new WORD[levelSize.X*levelSize.Y];

		// read the level data into the array
		for(int yCount = 0; yCount < levelSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < levelSize.X; ++xCount)
			{
				image_file.read(file_str, 3);
				// store the colors
				colorData[yCount*levelSize.X+xCount] = file_str[0];
				// store the character
				charData[yCount*levelSize.X+xCount] = file_str[1];
				//store the solid data
				solidData[yCount*levelSize.X+xCount] = file_str[2];
			}
			// get rid of the newline
			image_file.getline(file_str, 3);
		}

		char objstr[256];
		while (image_file.peek() != EOF)
		{
			LEVELOBJECT copyobj;
			char* position;

			image_file.getline(objstr, 256, '|');
			copyobj.name = objstr;
			image_file.getline(objstr, 256, '|');
			copyobj.imagename = objstr;
			image_file.getline(objstr, 256, '|');
			position = strchr(objstr, ',');
			position[0] = '\0';
			std::stringstream (objstr) >> copyobj.position.X;
			std::stringstream (position+1) >> copyobj.position.Y;
			image_file.getline(objstr, 256);
			copyobj.params = objstr;

			Objects.push_back(copyobj);
		}

		// close the file
		image_file.close();
	}
	else
	{
		std::cout << "no level file\n" << filename << "\n";
		system("pause");
		// create a blank level if we can't open the file
		levelSize.X = 80;
		levelSize.Y = 40;
		// create the level data
		colorData = new WORD[levelSize.X*levelSize.Y];
		charData = new WORD[levelSize.X*levelSize.Y];
		solidData = new WORD[levelSize.X*levelSize.Y];
	
		for(int cCount = 0; cCount < levelSize.X*levelSize.Y; ++cCount)
		{
			// initialize the data
			colorData[cCount] = C_GRAY + C_BLACK*16;
			charData[cCount] = ' ';
			solidData[cCount] = 0;
		}
	}
}

// creates a new image if given dimensions instead of a filename
ConsoleLevel::ConsoleLevel(Console &parent, int width, int height, int chara, 
						   int fcolor, int bcolor)
{
	// get a pointer to the parent
	Parent = &parent;

	// copy the data
	levelSize.X = width;
	levelSize.Y = height;

	// create the picture data
	colorData = new WORD[levelSize.X*levelSize.Y];
	charData = new WORD[levelSize.X*levelSize.Y];
	solidData = new WORD[levelSize.X*levelSize.Y];

	for(int cCount = 0; cCount < levelSize.X*levelSize.Y; ++cCount)
	{
		// store the colors
		colorData[cCount] = fcolor + bcolor*16;
		// store the character
		charData[cCount] = chara;
		// initialize the solid data
		solidData[cCount] = 0;
	}
}

// create a level from an existing level
ConsoleLevel::ConsoleLevel(Console &parent, ConsoleLevel *source, int x1, int y1, int x2, int y2)
{
	// get a pointer to the parent
	Parent = &parent;
	// copy the data
	levelSize.X = x2-x1+1;
	levelSize.Y = y2-y1+1;
	// create the level data
	colorData = new WORD[levelSize.X*levelSize.Y];
	charData = new WORD[levelSize.X*levelSize.Y];
	solidData = new WORD[levelSize.X*levelSize.Y];
	int spot;
	for(int yCount = 0; yCount < levelSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < levelSize.X; ++xCount)
		{
			spot = (yCount*levelSize.X)+xCount;
			colorData[spot] = source->GetFColor(x1+xCount,y1+yCount)+
				source->GetBColor(x1+xCount,y1+yCount)*16;
			charData[spot] = source->GetChar(x1+xCount,y1+yCount);
			solidData[spot] = source->GetSolid(x1+xCount,y1+yCount);
		}
	}
}

// destructor
ConsoleLevel::~ConsoleLevel()
{
	delete[] colorData;
	delete[] charData;
	delete[] solidData;
}

// copies the image's buffer onto the parent's buffer
void ConsoleLevel::Draw(int x, int y, int xo, int yo)
{
	COORD pos;
	for(int xCount = 0; xCount < levelSize.X; ++xCount)
	{
		if ((x+xCount < Parent->GetBufWidth()) && (x+xCount >= 0))
		{
			for(int yCount = 0; yCount < levelSize.Y; ++yCount)
			{
				if ((y+yCount < Parent->GetBufHeight()) && (y+yCount >= 0))
				{
					// update drawing position
					pos.X = x+xCount;
					pos.Y = y+yCount;
					if ((xo+xCount < levelSize.X) && (yo+yCount < levelSize.Y))
					{
						Parent->SetColor(pos.X,pos.Y,
							colorData[(yo+yCount)*levelSize.X+xo+xCount]);
						Parent->WriteChar(pos.X,pos.Y,
							(int)charData[(yo+yCount)*levelSize.X+xo+xCount]);
					}
				}
			}
		}
	}
}

// draws onto the buffer the solid parts
void ConsoleLevel::DrawSolid(int x, int y, int xo, int yo)
{
	COORD pos;
	int value;
	for(int xCount = 0; xCount < levelSize.X; ++xCount)
	{
		if ((x+xCount < Parent->GetBufWidth()) && (x+xCount >= 0))
		{
			for(int yCount = 0; yCount < levelSize.Y; ++yCount)
			{
				if ((y+yCount < Parent->GetBufHeight()) && (y+yCount >= 0))
				{
					pos.X = x+xCount;
					pos.Y = y+yCount;
					if ((xo+xCount < levelSize.X) && (yo+yCount < levelSize.Y))
					{
						value = (int)solidData[(yo+yCount)*levelSize.X+xo+xCount];
						if (value != 0)
						{
							Parent->SetColor(pos.X,pos.Y,C_BLACK,C_WHITE);
							if (value == 1)
								Parent->WriteChar(pos.X,pos.Y,'\x0F');
							if (value == 2)
								Parent->WriteChar(pos.X,pos.Y,'\x18');
							if (value == 3)
								Parent->WriteChar(pos.X,pos.Y,'\x19');
							if (value == 4)
								Parent->WriteChar(pos.X,pos.Y,'\x1B');
							if (value == 5)
								Parent->WriteChar(pos.X,pos.Y,'\x1A');
						}
					}
				}
			}
		}
	}
}

// draws onto the buffer the objects
void ConsoleLevel::DrawObjects(int x, int y, int xo, int yo)
{
	for(unsigned int oCount = 0; oCount < Objects.size(); ++oCount)
	{
		if ((Objects.at(oCount).position.X > xo) && (Objects.at(oCount).position.X < 
			xo+Parent->GetBufWidth()) && (Objects.at(oCount).position.Y > yo) && 
			(Objects.at(oCount).position.Y < yo+Parent->GetBufHeight()))
			Objects.at(oCount).image->Draw(0,x+Objects.at(oCount).position.X-xo,
				y+Objects.at(oCount).position.Y-yo);
	}
}

// sets the width of the level
void ConsoleLevel::SetW(int width)
{
	// create the new data
	WORD* temp_colorData = new WORD[width*levelSize.Y];
	WORD* temp_charData = new WORD[width*levelSize.Y];
	WORD* temp_solidData = new WORD[width*levelSize.Y];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int yCount = 0; yCount < levelSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < width; ++xCount)
		{
			spot = (yCount*width)+xCount;
			oldspot = (yCount*levelSize.X)+xCount;

			// if it is old data
			if (xCount < levelSize.X)
			{
				// copy the data
				temp_colorData[spot] = colorData[oldspot];
				temp_charData[spot] = charData[oldspot];
				temp_solidData[spot] = solidData[oldspot];
			}
			else // if it is new data
			{
				// fill it with blank
				temp_colorData[spot] = 7;
				temp_charData[spot] = 0;
				temp_solidData[spot] = 0;
			}
		}
	}

	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	delete[] solidData;
	colorData = temp_colorData;
	charData = temp_charData;
	solidData = temp_solidData;

	// set the new width
	levelSize.X = width;
}

// sets the height of the level
void ConsoleLevel::SetH(int height)
{
	// create the new data
	WORD* temp_colorData = new WORD[levelSize.X*height];
	WORD* temp_charData = new WORD[levelSize.X*height];
	WORD* temp_solidData = new WORD[levelSize.X*height];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int yCount = 0; yCount < height; ++yCount)
	{
		for(int xCount = 0; xCount < levelSize.X; ++xCount)
		{
			spot = (yCount*levelSize.X)+xCount;
			oldspot = (yCount*levelSize.X)+xCount;

			// if it is old data
			if (yCount < levelSize.Y)
			{
				// copy the data
				temp_colorData[spot] = colorData[oldspot];
				temp_charData[spot] = charData[oldspot];
				temp_solidData[spot] = solidData[oldspot];
			}
			else // if it is new data
			{
				// fill it with blank
				temp_colorData[spot] = 7;
				temp_charData[spot] = 0;
				temp_solidData[spot] = 0;
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	delete[] solidData;
	colorData = temp_colorData;
	charData = temp_charData;
	solidData = temp_solidData;

	// set the new height
	levelSize.Y = height;
}

// sets the solid value of a specified cell
void ConsoleLevel::SetSolid(int x, int y, int solid)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// set the new solid value
		solidData[y*levelSize.X+x] = solid;
	}
}

// sets the character of a specified cell
void ConsoleLevel::SetChar(int x, int y, int chara)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// set the new character
		charData[y*levelSize.X+x] = chara;
	}
}

// sets the foreground color of a specified cell
void ConsoleLevel::SetFColor(int x, int y, int color)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// set the new color
		colorData[y*levelSize.X+x] = (GetBColor(x,y)*16)+color;
	}
}

// sets the background color of a specified cell
void ConsoleLevel::SetBColor(int x, int y, int color)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// set the new color
		colorData[y*levelSize.X+x] = GetFColor(x,y)+color*16;
	}
}

// resets the level
void ConsoleLevel::Reset(int width, int height)
{
	delete[] colorData;
	delete[] charData;
	delete[] solidData;
	Objects.erase(Objects.begin());

	levelSize.X = width;
	levelSize.Y = height;

	// create the picture data
	colorData = new WORD[levelSize.X*levelSize.Y];
	charData = new WORD[levelSize.X*levelSize.Y];
	solidData = new WORD[levelSize.X*levelSize.Y];
	int spot;
	for(int yCount = 0; yCount < levelSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < levelSize.X; ++xCount)
		{
			spot = (yCount*levelSize.X)+xCount;

			colorData[spot] = C_GRAY + C_BLACK*16;
			charData[spot] = 0;
			solidData[spot] = 0;
		}
	}
}

// writes an image to the level
void ConsoleLevel::WriteImage(int x, int y, ConsoleImage *source, int sframe)
{
	int spot;
	for(int yCount = y; yCount < y+source->GetH(); ++yCount)
	{
		if ((yCount >= 0) && (yCount < levelSize.Y))
		{
			for(int xCount = x; xCount < x+source->GetW(); ++xCount)
			{
				if ((xCount >= 0) && (xCount < levelSize.X))
				{
					spot = (yCount*levelSize.X)+xCount;
					colorData[spot] = source->GetFColor(sframe,xCount-x,yCount-y)+
						source->GetBColor(sframe,xCount-x,yCount-y)*16;
					charData[spot] = source->GetChar(sframe,xCount-x,yCount-y);
				}
			}
		}
	}
}

// writes a level to the level
void ConsoleLevel::WriteLevel(int x, int y, ConsoleLevel *source)
{
	int spot;
	for(int yCount = y; yCount < y+source->GetH(); ++yCount)
	{
		if (yCount < levelSize.Y)
		{
			for(int xCount = x; xCount < x+source->GetW(); ++xCount)
			{
				if (xCount < levelSize.X)
				{
					spot = (yCount*levelSize.X)+xCount;
					colorData[spot] = source->GetFColor(xCount-x,yCount-y)+
						source->GetBColor(xCount-x,yCount-y)*16;
					charData[spot] = source->GetChar(xCount-x,yCount-y);
					solidData[spot] = source->GetSolid(xCount-x,yCount-y);
				}
			}
		}
	}
}

// adds a object to the level data
void ConsoleLevel::AddObject(std::string name, std::string imagename, int x, int y, std::string params)
{
	LEVELOBJECT tempobj;
	tempobj.name = name;
	tempobj.imagename = imagename;
	tempobj.position.X = x;
	tempobj.position.Y = y;
	tempobj.params = params;
	std::string file = imagename;
	file.append(".dat");
	tempobj.image = new ConsoleImage(*Parent,"data/objects/"+file);
	tempobj.image->SetTransparent(C_BLACK);
	Objects.push_back(tempobj);
}

// gets the entire width of the level
int ConsoleLevel::GetW()
{
	return levelSize.X;
}

// gets the entire height of the level
int ConsoleLevel::GetH()
{
	return levelSize.Y;
}

// gets the solid value of a specified cell
int ConsoleLevel::GetSolid(int x, int y)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// return the solid value
		return solidData[y*levelSize.X+x];
	}
	else return -1;
}

// returns if there are solid cells in the section of the given row
bool ConsoleLevel::GetSolidRow(int row, int x1, int x2, int solid)
{
	if ((x1 <= x2) && (x1 >= 0) && (x2 < levelSize.X) && (row >= 0) && (row < levelSize.Y))
	{
		bool stest = false;
		for(int xCount = x1; xCount <= x2; ++xCount)
		{
			if (GetSolid(xCount,row) == solid)
				stest = true;
		}
		return stest;
	}
	else return true;
}

// returns if there are solid cells in the section of the given column
bool ConsoleLevel::GetSolidCol(int col, int y1, int y2, int solid)
{
	if ((col >= 0) && (col < levelSize.X) && (y1 <= y2) && (y1 >= 0) && (y2 < levelSize.Y))
	{
		bool stest = false;
		for(int yCount = y1; yCount <= y2; ++yCount)
		{
			if (GetSolid(col,yCount) == solid)
				stest = true;
		}
		return stest;
	}
	else return true;
}

// gets the character of a specified cell
int ConsoleLevel::GetChar(int x, int y)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// return the char
		return charData[y*levelSize.X+x];
	}
	else return -1;
}

// gets the foreground color of a specified cell
int ConsoleLevel::GetFColor(int x, int y)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		// return the color
		return colorData[y*levelSize.X+x]%16;
	}
	else return -1;
}

// gets the background color of a specified column
int ConsoleLevel::GetBColor(int x, int y)
{
	if ((x >= 0) && (x < levelSize.X) && (y >= 0) && (y < levelSize.Y))
	{
		return colorData[y*levelSize.X+x]/16;
	}
	else return -1;
}

// gets the number of objects in the level
int ConsoleLevel::GetNumOfObjects()
{
	return (int)Objects.size();
}

// returns a struct of data about an object
LEVELOBJECT* ConsoleLevel::GetLevelObject(int index)
{
	return &Objects.at(index);
}

// saves the level to a file
void ConsoleLevel::Save(std::string filename)
{
	std::ofstream image_file;
	// open in binary mode (raw data) with read and write access
	image_file.open (filename.c_str(), std::ofstream::trunc | std::ofstream::binary);
	// if open
	if (image_file.is_open())
	{
		int spot;
		// clear file
		image_file.clear();
		// size of image
		image_file << levelSize.X << ",";
		image_file << levelSize.Y << "\n";
		for(int yCount = 0; yCount < levelSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < levelSize.X; ++xCount)
			{
				spot = (yCount*levelSize.X)+xCount;
				image_file.put((int)colorData[spot]);
				image_file.put((int)charData[spot]);
				image_file.put((int)solidData[spot]);
			}
			image_file << '\n';
		}
		// objects
		for(unsigned int oCount = 0; oCount < Objects.size(); ++oCount)
		{
			std::string tmp = Objects.at(oCount).name+"|";
			tmp.append(Objects.at(oCount).imagename+"|");
			tmp.append(Parent->IntToStr(Objects.at(oCount).position.X)+",");
			tmp.append(Parent->IntToStr(Objects.at(oCount).position.Y)+"|");
			tmp.append(Objects.at(oCount).params);
			image_file << tmp;
			image_file << '\n';
		}
	}
	image_file.close();
}

// load a level from a file
void ConsoleLevel::Load(std::string filename)
{
	// create a file input stream
	std::ifstream image_file;

	// open the file in binary with read access
	image_file.open (filename.c_str(), std::fstream::in | std::fstream::binary);

	delete[] colorData;
	delete[] charData;
	delete[] solidData;

	if (image_file.is_open())
	{
		// string to read stuff
		char file_str[10];
		// position in the string (for seperation etc.)
		char* position;

		// get the first line
		image_file.getline(file_str, 10);
		// get the position of the comma, replace it with a null
		position = strchr(file_str, ',');
		position[0] = '\0';
		// put the two chunks into the size
		std::stringstream (file_str) >> levelSize.X;
		std::stringstream (position+1) >> levelSize.Y;

		// create the level data
		colorData = new WORD[levelSize.X*levelSize.Y];
		charData = new WORD[levelSize.X*levelSize.Y];
		solidData = new WORD[levelSize.X*levelSize.Y];

		// read the level data into the array
		for(int yCount = 0; yCount < levelSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < levelSize.X; ++xCount)
			{
				image_file.read(file_str, 3);
				// store the colors
				colorData[yCount*levelSize.X+xCount] = file_str[0];
				// store the character
				charData[yCount*levelSize.X+xCount] = file_str[1];
				//store the solid data
				solidData[yCount*levelSize.X+xCount] = file_str[2];
			}
			// get rid of the newline
			image_file.getline(file_str, 3);
		}

		char objstr[256];
		while (image_file.peek() != EOF)
		{
			LEVELOBJECT copyobj;
			char* position;

			image_file.getline(objstr, 256, '|');
			copyobj.name = objstr;
			image_file.getline(objstr, 256, '|');
			copyobj.imagename = objstr;
			image_file.getline(objstr, 256, '|');
			position = strchr(objstr, ',');
			position[0] = '\0';
			std::stringstream (objstr) >> copyobj.position.X;
			std::stringstream (position+1) >> copyobj.position.Y;
			image_file.getline(objstr, 256);
			copyobj.params = objstr;

			Objects.push_back(copyobj);
		}

		// close the file
		image_file.close();
	}
	else
	{
		std::cout << "no level file\n" << filename << "\n";
		system("pause");
		// create a blank level if we can't open the file
		levelSize.X = 80;
		levelSize.Y = 40;
		// create the level data
		colorData = new WORD[levelSize.X*levelSize.Y];
		charData = new WORD[levelSize.X*levelSize.Y];
		solidData = new WORD[levelSize.X*levelSize.Y];
	
		for(int cCount = 0; cCount < levelSize.X; ++cCount)
		{
			// initialize the data
			colorData[cCount] = C_GRAY + C_BLACK*16;
			charData[cCount] = '\xBA';
			solidData[cCount] = 0;
		}
	}
}