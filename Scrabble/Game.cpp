#include "Game.h"
#include <time.h>
#include <stdlib.h>
#include <ctype.h>


template<size_t N>
inline void drawPanel(char const* const (&panel)[N])
{
	for (int i = 0; i < N; ++i)
		Console::write(panel[i]);
}



Game::Game(char const* dictionarySrc) : randSeed(size_t(time(NULL))), board(dictionarySrc)
{
	srand(randSeed);
	Console::setTextColor(Color::White);
	Console::setTextBackgroundColor(Color::Black);
	resetGame();
}

int32_t Game::run()
{	
	while (stage != Stage::End)
	{
		draw();
		handleInput();
		if (stage == Stage::PlayerChange)
		{
			(++currPlayerIdx) %= PlayersCount;
			stage = Stage::MainMenu;
		}
	}
	return 0;
}

void Game::draw()
{
	Console::clear();
	drawLegendPanel();
	board.drawBoard();
	players[currPlayerIdx].draw(currPlayerIdx + 1, stage == Stage::LettersExchange, lettersToExchange);
	drawIntputPanel();
	if (stage == Stage::WordPlacement)
		board.drawInputWord(inputStr, players[currPlayerIdx], learningMode);
	drawMsgPanel();
}

void Game::drawLegendPanel()
{
	Console::setCursorPosition(LegendPanelPos);
	drawPanel(LegendPanelTxt);
}

void Game::drawIntputPanel()
{
	Console::setCursorPosition(InputPanelPos);
	if (stage == Stage::Loading || stage == Stage::Saving || stage == Stage::WordWriting)
	{
		if (stage == Stage::Loading)
			Console::write(InputPanel_LoadTxt);
		else if (stage == Stage::Saving)
			Console::write(InputPanel_SaveTxt);
		else
			Console::write(InputPanel_NewWordTxt);

		if (stage == Stage::WordWriting)
			drawNewWordInput();	// with validation
		else
			Console::write(inputStr);
	}
}

void Game::drawNewWordInput()
{
	Player pCpy = players[currPlayerIdx];
	for (int32_t i = 0; i < int32_t(strlen(inputStr)); i++)
		Console::write(char(inputStr[i]), Vec2i(), Color::CurrentColor, pCpy.removeLetter(inputStr[i]) ? Color::Green : Color::Red);
	if (learningMode)
	{
		Console::setCursorPosition(Vec2i(InputPanelPos.x, Console::getCursorPosition().y + 1));
		Console::write("Found in dictionary: ");
		bool isWord = board.getDictionary().isWord(inputStr);
		Console::write(isWord ? "true" : "false");
	}

}

void Game::drawMsgPanel()
{
	if (stage == Stage::PrintMsg)
	{
		Console::setCursorPosition(MsgPanelPos);

		Console::write(MsgPanel_TopTxt);
		if (message == Message::SavingError)
			Console::write(MsgPanel_SavingTxt);
		else if (message == Message::LoadingError)
			Console::write(MsgPanel_LoadingTxt);
		else if(message == Message::WordPlacementError)
			Console::write(MsgPanel_WordPlacementTxt);
		else if (message == Message::AiMoved)
		{
			char buffer[sizeof(MsgPanel_AiPlayerMovedTxt) / sizeof(*MsgPanel_AiPlayerMovedTxt)];
			sprintf(buffer, MsgPanel_AiPlayerMovedTxt, currPlayerIdx + 1);
			Console::write(buffer);
		}
		else if (message == Message::GameOver)
		{
			char buffer[sizeof(MsgPanel_GameOverTxt) / sizeof(*MsgPanel_GameOverTxt)];
			int32_t maxP = players[0].getScore();
			int32_t idx = 0;

			for (int32_t i = 1; i < PlayersCount; ++i)
				if (players[i].getScore() > maxP)
				{
					maxP = players[i].getScore();
					idx = i;
				}
			sprintf(buffer, MsgPanel_GameOverTxt, idx + 1);
			Console::write(buffer);
		}
		Console::write(MsgPanel_BotTxt);
	}
}


