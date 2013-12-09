#define NOMINMAX

#include <windows.h>   // WinApi header
#include <iostream>
#include <fstream>		// file input and output
#include <sstream>		// string stream, for conversions
#include <string>		// string class
#include <vector>
#include "Console.h"
#include "ConsoleImage.h"
#include "ConsoleLevel.h"

// loads the image
ConsoleImage::ConsoleImage(Console & parent, std::string filename)
{
	// set background color to null
	bg_color = -1;
	// get a pointer to the parent
	Parent = &parent;
	
	// create a file input stream
	std::ifstream image_file;

	// open the file in binary with read access
	image_file.open (filename.c_str(), std::fstream::in | std::fstream::binary);

	if (image_file.is_open())
	{
		// string to read stuff
		char file_str[20];
		// position in the string (for seperation etc.)
		char* position;

		// get the first line
		image_file.getline(file_str, 20);
		// get the position of the comma, replace it with a null
		position = strchr(file_str, ',');
		position[0] = '\0';
		// put the two chunks into the image size
		std::stringstream (file_str) >> imageSize.X;
		std::stringstream (position+1) >> imageSize.Y;

		// get the second line and put it into frames
		image_file.getline(file_str,5);
		std::stringstream (file_str) >> num_frames;

		// create the picture data
		colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
		charData = new WORD[imageSize.X*imageSize.Y*num_frames];

		int spot;
		for(int fCount = 0; fCount < num_frames; ++fCount)
		{
			for(int yCount = 0; yCount < imageSize.Y; ++yCount)
			{
				for(int xCount = 0; xCount < imageSize.X; ++xCount)
				{
					spot = (imageSize.X*imageSize.Y*fCount)+ // frame
						   (yCount*imageSize.X)+xCount; // row and column

					image_file.read(file_str, 2);
					// store the colors
					int temp = file_str[0];
					if (temp < 0)
						temp = 256+temp;
					colorData[spot] = (WORD)temp;
					// store the character
					charData[spot] = file_str[1];
				}
				// get rid of the newline
				image_file.getline(file_str, 3);
			}
		}
		// close the file
		image_file.close();
	}
	else
	{
		std::cout << "no image file\n" << filename << "\n";
		system("pause");
		// create a blank image if we can't open the file
		num_frames = 1;
		imageSize.X = 10;
		imageSize.Y = 10;
		colorData = new WORD[imageSize.X*imageSize.Y];
		charData = new WORD[imageSize.X*imageSize.Y];
	
		WORD color_mixed = C_GRAY + C_BLACK*16;
		int spot;
	
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (yCount*imageSize.X)+xCount;		// row and column

				// store the colors
				colorData[spot] = color_mixed;

				// store the character
				charData[spot] = 0;
			}
		}
	}
}

// creates a new image if given dimensions instead of a filename
ConsoleImage::ConsoleImage(Console &parent, int width, int height, 
						   int frames, int color1, int color2)
{
	// set background color to null
	bg_color = -1;

	// get a pointer to the parent
	Parent = &parent;

	// copy the data
	imageSize.X = width;
	imageSize.Y = height;
	num_frames = frames;

	// create the picture data
	colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	charData = new WORD[imageSize.X*imageSize.Y*num_frames];

	WORD color_mixed = color1 + color2*16;
	int spot;

	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*imageSize.Y*fCount)+	// frame
					   (yCount*imageSize.X)+xCount;			// row and column

				// store the colors
				colorData[spot] = color_mixed;

				// store the character
				charData[spot] = 0;
			}
		}
	}
}

// creates a new image from a current image
ConsoleImage::ConsoleImage(Console &parent, ConsoleImage *source, int frame, int x1, int y1, int x2, int y2)
{
	// set background color to null
	bg_color = -1;
	// get a pointer to the parent
	Parent = &parent;
	// copy the data
	imageSize.X = x2-x1+1;
	imageSize.Y = y2-y1+1;
	num_frames = 1;
	// create the picture data
	colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	charData = new WORD[imageSize.X*imageSize.Y*num_frames];
	int spot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (yCount*imageSize.X)+xCount;
			colorData[spot] = source->GetFColor(frame,x1+xCount,y1+yCount)+
				source->GetBColor(frame,x1+xCount,y1+yCount)*16;
			charData[spot] = source->GetChar(frame,x1+xCount,y1+yCount);
		}
	}
}

