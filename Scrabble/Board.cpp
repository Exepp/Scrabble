#include "Board.h"

template<class T>
inline void drawLine(int32_t len, T c, int32_t cInc, Vec2i pos, Vec2i const& posInc)
{
	for (int i = 0; i < len; ++i, c += cInc, pos += posInc)
	{
		Console::setCursorPosition(pos);
		Console::write(c);
	}
}



Board::Board(char const* dictionarySrc) : dictionary(dictionarySrc)
{
	// filling board

	for (int32_t i = 0; i < (BoardSize + 1) / 2; ++i)
		for (int32_t j = 0; j < (BoardSize + 1) / 2; ++j)
		{
			board[i][j].type = BoardCellType(BoardCellsSetup[i][j]); // top left
			board[i][BoardSize / 2 + j].type = BoardCellType(BoardCellsSetup[i][(BoardSize - 1)/ 2 - j]); // top right
			board[BoardSize / 2 + i][BoardSize / 2 + j].type = BoardCellType(BoardCellsSetup[(BoardSize - 1) / 2 - i][(BoardSize - 1) / 2 - j]); // bottom right
			board[BoardSize / 2 + i][j].type = BoardCellType(BoardCellsSetup[(BoardSize - 1) / 2 - i][j]); // bottom left
		}
	reset();
}

void Board::moveCursor(Vec2i const& moveVec, int32_t wordLen)
{
	lastMovedWordLen = wordLen;
	currentIdx += moveVec;
	currentIdx.clamp(BoardMinIndices, BoardMaxIndices - (wordLen - 1) * Orients[(uint8_t)orient]);
}

void Board::resetCursor()
{
	currentIdx = BoardMinIndices;
	orient = Orientation::Horizontal;
}

void Board::reset()
{
	resetCursor();
	lastMovedWordLen = 0;
	firstWord = true;

	for (int32_t i = 0; i < BoardSize; ++i)
		for (int32_t j = 0; j < BoardSize; ++j)
			board[i][j].letter = BlankTile;
}

void Board::changeOrient()
{
	orient = Orientation(!(uint8_t)orient);
	moveCursor({0,0}, lastMovedWordLen);
}

bool Board::placeWordAtCursor(char const * word, Player& player)
{
	return placeWord(currentIdx, word, orient, player);
}

bool Board::placeWord(Vec2i const & pos, char const * word, Orientation ori, Player & player)
{
	int32_t wordLen = strlen(word);
	if (int32_t points = pointsForWordsAt(pos, word, player, ori))
	{
		// add word to the board and remove letters the from player
		Vec2i idx = pos;
		for (int32_t i = 0; i < wordLen; ++i, idx += Orients[(uint8_t)ori])
			if (board[idx.y][idx.x].letter != word[i])
			{
				board[idx.y][idx.x].letter = word[i];
				player.removeLetter(word[i]);
			}
		player.addPoints(points);

		firstWord = false;
		return true;
	}
	return false;
}

bool Board::canPlaceWordSomewhere(char const * word, Player const & player) const
{
	int32_t wordLen = strlen(word);
	if (wordLen < 2)
		return 0;

	for (int32_t i = 0; i < BoardSize; ++i)
		for (int32_t j = 0; j < BoardSize - (wordLen - 1); ++j)
			if (pointsForWordsAt(Vec2i(j, i), word, player, Orientation::Horizontal))
				return true;

	for (int32_t i = 0; i < BoardSize - (wordLen - 1); ++i)
		for (int32_t j = 0; j < BoardSize; ++j)
			if (pointsForWordsAt(Vec2i(j, i), word, player, Orientation::Vertical))
				return true;

	return 0;
}

