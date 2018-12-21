#pragma once
#include "Vec2.h"

enum class Color : uint8_t
{
	Black, Blue, Green, Cyan, Red,
	Magenta, Brown, Lightgray, Darkgray,
	Lightblue, Lightgreen, Lightcyan, Lightred,
	Lightmagenta, Yellow, White,
	CurrentColor
};

struct Input
{
	enum class SpecialKeys : int8_t 
	{	LeftArrow = 75,	RightArrow = 77, UpArrow = 72, DownArrow = 80	};

	enum class NormalKeys : int8_t
	{	Enter = 13, Esc = 27, Backspace = 8	};

	bool isSpecial = false;
	int32_t code = 0;
};


class Console
{
public:

	Console() = delete;

	static void init();

	static void shutdown();

	static void setCursorPosition(Vec2i const& newPos);


	static void setTextColor(Color color);

	static void setTextBackgroundColor(Color color);


	// clears console at a given color
	static void clear(Color color = Color::Black);

	
	// writes a string at a given position (current cursor's position by default) 
	// and at a given color (current by default) with a given background color
	static void write(char const* str, Vec2i pos = Vec2i(), Color textColor = Color::CurrentColor, Color textBColor = Color::CurrentColor);

	static void write(int32_t val, Vec2i const& pos = Vec2i(), Color textColor = Color::CurrentColor, Color textBColor = Color::CurrentColor);

	static void write(char val, Vec2i const& pos = Vec2i(), Color textColor = Color::CurrentColor, Color textBColor = Color::CurrentColor);


	static Vec2i getCursorPosition();

	static Input getInput();

private:

	static Color textFColor;

	// text background color
	static Color textBColor;	
};