// creates a new image from the screen buffer
ConsoleImage::ConsoleImage(Console &parent, Console *source, int x1, int y1, int x2, int y2)
{
	// set background color to null
	bg_color = -1;
	// get a pointer to the parent
	Parent = &parent;
	// copy the data
	imageSize.X = x2-x1+1;
	imageSize.Y = y2-y1+1;
	num_frames = 1;
	// create the picture data
	colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	charData = new WORD[imageSize.X*imageSize.Y*num_frames];
	int spot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (yCount*imageSize.X)+xCount;
			colorData[spot] = source->GetFColor(x1+xCount,y1+yCount)+
				source->GetBColor(x1+xCount,y1+yCount)*16;
			charData[spot] = source->GetChar(x1+xCount,y1+yCount);
		}
	}
}

// creates a new image from a level
ConsoleImage::ConsoleImage(Console &parent, ConsoleLevel *source, int x1, int y1, int x2, int y2)
{
	// set background color to null
	bg_color = -1;
	// get a pointer to the parent
	Parent = &parent;
	// copy the data
	imageSize.X = x2-x1+1;
	imageSize.Y = y2-y1+1;
	num_frames = 1;
	// create the picture data
	colorData = new WORD[imageSize.X*imageSize.Y];
	charData = new WORD[imageSize.X*imageSize.Y];
	int spot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (yCount*imageSize.X)+xCount;
			colorData[spot] = source->GetFColor(x1+xCount,y1+yCount)+
				source->GetBColor(x1+xCount,y1+yCount)*16;
			charData[spot] = source->GetChar(x1+xCount,y1+yCount);
		}
	}
}

// destructor
ConsoleImage::~ConsoleImage()
{
	delete[] colorData;
	delete[] charData;
}

// copies the image onto the parent's buffer
void ConsoleImage::Draw(int frame, int x, int y, bool hflip)
{
	COORD pos;
	int spot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		if ((y+yCount < Parent->GetBufHeight()) && (y+yCount >= 0))
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				if ((x+xCount < Parent->GetBufWidth()) && (x+xCount >= 0))
				{
					// calculate the position in the data arrays
					spot = (imageSize.X*imageSize.Y*frame)+(yCount*imageSize.X)+
						abs((imageSize.X-1)*(hflip)-xCount);

					// update drawing position
					pos.X = x+xCount;
					pos.Y = y+yCount;

					if (colorData[spot]/16 != bg_color)
					{
						// write the data
						Parent->SetColor(pos.X,pos.Y,colorData[spot]);
						if (hflip) {
							int tmpC = FlipCharH((char)charData[spot]);
							Parent->WriteChar(pos.X,pos.Y,tmpC);
						} else
							Parent->WriteChar(pos.X,pos.Y,(int)charData[spot]);
					}
					else if ((charData[spot] != 0) && (charData[spot] != 32))
					{
						// write the forground color
						Parent->SetFColor(pos.X,pos.Y,colorData[spot]%16);
						// write the char
						if (hflip) {
							int tmpC = FlipCharH((char)charData[spot]);
							Parent->WriteChar(pos.X,pos.Y,tmpC);
						} else
							Parent->WriteChar(pos.X,pos.Y,(int)charData[spot]);
					}
				}
			}
		}
	}
}

