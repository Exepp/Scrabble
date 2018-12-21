#define _CRT_SECURE_NO_WARNINGS
#include "Dictionary.h"
#include "Math.h"
#include "Console.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>



Dictionary::Dictionary(char const * src)
{
	if (!src)
		src = "dictionary.txt";
	reserve(size_t(16));
	loadFromFile(src);
}

Dictionary::~Dictionary()
{
	if (data)
	{
		for (size_t i = 0; i < count; ++i)
			if(data[i])
				free(data[i]);
		free(data);
	}
}

void Dictionary::loadFromFile(char const * src)
{
	if (!src)
		return;
	if (FILE* file = fopen(src, "r"))
	{
		while (!ferror(file) && !feof(file))
		{
			char word[64];
			fscanf(file, "%63s", word);

			if (strlen(word) <= BoardSize)
			{
				for (int32_t i = 0; word[i]; ++i)
					word[i] = toupper(word[i]);
				addWord(word);
			}
		}
		fclose(file);
		sort();
	}
}

void Dictionary::addWord(char const * word)
{
	if (count < reserved || reserve(reserved * growth))
	{
		data[count] = (char*)malloc(strlen(word) + 1);
		strcpy(data[count], word);
		count++;
	}
}

bool Dictionary::reserve(size_t n)
{
	if (n > reserved)
	{
		reserved = n;
		char** newData = (char**)malloc(reserved * sizeof(char*));
		if (newData)
		{
			for (size_t i = 0; i < count; ++i)
				newData[i] = data[i];
			if(data)
				free(data);
			data = newData;
			return true;
		}
		else
		{
			Console::write("Not enough memory!");
			exit(EXIT_FAILURE);
		}
	}
	return false;
}

//static int comp(void const* l, void const* r)
//{
//	return strcmp(*(char const**)l, *(char const**)r);
//}

void merge(char const ** arr, int32_t l, int32_t m, int32_t r)
{
	int32_t lLen = m - l + 1;
	int32_t rLen = r - m;
	int32_t lIdx = 0, rIdx = 0, oIdx = l; // oIdx - index of arr
	char const ** lCpy = (char const **)malloc(lLen * sizeof(char*));
	char const ** rCpy = (char const **)malloc(rLen * sizeof(char*));


	for (lIdx = 0; lIdx < lLen; lIdx++)
		lCpy[lIdx] = arr[l + lIdx];
	for (rIdx = 0; rIdx < rLen; rIdx++)
		rCpy[rIdx] = arr[m + rIdx + 1];

	for (oIdx = l, lIdx = 0, rIdx = 0; lIdx < lLen && rIdx < rLen; ++oIdx)
		if (strcmp(lCpy[lIdx], rCpy[rIdx]) <= 0)
			arr[oIdx] = lCpy[lIdx++];
		else
			arr[oIdx] = rCpy[rIdx++];

	while (lIdx < lLen)
		arr[oIdx++] = lCpy[lIdx++];
	while (rIdx < rLen)
		arr[oIdx++] = rCpy[rIdx++];

	free(lCpy);
	free(rCpy);
}

void mergeSort(char const** arr, int32_t l, int32_t r)
{
	if (l < r)
	{
		int m = l + (r - l) / 2;

		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);

		merge(arr, l, m, r);
	}
}


void Dictionary::sort()
{
	if (data && count)
		mergeSort((char const**)data, 0, count - 1);
		//qsort(data, count, sizeof(*data), comp);
}

bool Dictionary::isWord(char const * word) const
{
	if (!data || !count)
		return true;
	return strlen(word) && bSearch(word);
}

char const* Dictionary::bSearch(char const * word) const
{
	int32_t l = 0;
	int32_t r = count - 1;

	while (l <= r)
	{
		int32_t m = (l + r) / 2;
		int32_t cmp = strcmp(word, data[m]);

		if (cmp < 0)
			r = m - 1;
		else if (cmp > 0)
			l = m + 1;
		else
			return data[m];
	}
	return nullptr;
}

char const ** Dictionary::getWords() const
{
	return (char const**)data;
}

size_t Dictionary::getWordsCount() const
{
	return count;
}
