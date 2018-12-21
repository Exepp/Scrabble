#pragma once

class Dictionary
{
public:

	explicit Dictionary(char const* src = nullptr);

	Dictionary(Dictionary const&) = delete;

	~Dictionary();


	void loadFromFile(char const* src);


	bool isWord(char const* word) const;

	char const** getWords() const;

	size_t getWordsCount() const;

private:

	bool reserve(size_t n);


	void addWord(char const* word);


	void sort();


	char const* bSearch(char const* word) const;


private:

	char** data = nullptr;

	size_t count = 0;

	size_t reserved = 0;

	static size_t const growth = 2;
};