// copies a part of the image onto the parent's buffer
void ConsoleImage::DrawPart(int frame, int px, int py, int x1, int y1, int x2, int y2)
{
	COORD pos;
	int spot;
	for(int yCount = 0; yCount <= y2-y1; ++yCount)
	{
		if ((py+yCount < Parent->GetBufHeight()) && (py+yCount >= 0))
		{
			for(int xCount = 0; xCount <= x2-x1; ++xCount)
			{
				if ((px+xCount < Parent->GetBufWidth()) && (px+xCount >= 0))
				{
					// calculate the position in the data arrays
					spot = (imageSize.X*imageSize.Y*frame)+((yCount+y1)*imageSize.X)+xCount+x1;

					// update drawing position
					pos.X = px+xCount;
					pos.Y = py+yCount;

					if (colorData[spot]/16 != bg_color)
					{
						// write the data
						Parent->SetColor(pos.X,pos.Y,colorData[spot]);
						Parent->WriteChar(pos.X,pos.Y,(int)charData[spot]);
					}
					else if (charData[spot] != 0)
					{
						// write the forground color
						Parent->SetColor(pos.X,pos.Y,colorData[spot]%16,
							Parent->GetBColor(pos.X,pos.Y));
						// write the char
						Parent->WriteChar(pos.X,pos.Y,(int)charData[spot]);
					}
				}
			}
		}
	}
}

// sets the transparent color of the image
void ConsoleImage::SetTransparent(int bgcolor)
{
	bg_color = bgcolor;
}

// resets the image to black and gray
void ConsoleImage::Reset(int width, int height, int frames)
{
	delete[] colorData;
	delete[] charData;

	num_frames = frames;
	imageSize.X = width;
	imageSize.Y = height;

	// create the picture data
	colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	charData = new WORD[imageSize.X*imageSize.Y*num_frames];
	int spot;
	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*imageSize.Y*fCount)+	// frame
					   (yCount*imageSize.X)+xCount;			// row and column

				colorData[spot] = C_GRAY + C_BLACK*16;
				charData[spot] = 0;
			}
		}
	}
}

// writes a character onto the image
void ConsoleImage::WriteChar(int frame, int x, int y, char char1)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
		charData[spot] = char1;
}

// sets the foreground color on the image
void ConsoleImage::WriteFColor(int frame, int x, int y, int color)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
	{
		// get the background color and set the new value
		colorData[spot] = color + (colorData[spot]/16)*16;
	}
}

// sets the background color on the image
void ConsoleImage::WriteBColor(int frame, int x, int y, int color)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
	{
		// get the foreground color and set the new value
		colorData[spot] = colorData[spot]%16 + color*16;
	}
}

// creates a new image from a current image
void ConsoleImage::WriteImage(int frame, int x, int y, ConsoleImage *source, int sframe)
{
	int spot;
	for(int yCount = y; yCount < y+source->GetH(); ++yCount)
	{
		if (yCount < imageSize.Y)
		{
			for(int xCount = x; xCount < x+source->GetW(); ++xCount)
			{
				if (xCount < imageSize.X)
				{
					spot = (imageSize.X*imageSize.Y*frame)+
						   (yCount*imageSize.X)+xCount;
					colorData[spot] = source->GetFColor(sframe,xCount-x,yCount-y)+
						source->GetBColor(sframe,xCount-x,yCount-y)*16;
					charData[spot] = source->GetChar(sframe,xCount-x,yCount-y);
				}
			}
		}
	}
}

// colorize the image (relaces anything that's not the transparent color to the specified color)
void ConsoleImage::Colorize(int frame, int color)
{
	int spot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		if (yCount < imageSize.Y)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				if (xCount < imageSize.X)
				{
					spot = (imageSize.X*imageSize.Y*frame)+
						   (yCount*imageSize.X)+xCount;

					if (colorData[spot]/16 != bg_color)
						colorData[spot] = color + color*16;
					else if ((charData[spot] != 0) && (charData[spot] != 32))
						colorData[spot] = color + colorData[spot]/16;
				}
			}
		}
	}
}
// sets the number of frames in the image
void ConsoleImage::SetFrames(int frames)
{
	// create the new data
	WORD* temp_colorData = new WORD[imageSize.X*imageSize.Y*frames];
	WORD* temp_charData = new WORD[imageSize.X*imageSize.Y*frames];

	// transfer the old data into the new array
	int spot;
	for(int fCount = 0; fCount < frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*imageSize.Y*fCount)+	// frame
					   (yCount*imageSize.X)+xCount;			// row and column

				// if it is old data
				if (fCount < num_frames)
				{
					// copy the data
					temp_colorData[spot] = colorData[spot];
					temp_charData[spot] = charData[spot];
				}
				else // if it is new data
				{
					// fill it with blank
					temp_colorData[spot] = 7;
					temp_charData[spot] = 0;
				}
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	colorData = temp_colorData;
	charData = temp_charData;

	// set the new number of frames
	num_frames = frames;
}