void Game::handleInput()
{
	if (stage == Stage::MainMenu && currPlayerIdx >= PlayersCount - AiPlayers && players[currPlayerIdx].getLettersCount())
	{
		aiMove();
		//stage = Stage::PlayerChange;
		stage = Stage::PrintMsg;
		message = Message::AiMoved;
	}
	else
	{
		Input input = Console::getInput();
		if (stage == Stage::MainMenu)
			mainMenuInput(input);
		else if (stage == Stage::Saving)
			saveFileWrittingInput(input);
		else if (stage == Stage::Loading)
			loadFileWrittingInput(input);
		else if (stage == Stage::WordWriting)
			wordWrittingInput(input);
		else if (stage == Stage::WordPlacement)
			wordPlacementInput(input);
		else if (stage == Stage::LettersExchange)
			lettersExchangeInput(input);
		else if (stage == Stage::PrintMsg)
			msgInput(input);
	}
	if (stage != Stage::GameOver)
	{
		for (auto const& p : players)
			if (!p.getLettersCount())
			{
				message = Message::GameOver;
				stage = Stage::PrintMsg;
			}
	}
	else
		resetGame();
}

void Game::mainMenuInput(Input const& input)
{
	if (input.isSpecial)
		return;
	if (input.code == 'q')
		stage = Stage::End;
	else if (input.code == 'i' || input.code == 's' || input.code == 'l')
	{
		if (input.code == 'i')
			stage = Stage::WordWriting;
		else if (input.code == 's')
			stage = Stage::Saving;
		else
			stage = Stage::Loading;
		inputStr[0] = '\0';
	}
	else if (input.code == 'n')
		resetGame();
	else if (input.code == 'w')
		stage = Stage::LettersExchange;
	else if (input.code == 't')
		learningMode = !learningMode;
}

void Game::saveFileWrittingInput(Input const & input)
{
	if (input.isSpecial)
		return;
	if (input.code == (int32_t)Input::NormalKeys::Enter)
	{
		if(saveGame())
			stage = Stage::MainMenu;
		else
		{
			stage = Stage::PrintMsg;
			message = Message::SavingError;
		}
	}
	else
		handleStringInput(input, sizeof(inputStr));
}

void Game::loadFileWrittingInput(Input const & input)
{
	if (input.isSpecial)
		return;
	if (input.code == (int32_t)Input::NormalKeys::Enter)
	{
		if (LoadGame())
			stage = Stage::MainMenu;
		else
		{
			stage = Stage::PrintMsg;
			message = Message::LoadingError;
		}
	}
	else
		handleStringInput(input, sizeof(inputStr));
}

void Game::wordWrittingInput(Input const& input)
{
	if (input.isSpecial)
		return;
	if (input.code == (int32_t)Input::NormalKeys::Enter)
		if (board.canPlaceWordSomewhere(inputStr, players[currPlayerIdx]))
		{
			stage = Stage::WordPlacement;
			board.resetCursor();
		}
		else
		{
			stage = Stage::PrintMsg;
			message = Message::WordPlacementError;
		}
	else
		handleStringInput(input, MaxInputWordLength + 1);
}

void Game::handleStringInput(Input const& input, int32_t maxLen)
{
	if (input.isSpecial)
		return;

	int32_t len = strlen(inputStr);

	if (input.code == (int32_t)Input::NormalKeys::Esc)
		stage = Stage::MainMenu;
	else if (input.code == (int32_t)Input::NormalKeys::Backspace)
	{
		if (len)
			inputStr[len - 1] = '\0';
	}
	else if ((input.code >= 'a' && input.code <= 'z') || (input.code >= 'A' && input.code <= 'Z'))
		if (len + 1 < maxLen)
		{
			inputStr[len] = toupper(input.code);
			inputStr[len + 1] = '\0';
		}
}