int32_t Board::pointsForWordsAt(Vec2i const& pos, char const * word, Player const & player, Orientation ori) const
{
	if (!dictionary.isWord(word))
		return 0;

	ValidatedWord vWord = validateWordAt(pos, word, player, ori);
	int32_t wordLen = strlen(word);
	int32_t boardTilesUsed = 0;
	int32_t pointsSum = 0;

	// check if all letters are valid
	for (int32_t i = 0; i < wordLen; ++i)
		if (vWord.isValid[i])
		{
			if (vWord.fromBoard[i])
				++boardTilesUsed;
		}
		else
			return 0;

	if (wordLen - boardTilesUsed == MaxPlayerLetters)
		pointsSum += OneMoveAllLettersBonus;

	// check if this word and other created words (with this one on board) are actually words from dictionary
	bool areWords = true;
	bool anyExtraWord = false;

	PointsNWord mainWord = pointsForSingleWordAt(pos, word, ori);
	areWords &= mainWord.points > 0;
	pointsSum += mainWord.points;

	Vec2i idx = pos;
	for (int32_t i = 0; i < wordLen && areWords; ++i, idx += Orients[(uint8_t)ori])
		if (board[idx.y][idx.x].letter != word[i])
		{
			char buf[2] = { word[i], '\0' };
			int32_t result = pointsForSingleWordAt(idx, buf, Orientation(!uint8_t(ori))).points;
			areWords &= result >= 0; // if no word was created, it's not an error (then it may be also == 0)
			anyExtraWord |= result > 0;
			pointsSum += result;
		}

	if (areWords && ((firstWord && wordPassesBoardStart(pos, ori, wordLen)) || anyExtraWord || ((boardTilesUsed || strcmp(word, mainWord.word) != 0) && (boardTilesUsed < wordLen))))
		return pointsSum;
	return 0;
}

PointsNWord Board::pointsForSingleWordAt(Vec2i const& pos, char const * word, Orientation ori) const
{
	PointsNWord wholeWord;
	int32_t wholeWordLen = 0;
	Vec2i wStart;
	Vec2i idx;

	// look for a whole word
	// add letters before oryginal word

	idx = pos - Orients[(uint8_t)ori];
	for (idx; idx.x >= BoardMinIndices.x && idx.y >= BoardMinIndices.y && board[idx.y][idx.x].letter != BlankTile; idx -= Orients[(uint8_t)ori])
	{
		char l[2] = { board[idx.y][idx.x].letter, '\0' };
		strcat(wholeWord.word, l);
	}
	_strrev(wholeWord.word); // letters were added in reversed order, string reverse is needed
	wStart = idx + Orients[(uint8_t)ori]; // idx is currently one index before first letter of this word, move it one forward

	// add oryginal word
	strcat(wholeWord.word, word);

	// add letters after oryginal word
	idx = pos + (int32_t)strlen(word) * Orients[(uint8_t)ori];
	for (idx; idx.x <= BoardMaxIndices.x && idx.y <= BoardMaxIndices.y && board[idx.y][idx.x].letter != BlankTile; idx += Orients[(uint8_t)ori])
	{
		char l[2] = { board[idx.y][idx.x].letter, '\0' };
		strcat(wholeWord.word, l);
	}

	wholeWordLen = strlen(wholeWord.word);

	if (wholeWordLen < 2) // when the word's length is < 2, it's not a word
	{
		wholeWord.points = 0;
		return wholeWord;
	}

	if (!dictionary.isWord(wholeWord.word))
	{
		wholeWord.points = -1;
		return wholeWord;
	}

	// sum up the points
	int32_t wordMulti = 1;

	idx = wStart;
	for (int32_t wI = 0, oI = 0; wI < wholeWordLen; ++wI, idx += Orients[(uint8_t)ori])	// wI -> whole word index, oI -> oryginal word index
		if ((idx - pos).clamp(Vec2i(-1, -1), Vec2i(1, 1)) != -Orients[(uint8_t)ori] && oI < (int32_t)strlen(word))
		{
			if (board[idx.y][idx.x].letter != word[oI])
			{
				int32_t letterMulti = 1;
				if (board[idx.y][idx.x].type != BoardCellType::LesserWordMulti && board[idx.y][idx.x].type != BoardCellType::GreaterWordMulti)
					letterMulti = BoardMultipliers[(uint8_t)board[idx.y][idx.x].type];
				else
					wordMulti *= BoardMultipliers[(uint8_t)board[idx.y][idx.x].type];

				wholeWord.points += LettersValues[word[oI] - 'A'] * letterMulti;
			}
			else
				wholeWord.points += LettersValues[word[oI] - 'A'];
			++oI;
		}
		else
			wholeWord.points += LettersValues[wholeWord.word[wI] - 'A'];
	wholeWord.points *= wordMulti;

	return wholeWord;
}