// sets the width of the image
void ConsoleImage::SetWidth(int width)
{
	// create the new data
	WORD* temp_colorData = new WORD[width*imageSize.Y*num_frames];
	WORD* temp_charData = new WORD[width*imageSize.Y*num_frames];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < width; ++xCount)
			{
				spot = (width*imageSize.Y*fCount)+	// frame
					   (yCount*width)+xCount;		// row and column
				oldspot = (imageSize.X*imageSize.Y*fCount)+	// frame
						  (yCount*imageSize.X)+xCount;		// row and column

				// if it is old data
				if (xCount < imageSize.X)
				{
					// copy the data
					temp_colorData[spot] = colorData[oldspot];
					temp_charData[spot] = charData[oldspot];
				}
				else // if it is new data
				{
					// fill it with blank
					temp_colorData[spot] = 7;
					temp_charData[spot] = 0;
				}
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	colorData = temp_colorData;
	charData = temp_charData;

	// set the new width
	imageSize.X = width;
}

// sets the height of the image
void ConsoleImage::SetHeight(int height)
{
	// create the new data
	WORD* temp_colorData = new WORD[imageSize.X*height*num_frames];
	WORD* temp_charData = new WORD[imageSize.X*height*num_frames];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < height; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*height*fCount)+		// frame
					   (yCount*imageSize.X)+xCount;		// row and column
				oldspot = (imageSize.X*imageSize.Y*fCount)+	// frame
						  (yCount*imageSize.X)+xCount;		// row and column

				// if it is old data
				if (yCount < imageSize.Y)
				{
					// copy the data
					temp_colorData[spot] = colorData[oldspot];
					temp_charData[spot] = charData[oldspot];
				}
				else // if it is new data
				{
					// fill it with blank
					temp_colorData[spot] = 7;
					temp_charData[spot] = 0;
				}
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	colorData = temp_colorData;
	charData = temp_charData;

	// set the new height
	imageSize.Y = height;
}

// inserts a frame after the given frame
void ConsoleImage::InsertFrame(int frame, int color1, int color2)
{
	// add a frame
	num_frames += 1;

	// create the new data
	WORD* temp_colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	WORD* temp_charData = new WORD[imageSize.X*imageSize.Y*num_frames];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*imageSize.Y*fCount)+	// frame
						  (yCount*imageSize.X)+xCount;		// row and column

				// if it is before the new frame
				if (fCount <= frame)
				{
					// copy the data
					temp_colorData[spot] = colorData[spot];
					temp_charData[spot] = charData[spot];
				}
				// if it is the new frame
				if (fCount == frame+1)
				{
					// copy the data
					temp_colorData[spot] = color1 + color2*16;
					temp_charData[spot] = 0;
				}
				// if it is after the new frame
				if (fCount > frame+1)
				{
					// get the data from the previous frame
					oldspot = (imageSize.X*imageSize.Y*(fCount-1))+
							  (yCount*imageSize.X)+xCount;
					temp_colorData[spot] = colorData[oldspot];
					temp_charData[spot] = charData[oldspot];
				}
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	colorData = temp_colorData;
	charData = temp_charData;
}

// copies a frame into another
void ConsoleImage::CopyFrame(int copyfrom, int dest)
{
	// copy the first frame into the other frame
	int copyspot, destspot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			copyspot = (imageSize.X*imageSize.Y*copyfrom)+	// frame
					  (yCount*imageSize.X)+xCount;			// row and column
			destspot = (imageSize.X*imageSize.Y*dest)+	// frame
					  (yCount*imageSize.X)+xCount;		// row and column
			
			// copy the data
			colorData[destspot] = colorData[copyspot];
			charData[destspot] = charData[copyspot];
		}
	}
	// end of the for loops
}