void Game::wordPlacementInput(Input const& input)
{
	if (input.isSpecial)
	{
		int32_t wordLen = strlen(inputStr);
		if (input.code == int32_t(Input::SpecialKeys::LeftArrow))
			board.moveCursor({ -1, 0 }, wordLen);
		else if (input.code == int32_t(Input::SpecialKeys::RightArrow))
			board.moveCursor({ 1, 0 }, wordLen);
		else if (input.code == int32_t(Input::SpecialKeys::UpArrow))
			board.moveCursor({ 0, -1 }, wordLen);
		else if (input.code == int32_t(Input::SpecialKeys::DownArrow))
			board.moveCursor({ 0, 1 }, wordLen);
	}
	else if (input.code == (int32_t)Input::NormalKeys::Enter)
		if (!board.placeWordAtCursor(inputStr, players[currPlayerIdx]))
		{
			stage = Stage::PrintMsg;
			message = Message::WordPlacementError;
		}
		else
		{
			players[currPlayerIdx].addLetters(freeLetters, freeCount);
			stage = Stage::PlayerChange;
		}
	else if (input.code == (int32_t)Input::NormalKeys::Esc)
		stage = Stage::MainMenu;
	else if (input.code == 'o')
		board.changeOrient();
}

void Game::lettersExchangeInput(Input const& input)
{
	if (input.isSpecial)
		return;
	if (input.code >= '1' && input.code < '1' + minVal(players[currPlayerIdx].getLettersCount(), 9))
		lettersToExchange[input.code - '1'] = !lettersToExchange[input.code - '1'];
	else if (input.code == (int32_t)Input::NormalKeys::Enter || input.code == (int32_t)Input::NormalKeys::Esc || input.code == 'w')
	{
		if (input.code == (int32_t)Input::NormalKeys::Esc)
			stage = Stage::MainMenu;
		else
		{
			stage = Stage::PlayerChange;
			players[currPlayerIdx].exchangeLetters(lettersToExchange, freeLetters, freeCount);
		}
		for (auto& b : lettersToExchange) b = false;
	}
}

void Game::msgInput(Input const & input)
{
	if (input.code == (int32_t)Input::NormalKeys::Enter)
	{
		if (message == Message::WordPlacementError || message == Message::AiMoved)
			stage = Stage::PlayerChange;
		else if (message == Message::GameOver)
			stage = Stage::GameOver;
		else
			stage = Stage::MainMenu;
		message = Message::None;
	}
}

void Game::resetGame()
{	
	stage = Stage::MainMenu;
	currPlayerIdx = 0;
	board.reset();

	freeCount = 0;
	for (int32_t i = 0; i < 'Z' - 'A' + 1; ++i)
		for (int32_t j = 0; j < StartingLettersCounts[i]; ++j, ++freeCount)
			freeLetters[freeCount] = 'A' + i;
	// add blank tiles
	for (int32_t i = 0; i < StartingLettersCounts['Z' - 'A' + 1]; ++i, ++freeCount)
		freeLetters[freeCount] = '_';

	for (auto& p : players)
		p = Player(freeLetters, freeCount);
}

bool Game::saveGame()
{
	FILE* file = fopen(inputStr, "wb");
	bool success = file && saveTo(file) && board.saveTo(file);
	if (success)
	{
		for (int32_t i = 0; i < PlayersCount; ++i)
			success &= players[i].saveTo(file);
		fclose(file);
	}
	return success;
}

bool Game::LoadGame()
{
	FILE* file = fopen(inputStr, "rb");
	bool success = file && loadFrom(file) && board.loadFrom(file);;
	if (success)
	{
		for (int32_t i = 0; i < PlayersCount; ++i)
			success &= players[i].loadFrom(file);
		fclose(file);
	}
	return success;
}