ValidatedWord Board::validateWordAt(Vec2i idx, char const * word, Player player, Orientation ori) const
{
	ValidatedWord validWord;
	int32_t	wordLen = strlen(word);

	for (int32_t i = 0; i < wordLen; ++i, idx += Orients[(uint8_t)ori])
		if (board[idx.y][idx.x].letter == word[i])
		{
			validWord.isValid[i] = true;
			validWord.fromBoard[i] = true;
		}
		else if (board[idx.y][idx.x].letter == BlankTile && player.removeLetter(word[i]))
		{
			validWord.isValid[i] = true;
			validWord.fromBoard[i] = false;
		}
		else
		{
			validWord.isValid[i] = false;

			if (board[idx.y][idx.x].letter == BlankTile)
				validWord.fromBoard[i] = false;
			else
				validWord.fromBoard[i] = true;
		}
	return validWord;
}

bool Board::wordPassesBoardStart(Vec2i const& pos, Orientation ori, int32_t wordLength) const
{
	if (ori == Orientation::Horizontal)
		return pos.y == BoardStartingIdx.y && pos.x <= BoardStartingIdx.x && pos.x + wordLength - 1 >= BoardStartingIdx.x;
	return pos.x == BoardStartingIdx.x && pos.y <= BoardStartingIdx.y && pos.y + wordLength - 1 >= BoardStartingIdx.y;
}


bool Board::saveTo(FILE * file) const
{
	bool success = fwrite(&firstWord, sizeof(firstWord), 1, file);
	for (int32_t i = 0; i < BoardSize; i++)
		success &= fwrite(board[i], sizeof(*board[i]), BoardSize, file) == BoardSize;
	return success;
}

bool Board::loadFrom(FILE * file)
{
	bool success =	fread(&firstWord, sizeof(firstWord), 1, file);
	for (int32_t i = 0; i < BoardSize; i++)
		success &= fread(board[i], sizeof(*board[i]), BoardSize, file) == BoardSize;
	return success;
}


void Board::drawBoard() const
{
	drawBoardPanelFrames();
	drawBoardPanelContent();
}

void Board::drawInputWord(char const * word, Player const& player, bool learningMode) const
{
	drawCursor();
	drawValidatedWord(word, player);

	if (learningMode)
		drawLearningModePoints(word);
}

void Board::drawCursor() const
{
	char posTxt[50] = "Current Position : x = %c, y = %d";
	sprintf_s(posTxt, posTxt, 'A' + currentIdx.x, currentIdx.y + 1);
	Console::write(posTxt, BoardPanelFramePos + Vec2i(0, 2 * HFSize.y + VFSize.y + 1));
}

void Board::drawValidatedWord(char const * word, Player const& player) const
{
	ValidatedWord vWord = validateWordAt(currentIdx, word, player, orient);
	int32_t	wordLen = strlen(word);
	Vec2i idx = currentIdx;
	Color bColor = Color::Green;
	char letter = BlankTile;

	bool allFromPlayer = true;
	for (int32_t i = 0; i < wordLen; i++)
		allFromPlayer &= !vWord.fromBoard[i];

	for (int32_t i = 0; i < wordLen; ++i, idx += Orients[(uint8_t)orient])
	{
		if (!vWord.isValid[i]/* || allPointsFromWordAt(currentIdx, word, player, orient) <= 0*/)
			bColor = Color::Lightred;
		else
			bColor = Color::Green;
		if (vWord.fromBoard[i])
			letter = board[idx.y][idx.x].letter;
		else
			letter = word[i];
		Console::write(letter, BoardPanelContentPos + Vec2i(idx.x * 2, idx.y), Color::CurrentColor, bColor);
	}
}