// removes the given frame
void ConsoleImage::RemoveFrame(int frame)
{
	// remove a frame
	num_frames -= 1;

	// create the new data
	WORD* temp_colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
	WORD* temp_charData = new WORD[imageSize.X*imageSize.Y*num_frames];

	// transfer the old data into the new array
	int spot;
	int oldspot;
	for(int fCount = 0; fCount < num_frames; ++fCount)
	{
		for(int yCount = 0; yCount < imageSize.Y; ++yCount)
		{
			for(int xCount = 0; xCount < imageSize.X; ++xCount)
			{
				spot = (imageSize.X*imageSize.Y*fCount)+	// frame
						  (yCount*imageSize.X)+xCount;		// row and column

				// if it is before the removed frame
				if (fCount < frame)
				{
					// copy the data
					temp_colorData[spot] = colorData[spot];
					temp_charData[spot] = charData[spot];
				}
				else // it is after the new frame
				{
					// get the data from the previous frame
					oldspot = (imageSize.X*imageSize.Y*(fCount+1))+
							  (yCount*imageSize.X)+xCount;
					temp_colorData[spot] = colorData[oldspot];
					temp_charData[spot] = charData[oldspot];
				}
			}
		}
	}
	// delete the old data and set the pointers
	delete[] colorData;
	delete[] charData;
	colorData = temp_colorData;
	charData = temp_charData;
}

// shifts the frame up one cell and fills the bottom with black
void ConsoleImage::ShiftUp(int frame)
{
	int spot, copyspot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (imageSize.X*imageSize.Y*frame)+	// frame
				   (yCount*imageSize.X)+xCount;			// row and column
			copyspot = (imageSize.X*imageSize.Y*frame)+	// frame
					   ((yCount+1)*imageSize.X)+xCount;		// row and column
			
			// copy the data
			if (yCount < imageSize.Y-1)
			{
				colorData[spot] = colorData[copyspot];
				charData[spot] = charData[copyspot];
			} else {
				colorData[spot] = C_GRAY + C_BLACK*16;
				charData[spot] = ' ';
			}
		}
	}
}

// shifts the frame down one cell and fills the top with black
void ConsoleImage::ShiftDown(int frame)
{
	int spot, copyspot;
	for(int yCount = imageSize.Y-1; yCount >= 0; --yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (imageSize.X*imageSize.Y*frame)+	// frame
				   (yCount*imageSize.X)+xCount;			// row and column
			copyspot = (imageSize.X*imageSize.Y*frame)+	// frame
					   ((yCount-1)*imageSize.X)+xCount;		// row and column
			
			// copy the data
			if (yCount > 0)
			{
				colorData[spot] = colorData[copyspot];
				charData[spot] = charData[copyspot];
			} else {
				colorData[spot] = C_GRAY + C_BLACK*16;
				charData[spot] = ' ';
			}
		}
	}
}

// shifts the frame left one cell and fills the right with black
void ConsoleImage::ShiftLeft(int frame)
{
	int spot, copyspot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = 0; xCount < imageSize.X; ++xCount)
		{
			spot = (imageSize.X*imageSize.Y*frame)+	// frame
				   (yCount*imageSize.X)+xCount;			// row and column
			copyspot = (imageSize.X*imageSize.Y*frame)+	// frame
					   (yCount*imageSize.X)+xCount+1;		// row and column
			
			// copy the data
			if (xCount < imageSize.X-1)
			{
				colorData[spot] = colorData[copyspot];
				charData[spot] = charData[copyspot];
			} else {
				colorData[spot] = C_GRAY + C_BLACK*16;
				charData[spot] = ' ';
			}
		}
	}
}

// shifts the frame right one cell and fills the left with black
void ConsoleImage::ShiftRight(int frame)
{
	int spot, copyspot;
	for(int yCount = 0; yCount < imageSize.Y; ++yCount)
	{
		for(int xCount = imageSize.X-1; xCount >= 0; --xCount)
		{
			spot = (imageSize.X*imageSize.Y*frame)+	// frame
				   (yCount*imageSize.X)+xCount;			// row and column
			copyspot = (imageSize.X*imageSize.Y*frame)+	// frame
					   (yCount*imageSize.X)+xCount-1;		// row and column
			
			// copy the data
			if (xCount > 0)
			{
				colorData[spot] = colorData[copyspot];
				charData[spot] = charData[copyspot];
			} else {
				colorData[spot] = C_GRAY + C_BLACK*16;
				charData[spot] = ' ';
			}
		}
	}
}

