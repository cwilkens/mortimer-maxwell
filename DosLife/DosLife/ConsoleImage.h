// the header for the console image class
// this class handles images in the console (like sprites in ascii)

#ifndef CONSOLEIMAGE_H
#define CONSOLEIMAGE_H

#include "Console.h"

class ConsoleImage {
public:
	// create from file
	ConsoleImage(Console &parent, std::string filename);
	// create blank image
	ConsoleImage(Console &parent, int width, int height, int frames, int color1, int color2);
	// create from other image
	ConsoleImage(Console &parent, ConsoleImage *source, int frame, int x1, int y1, int x2, int y2);
	// create from screen buffer
	ConsoleImage(Console &parent, Console *source, int x1, int y1, int x2, int y2);
	// create from level
	ConsoleImage(Console &parent, ConsoleLevel *source, int x1, int y1, int x2, int y2);
	~ConsoleImage();

	// displaying and settings
	void Draw(int frame, int x, int y, bool hflip = false);
	void DrawPart(int frame, int px, int py, int x1, int y1, int x2, int y2);
	void SetTransparent(int bgcolor);

	// editing functions
	void Reset(int width, int height, int frames);
	void WriteChar(int frame, int x, int y, char char1);
	void WriteFColor(int frame, int x, int y, int color);
	void WriteBColor(int frame, int x, int y, int color);
	void WriteImage(int frame, int x, int y, ConsoleImage *source, int sframe);
	void Colorize(int frame, int color);
	void SetFrames(int frames);
	void SetWidth(int width);
	void SetHeight(int height);
	void InsertFrame(int frame, int color1, int color2);
	void CopyFrame(int copyfrom, int dest);
	void RemoveFrame(int frame);
	void ShiftUp(int frame);
	void ShiftDown(int frame);
	void ShiftLeft(int frame);
	void ShiftRight(int frame);

	// get functions
	int GetChar(int frame, int x, int y);
	int GetFColor(int frame, int x, int y);
	int GetBColor(int frame, int x, int y);
	int GetW();
	int GetH();
	int GetTransparent();
	int GetFrames();

	// saving and loading
	void Save(std::string filename);
	void Load(std::string filename);

private:
	int FlipCharH(char charflip);

	Console* Parent;
	int num_frames; // number of frames
	int bg_color;	// transparent color
	COORD imageSize; // size of image
	// pointers to the image data
	WORD* colorData;
	WORD* charData;
};

#endif /* CONSOLEIMAGE_H */