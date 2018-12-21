#include "Player.h"
#include "Console.h"
#include <ctype.h>
#include <stdlib.h>

Player::Player(char freeLetters[], int32_t& freeCount)
{
	addLetters(freeLetters, freeCount);
}

void Player::draw(int32_t nr, bool exchange, bool const lettersToExchange[MaxPlayerLetters]) const
{
	char playerTxt[40];
	strcpy_s(playerTxt, "Player%d:\n" "Score: %d\n");
	sprintf_s(playerTxt, playerTxt, nr, score);

	Console::setCursorPosition(PlayersDataPanelPos);
	Console::write(playerTxt);
	Console::write("Letters:", Vec2i(), Color::CurrentColor, exchange ? Color::Green : Color::CurrentColor);
	Console::write(' ', Vec2i(), Color::CurrentColor, Color::CurrentColor);
	for (int32_t i = 0; i < lCount; ++i)
	{
		char letter[] = { letters[i], '\0' };
		Color bColor = lettersToExchange[i] ? Color::Green : Color::Black;
		Console::write(letter, Vec2i(), Color::CurrentColor, bColor);
		Console::write(' ', Vec2i(), Color::CurrentColor, Color::CurrentColor);
	}
}

bool Player::removeLetter(char letter)
{
	letter = toupper(letter);
	char* addr = strchr(letters, letter);
	if (addr || (addr = strchr(letters, '_')))
	{
		--lCount;
		for (; addr < letters + lCount; ++addr)
			*addr = *(addr + 1);
		*addr = '\0';
		return true;
	}
	return false;
}

void Player::addLetters(char freeLetters[], int32_t& freeCount)
{
	for (int i = 0; i < freeCount; i++)
		if (freeLetters[i] == 0)
			throw("error");

	for (int32_t i = 0; i < lCount; ++i)
		if (letters[i] == ' ')
			letters[i] = rollCharFromFree(freeLetters, freeCount);

	for (lCount; lCount < MaxPlayerLetters && freeCount > 0; ++lCount)
		letters[lCount] = rollCharFromFree(freeLetters, freeCount);
}

char Player::rollCharFromFree(char freeLetters[], int32_t & freeCount)
{
	int32_t idx = myRand() % freeCount--;
	char c = freeLetters[idx];
	freeLetters[idx] = freeLetters[freeCount];
	freeLetters[freeCount] = '\0';
	return c;
}

int Player::myRand()
{
	++randUsed;
	return rand();
}

void Player::exchangeLetters(bool lettersToExchange[MaxPlayerLetters], char freeLetters[], int32_t freeCount)
{
	if (!freeCount)
		return;
	// return letters to free ones
	for(int32_t i = 0; i < lCount; ++i)
		if (lettersToExchange[i])
		{
			freeLetters[freeCount++] = letters[i];
			letters[i] = ' ';
		}
	// fill with new
	addLetters(freeLetters, freeCount);

	for (int32_t i = 0; i < MaxPlayerLetters; ++i)
		lettersToExchange[i] = false;
}

void Player::addPoints(int32_t points)
{
	score += points;
}

bool Player::saveTo(FILE * file) const
{
	return	fwrite(&score, sizeof(score), 1, file) && fwrite(&lCount, sizeof(lCount), 1, file) &&
			fwrite(letters, sizeof(*letters), sizeof(letters) / sizeof(*letters), file);
}

bool Player::loadFrom(FILE * file)
{
	return	fread(&score, sizeof(score), 1, file) && fread(&lCount, sizeof(lCount), 1, file) &&
			fread(letters, sizeof(*letters), sizeof(letters) / sizeof(*letters), file);
}

int32_t Player::getScore() const
{
	return score;
}

int32_t Player::getLettersCount() const
{
	return lCount;
}

size_t Player::getRandUsed() const
{
	return randUsed;
}
