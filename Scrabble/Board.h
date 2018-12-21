#pragma once
#include "Player.h"
#include "Console.h"
#include "Dictionary.h"

static char const BlankTile = ' ';
static Vec2i const BoardStartingIdx(BoardSize / 2, BoardSize / 2);
static Vec2i const BoardMinIndices(0, 0);
static Vec2i const BoardMaxIndices(BoardSize - 1, BoardSize - 1);
static Vec2i const Orients[2] = { Vec2i(1, 0), Vec2i(0, 1) };			// [0] = Horizontal, [1] = Vertical


enum class Orientation : uint8_t { Horizontal, Vertical };


struct ValidatedWord
{
	bool isValid[MaxInputWordLength] = { 0 };

	bool fromBoard[MaxInputWordLength] = { 0 }; 

	// if(isValid && fromBoard) => letter is valid, and came from board
	// if(isValid && !fromBoard) => letter is valid, and came from player
	// if(!isValid && fromBoard) => letter is invalid because of board (this letter is ontop other from board)
	// if(!isValid && !fromBoard) => letter is invalid because of player (player didn't have that letter)
};


struct PointsNWord
{
	int32_t points = 0;
	char word[BoardSize + 1] = { 0 };
};


struct BoundingBox
{
	Vec2i topLeft = BoardMaxIndices;
	Vec2i botRight = BoardMinIndices;
};


class Board
{


	struct Cell
	{
		char letter = BlankTile;
		BoardCellType type = BoardCellType::Normal;
	};


public:

	explicit Board(char const* dictionarySrc);

	Board(Board const&) = delete;


	void moveCursor(Vec2i const & moveVec, int32_t wordLen);

	void resetCursor();

	void changeOrient();

	void reset();


	// returns false if the word cannot be placed
	bool placeWordAtCursor(char const* word, Player & player);

	bool placeWord(Vec2i const& pos, char const* word, Orientation ori, Player & player);

	bool canPlaceWordSomewhere(char const* word, Player const& player) const;

	// returns points for all words created by placing "word" on a board at "pos" with "orientation" by "player"
	// returns 0 when the word cannot be placed (invalid words are created, or player is out of needed letters
	int32_t pointsForWordsAt(Vec2i const& pos, char const* word, Player const& player, Orientation orientation) const;


	bool saveTo(FILE* file) const;

	bool loadFrom(FILE* file);


	void drawBoard() const;

	void drawInputWord(char const* word, Player const& player, bool learningMode) const;


	Dictionary const& getDictionary() const;

	BoundingBox getBoundingBox() const;

private:

	// returns points for a word created by placing "word" on a board at "pos" with "orientation"
	// (only this single word, or extended version of this word if it's placed next to existing letters)
	// when points < 0, then created word is not a word from dictionary
	PointsNWord pointsForSingleWordAt(Vec2i const& pos, char const* word, Orientation orient) const;


	ValidatedWord validateWordAt(Vec2i pos, char const* word, Player player, Orientation orientation) const;


	bool wordPassesBoardStart(Vec2i const& pos, Orientation ori, int32_t wordLength) const;


	void drawBoardPanelFrames() const;

	void drawBoardPanelContent() const;

	void drawCursor() const;

	void drawValidatedWord(char const* word, Player const& player) const;

	void drawLearningModePoints(char const* word) const;

private:

	Cell board[BoardSize][BoardSize];

	Vec2i currentIdx = BoardStartingIdx;	// cursor Index

	Orientation orient = Orientation::Horizontal;

	int32_t lastMovedWordLen = 0; // for orientation change

	bool firstWord = true;


	Dictionary dictionary;
};