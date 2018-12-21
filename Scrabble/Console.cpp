#include <stdio.h>
#include <string.h>
#include "Console.h"
#include "conio2.h"

Color Console::textFColor = Color::White;
Color Console::textBColor = Color::Black;



void Console::init()
{
	settitle("Pawel, Glomski, 172026");
}

void Console::shutdown()
{
	_setcursortype(_NORMALCURSOR);
	text_info t;
	gettextinfo(&t);
	setCursorPosition({ 1, t.screenheight });
}

void Console::setCursorPosition(Vec2i const& newPos)
{
	gotoxy(newPos.x, newPos.y);
}

void Console::setTextColor(Color color)
{
	textFColor = color;
	textcolor(int32_t(color));
}

void Console::setTextBackgroundColor(Color color)
{
	textBColor = color;
	textbackground(int32_t(color));
}

void Console::clear(Color color)
{
	_setcursortype(_NOCURSOR);
	Color currColor = textBColor;
	Vec2i currPos = { wherex(), wherey() };
	text_info info; 	gettextinfo(&info);

	setTextBackgroundColor(color);
	clrscr();
	setTextBackgroundColor(currColor);
	setCursorPosition(currPos);
}

void Console::write(char const* str, Vec2i pos, Color tColor, Color tBColor)
{
	Color currTextColor = textFColor;
	Color currTextBColor = textBColor;
	
	if (pos.x != 0 || pos.y != 0)
		setCursorPosition(pos);
	else
		pos = { wherex(), wherey() };
	if (tColor != Color::CurrentColor)
		setTextColor(tColor);
	if (tBColor != Color::CurrentColor)
		setTextBackgroundColor(tBColor);

	char text[10240] = {0}; strcpy_s(text, str);
	char* subStrStart = text;
	char* subStrEnd = strchr(subStrStart, '\n');

	while (subStrEnd)
	{
		*subStrEnd = '\0';
		cputs(subStrStart);
		subStrStart = subStrEnd + 1;
		subStrEnd = strchr(subStrStart, '\n');
		setCursorPosition({ pos.x, ++pos.y });
	}
	cputs(subStrStart);

	setTextColor(currTextColor);
	setTextBackgroundColor(currTextBColor);
}

void Console::write(int32_t val, Vec2i const& pos, Color tColor, Color tBColor)
{
	static char txt[32] = {0};
	sprintf_s(txt, "%d", val);
	write(txt, pos, tColor, tBColor);
}

void Console::write(char val, Vec2i const& pos, Color tColor, Color tBColor)
{
	static char txt[2] = { 0, '\0' };
	txt[0] = val;
	write(txt, pos, tColor, tBColor);
}

Vec2i Console::getCursorPosition()
{
	return Vec2i(wherex(), wherey());
}

Input Console::getInput()
{
	int32_t c = getch();
	bool special = (c == 0);
	if (special)
		c = getch();
	return { special, c };
}
