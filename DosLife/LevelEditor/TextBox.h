// text box class

#include <windows.h>
#include <string>

#ifndef TEXTBOX_H
#define TEXTBOX_H

class TextBox {
public:
	TextBox(Console &parent, std::string text, bool textinput, std::string yes_btn, std::string no_btn);
	int Activate();
	std::string GetText();

private:
	void DrawRect(int left, int top, int right, int bottom);
	std::vector<std::string> Text;
	std::string Yes;
	std::string No;
	std::string TextStr;
	unsigned int textlength;
	bool TextIn;

	Console* Parent;
};

#endif /* TEXTBOX_H */