void Board::drawLearningModePoints(char const * word) const
{
	Console::setCursorPosition(InputPanelPos);

	int32_t wordMulti[BoardSize];
	size_t wordMultis = 0;

	Console::write("Points: (");

	Vec2i idx = currentIdx;
	for (int32_t i = 0; i < (int32_t)strlen(word); ++i, idx += Orients[(uint8_t)orient])
	{
		char str[10];
			
		sprintf(str, "%d", LettersValues[word[i] - 'A']);
		Console::write(str);

		sprintf(str, "(%c)", word[i]);
		Console::write(str);

		if (board[idx.y][idx.x].letter != word[i])
		{
			if (board[idx.y][idx.x].type != BoardCellType::LesserWordMulti && board[idx.y][idx.x].type != BoardCellType::GreaterWordMulti)
				sprintf(str, "*%d", BoardMultipliers[(uint8_t)board[idx.y][idx.x].type]);
			else
			{
				strcpy(str, "*1");
				wordMulti[wordMultis++] = BoardMultipliers[(uint8_t)board[idx.y][idx.x].type];
			}
		}
		else
			strcpy(str, "*1");

		Console::write(str);			

		if (i != strlen(word) - 1)
			Console::write("+");
	}

	Console::write(")");

	for (size_t i = 0; i < wordMultis; ++i)
	{
		Console::write("*");
		Console::write(wordMulti[i]);
	}
}

void Board::drawBoardPanelFrames() const
{
	// top down left right
	drawLine(BoardSize, 'A', 1, { BoardPanelFramePos.x + VFSize.x, BoardPanelFramePos.y }, { 2,0 });
	drawLine(BoardSize, 'A', 1, { BoardPanelFramePos.x + VFSize.x, BoardPanelFramePos.y + VFSize.y + HFSize.y + 1 }, { 2,0 });
	drawLine(BoardSize, 1, 1, { BoardPanelFramePos.x, BoardPanelFramePos.y + HFSize.y }, { 0,1 });
	drawLine(BoardSize, 1, 1, { BoardPanelFramePos.x + VFSize.x + HFSize.x + 1, BoardPanelFramePos.y + HFSize.y }, { 0,1 });

	// top down left right
	drawLine(BoardSize, '-', 0, { BoardPanelFramePos.x + VFSize.x, BoardPanelFramePos.y + 1 }, { 2,0 });
	drawLine(BoardSize, '-', 0, { BoardPanelFramePos.x + VFSize.x, BoardPanelFramePos.y + VFSize.y + HFSize.y }, { 2,0 });
	drawLine(BoardSize, '|', 0, { BoardPanelFramePos.x + 2, BoardPanelFramePos.y + HFSize.y }, { 0,1 });
	drawLine(BoardSize, '|', 0, { BoardPanelFramePos.x + VFSize.x + HFSize.x, BoardPanelFramePos.y + HFSize.y }, { 0,1 });
}

void Board::drawBoardPanelContent() const
{
	for (int32_t i = 0; i < BoardSize; ++i)
		for (int32_t j = 0; j < BoardSize; ++j)
			Console::write(board[i][j].letter, BoardPanelContentPos + Vec2i(j * 2, i), Color::White, CellsBackColors[(uint8_t)board[i][j].type]);
	if (firstWord)
		Console::write('#', BoardPanelContentPos + Vec2i(BoardStartingIdx.x * 2, BoardStartingIdx.y), 
			Color::White, CellsBackColors[(uint8_t)board[BoardStartingIdx.y][BoardStartingIdx.x].type]);
}


Dictionary const & Board::getDictionary() const
{
	return dictionary;
}

BoundingBox Board::getBoundingBox() const
{
	if (firstWord)
		return { BoardStartingIdx, BoardStartingIdx };
	BoundingBox box;
	for (int32_t i = 0; i < BoardSize; ++i)
		for (int32_t j = 0; j < BoardSize; ++j)
			if (board[i][j].letter != BlankTile)
			{
				if (box.topLeft.x > j)
					box.topLeft.x = j;
				if (box.topLeft.y > i)
					box.topLeft.y = i;
				if (box.botRight.x < j)
					box.botRight.x = j;
				if (box.botRight.y < i)
					box.botRight.y = i;
			}
	return box;
}
