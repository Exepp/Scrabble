#pragma once
#include "Console.h"
#include "Vec2.h"

// height of a panel is needed at compile time
static char const* const LegendPanelTxt[] =
{
	"Pawel Glomski 172026\n",
	"Functionalities: a) b) c) d) e) f) g) h) i) j) k) l) m)\n\n"
	"cursors      = movement\n",
	"q            = exit\n",
	"n            = new game\n",
	"enter        = confirm and end move\n",
	"esc          = cancel current action\n",
	"i            = place a new word\n",
	"backspace    = delete most recent character from input\n",
	"o            = change the orientation of a placed word\n",
	"w            = change your letters\n",
	"1,2,..., 7   = select letters for change\n",
	"s            = save game\n",
	"l            = load game\n",
	"t            = learning mode\n\n"
};


static char const InputPanel_SaveTxt[] =
{
	"<------------- Save Game ------------->\n\n"
	"File name: "
};

static char const InputPanel_LoadTxt[] =
{
	"<------------- Load Game ------------->\n\n"
	"File name: "
};

static char const InputPanel_NewWordTxt[] =
{
	"<------------- New Word ------------->\n\n"
	"Word: "
};


static char const MsgPanel_TopTxt[] =
{
	"                                   \n"
	" |----------- Message -----------| \n"
};

static char const MsgPanel_BotTxt[] =
{
	" |          Press Enter          | \n"
	" |-------------------------------| \n"
	"                                   \n"
};

static char const MsgPanel_WordPlacementTxt[] =
{
	" |    Word Placement Failed!     | \n"
	" |     You loose your turn       | \n"
};

static char const MsgPanel_LoadingTxt[] =
{
	" |        Loading Failed!        | \n"
};

static char const MsgPanel_SavingTxt[] =
{
	" |        Saving Failed!         | \n"
};

static char const MsgPanel_GameOverTxt[] =
{
	" |     Game Won By: Player%d      | \n"
};

static char const MsgPanel_AiPlayerMovedTxt[] =
{
	" |        Ai Player%d Moved       | \n"
};


static int32_t const PlayersCount = 2;

static int32_t const AiPlayers = PlayersCount - 1;		// max = PlayersCount

static int32_t const MaxPlayerLetters = 7;				// max = 9, for exchange to work

static int32_t const BoardSize = minVal(15, 'Z' - 'A' + 1); // width and height

static int32_t const MaxInputWordLength = BoardSize;	// max input for a word 

static int32_t const OneMoveAllLettersBonus = 50;



enum LetterValue : int8_t { Val1 = 1, Val2 = 2, Val3 = 3, Val4 = 4, Val5 = 5, Val6 = 8, Val7 = 10 };

enum class BoardCellType : uint8_t { Normal, LesserLetterMulti, GreaterLetterMulti, LesserWordMulti, GreaterWordMulti, Count };


static int32_t const BoardMultipliers[(uint8_t)BoardCellType::Count] = 
{
	1, // Normal
	2, // LesserLetterMulti
	3, // GreaterLetterMulti
	2, // LesserWordMulti
	3  // GreaterWordMulti
};

static Color CellsBackColors[(uint8_t)BoardCellType::Count] = { Color::Black, Color::Cyan, Color::Blue, Color::Magenta, Color::Red };


static int32_t const LettersValues['Z' - 'A' + 1] =
{
	Val1, // A
	Val3, // B
	Val3, // C
	Val2, // D
	Val1, // E
	Val4, // F
	Val2, // G
	Val4, // H
	Val1, // I
	Val6, // J
	Val5, // K
	Val1, // L
	Val3, // M
	Val1, // N
	Val1, // O
	Val3, // P
	Val7, // Q
	Val1, // R
	Val1, // S
	Val1, // T
	Val1, // U
	Val4, // V
	Val4, // W
	Val6, // X
	Val4, // Y
	Val7  // Z
};

static int32_t constexpr StartingLettersCounts['Z' - 'A' + 1 + 1] = // second +1 for a blank tile
{
	9, // A
	2, // B
	2, // C
	4, // D
	12, // E
	2, // F
	3, // G
	2, // H
	9, // I
	1, // J
	1, // K
	4, // L
	2, // M
	6, // N
	8, // O
	2, // P
	1, // Q
	6, // R
	4, // S
	6, // T
	4, // U
	2, // V
	2, // W
	1, // X
	2, // Y
	1, // Z
	2  // blank
};

// values represents BoardCellTypes
static const uint8_t BoardCellsSetup[(BoardSize + 1) / 2][(BoardSize + 1) / 2] =
{
	{4, 0, 0, 1, 0, 0, 0, 4},
	{0, 3, 0, 0, 0, 2, 0, 0},
	{0, 0, 3, 0, 0, 0, 1, 0},
	{1, 0, 0, 3, 0, 0, 0, 1},
	{0, 0, 0, 0, 3, 0, 0, 0},
	{0, 2, 0, 0, 0, 2, 0, 0},
	{0, 0, 1, 0, 0, 0, 1, 0},
	{4, 0, 0, 1, 0, 0, 0, 3}
};



//////////////////////////////// Panels ////////////////////////////////



static Vec2i const BoardPanelFramePos(2, 1);										// top left position of board frame

static Vec2i const VFSize(3, BoardSize);											// horizontalFrameSize

static Vec2i const HFSize(2 * BoardSize - 1, 2);									// verticalFrameSize, -1 - couse after the last one there is no need for a gap

static Vec2i const BoardPanelContentPos												// top left position of board content
					(BoardPanelFramePos + Vec2i(VFSize.x, HFSize.y));

static Vec2i const PlayersDataPanelPos												// top left position of player's data panel
					(BoardPanelFramePos + Vec2i(0, 2 * HFSize.y + VFSize.y + 3));



static Vec2i const LegendPanelPos(60, 1);											// top left position of legend

static Vec2i const InputPanelPos													// top left position of panel for game loading/saving and new word input
					(LegendPanelPos + Vec2i(0, sizeof(LegendPanelTxt) / sizeof(*LegendPanelTxt) + 4));

static Vec2i const MsgPanelPos														// top left position of error msg panel
					(minVal(LegendPanelPos.x, BoardPanelFramePos.x) + abs(LegendPanelPos.x - BoardPanelFramePos.x) / 2,		 // x
					 minVal(LegendPanelPos.y, BoardPanelFramePos.y) + sizeof(LegendPanelTxt) / sizeof(*LegendPanelTxt) / 2); // y



//////////////////////////////// Utility ////////////////////////////////



constexpr int32_t StartingLettersCount() { int32_t sum = 0; for (auto l : StartingLettersCounts) sum += l; return sum; }