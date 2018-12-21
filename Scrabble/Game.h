#pragma once
#include "Board.h"

class Game
{
	enum class Message : uint8_t
	{
		WordPlacementError, SavingError, LoadingError, AiMoved, GameOver, None
	};

	enum class Stage : uint8_t
	{
		MainMenu,
		Saving,
		Loading,
		WordWriting,
		WordPlacement,
		LettersExchange,
		PlayerChange,
		PrintMsg,
		GameOver,
		End
	};

public:

	explicit Game(char const* dictionarySrc);

	int32_t run();

private:

	void draw();

	void drawLegendPanel();

	void drawIntputPanel();

	void drawNewWordInput();

	void drawMsgPanel();


	void handleInput();	

	void mainMenuInput(Input const& input);

	void saveFileWrittingInput(Input const& input);

	void loadFileWrittingInput(Input const& input);	

	void wordWrittingInput(Input const& input);

	void wordPlacementInput(Input const& input);

	void lettersExchangeInput(Input const& input);

	void msgInput(Input const& input);


	void handleStringInput(Input const& input, int32_t maxLen);


	void resetGame();


	bool saveGame();

	bool LoadGame();

	bool saveTo(FILE* file);

	bool loadFrom(FILE* file);


	void aiMove();

private:

	int32_t currPlayerIdx = 0;

	Player players[PlayersCount];

	Board board;

	Stage stage = Stage::MainMenu;

	Message message = Message::None;


	char freeLetters[StartingLettersCount()];

	int32_t freeCount = 0;


	char inputStr[maxVal(32, MaxInputWordLength)] = "";

	bool lettersToExchange[MaxPlayerLetters] = { 0 };

	bool learningMode = false;


	size_t randSeed = 0;

	size_t randUsed = 0;
};