bool Game::saveTo(FILE * file)
{
	for (int32_t i = 0; i < PlayersCount; ++i)
		randUsed += players[i].getRandUsed();

	return	fwrite(&currPlayerIdx, sizeof(currPlayerIdx), 1, file) &&
			fwrite(&randSeed, sizeof(randSeed), 1, file) &&
			fwrite(&randUsed, sizeof(randUsed), 1, file) &&
			fwrite(&freeCount, sizeof(freeCount), 1, file) &&
			fwrite(freeLetters, sizeof(*freeLetters), StartingLettersCount(), file) == StartingLettersCount();
}

bool Game::loadFrom(FILE * file)
{
	bool success = 	fread(&currPlayerIdx, sizeof(currPlayerIdx), 1, file) &&
					fread(&randSeed, sizeof(randSeed), 1, file) &&
					fread(&randUsed, sizeof(randUsed), 1, file) &&
					fread(&freeCount, sizeof(freeCount), 1, file) &&
					fread(freeLetters, sizeof(*freeLetters), StartingLettersCount(), file) == StartingLettersCount();
	if (success)
	{
		srand(randSeed);
		for (size_t i = 0; i < randUsed; ++i)
			rand();
	}
	return success;
}

void Game::aiMove()
{
	Dictionary const& dic = board.getDictionary();
	const char* bestWord = "";
	int32_t bestScore = 0;
	Vec2i bestIdx;
	Orientation bestOrient = Orientation::Horizontal;
	BoundingBox box = board.getBoundingBox();
	box.topLeft -= Vec2i(1, 1); box.topLeft.clamp(BoardMinIndices, BoardMaxIndices);
	box.botRight += Vec2i(1, 1); box.botRight.clamp(BoardMinIndices, BoardMaxIndices);

	for (int32_t i = box.topLeft.y; i <= box.botRight.y; ++i)
		for (int32_t j = 0; j < BoardSize - 2; ++j)
			for (int32_t k = 0; k < (int32_t)dic.getWordsCount(); ++k)
			{
				if (j + strlen(dic.getWords()[k]) - 1 >= BoardSize)
					continue;
				int32_t score = board.pointsForWordsAt(Vec2i(j, i), dic.getWords()[k], players[currPlayerIdx], Orientation::Horizontal);
				if (score > bestScore)
				{
					bestWord = dic.getWords()[k];
					bestScore = score;
					bestIdx = Vec2i(j, i);
					bestOrient = Orientation::Horizontal;
				}
			}

	for (int32_t i = 0; i < BoardSize - 2; ++i)
		for (int32_t j = box.topLeft.x; j <= box.botRight.x; ++j)
			for (int32_t k = 0; k < (int32_t)dic.getWordsCount(); ++k)
			{
				if (j + strlen(dic.getWords()[k]) - 1 >= BoardSize)
					continue;
				int32_t score = board.pointsForWordsAt(Vec2i(j, i), dic.getWords()[k], players[currPlayerIdx], Orientation::Vertical);
				if (score > bestScore)
				{
					bestWord = dic.getWords()[k];
					bestScore = score;
					bestIdx = Vec2i(j, i);
					bestOrient = Orientation::Vertical;
				}
			}

	if (bestScore > 0)
	{
		board.placeWord(bestIdx, bestWord, bestOrient, players[currPlayerIdx]);
		players[currPlayerIdx].addLetters(freeLetters, freeCount);
	}
	else
	{
		++randUsed;
		int32_t toChange = rand() % players[currPlayerIdx].getLettersCount();
		for (int32_t i = 0; i < toChange; ++i)
			lettersToExchange[i] = true;

		players[currPlayerIdx].exchangeLetters(lettersToExchange, freeLetters, freeCount);
	}
}




int32_t main(int32_t argc, char* argv[])
{
	char* directoryStr = argc > 1 ? argv[1] : nullptr;
	Console::init();
	int32_t r = Game(directoryStr).run();
	Console::shutdown();
	return r;
}