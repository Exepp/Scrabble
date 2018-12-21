#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "Constants.h"

struct Player
{
public:

	Player() = default;

	Player(char freeLetters[], int32_t& freeCount);


	bool removeLetter(char letter);
	
	void addLetters(char freeLetters[], int32_t& freeCount);
	
	void exchangeLetters(bool lettersToExchange[MaxPlayerLetters], char freeLetters[], int32_t freeCount);


	void addPoints(int32_t points);


	bool saveTo(FILE* file) const;
	
	bool loadFrom(FILE* file);


	void draw(int32_t nr, bool exchange, bool const lettersToExchange[MaxPlayerLetters]) const;


	int32_t getScore() const;

	int32_t getLettersCount() const;

	size_t getRandUsed() const;

private:

	char rollCharFromFree(char freeLetters[], int32_t& freeCount);

	int myRand();

private:

	char letters[MaxPlayerLetters + 1] = { 0 };

	int32_t lCount = 0; // letters count

	int32_t score = 0;

	int32_t randUsed = 0;
};