// gets a character from the image
int ConsoleImage::GetChar(int frame, int x, int y)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
		return charData[spot] % 256;
	else
		return 0;
}

// gets the foreground color from the image
int ConsoleImage::GetFColor(int frame, int x, int y)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
		return colorData[spot] % 16;
	else
		return 0;
}

// gets the background color from the image
int ConsoleImage::GetBColor(int frame, int x, int y)
{
	int spot = (imageSize.X*imageSize.Y*frame)+(y*imageSize.X)+x;

	if ((spot >= 0) && (x < imageSize.X) && (y < imageSize.Y) && (frame < num_frames))
		return colorData[spot] / 16;
	else
		return 0;
}

// gets the width of the image
int ConsoleImage::GetW()
{
	return imageSize.X;
}

// gets the height of the image
int ConsoleImage::GetH()
{
	return imageSize.Y;
}

// gets the transparent color of the image
int ConsoleImage::GetTransparent()
{
	return bg_color;
}

// gets the number of frames in the image
int ConsoleImage::GetFrames()
{
	return num_frames;
}

// saves the image buffer to a file
void ConsoleImage::Save(std::string filename)
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
		image_file << imageSize.X << ",";
		image_file << imageSize.Y << "\n";
		// number of frames
		image_file << num_frames << "\n";
		for(int fCount = 0; fCount < num_frames; ++fCount)
		{
			for(int yCount = 0; yCount < imageSize.Y; ++yCount)
			{
				for(int xCount = 0; xCount < imageSize.X; ++xCount)
				{
					spot = (imageSize.X*imageSize.Y*fCount)+(yCount*imageSize.X)+xCount;
					image_file << (unsigned char)colorData[spot];
					image_file << (unsigned char)charData[spot];
				}
				image_file << '\n';
			}
		}
	}
	image_file.close();
}

// loads the image data from a file
void ConsoleImage::Load(std::string filename)
{
	// create a file input stream
	std::ifstream image_file;

	// open the file in binary with read access
	image_file.open (filename.c_str(), std::fstream::in | std::fstream::binary);

	if (image_file.is_open())
	{
		// string to read stuff
		char file_str[20];
		// position in the string (for seperation etc.)
		char* position;

		// get the first line
		image_file.getline(file_str, 20);
		// get the position of the comma, replace it with a null
		position = strchr(file_str, ',');
		position[0] = '\0';
		// put the two chunks into the image size
		std::stringstream (file_str) >> imageSize.X;
		std::stringstream (position+1) >> imageSize.Y;

		// get the second line and put it into frames
		image_file.getline(file_str,5);
		std::stringstream (file_str) >> num_frames;

		// create the new data
		WORD* temp_colorData = new WORD[imageSize.X*imageSize.Y*num_frames];
		WORD* temp_charData = new WORD[imageSize.X*imageSize.Y*num_frames];

		int spot;
		for(int fCount = 0; fCount < num_frames; ++fCount)
		{
			for(int yCount = 0; yCount < imageSize.Y; ++yCount)
			{
				for(int xCount = 0; xCount < imageSize.X; ++xCount)
				{
					spot = (imageSize.X*imageSize.Y*fCount)+ // frame
						   (yCount*imageSize.X)+xCount; // row and column

					image_file.read(file_str, 2);
					// store the colors
					temp_colorData[spot] = file_str[0];
					// store the character
					temp_charData[spot] = file_str[1];
				}
				// get rid of the newline
				image_file.getline(file_str, 3);
			}
		}

		// delete the old data and set the pointers
		delete[] colorData;
		delete[] charData;
		colorData = temp_colorData;
		charData = temp_charData;
	}
	// close the file
	image_file.close();
}

int ConsoleImage::FlipCharH(char charflip)
{
	//
	int chr = charflip;
	int temp = (char)charflip;
	if (temp < 0)
		temp = 256+temp;
	switch (temp)
	{
	case 221:	chr = 222;	break;
	case 222:	chr = 221;	break;
	}
	return chr;
}