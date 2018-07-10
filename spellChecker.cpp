/*
 * Alex Li
 * spellChecker implementation
 */

#include "hashMap.hpp"
#include <fstream>
#include <ctime>

using std::ifstream;
using std::clock;
using std::cin;

// prototypes
int loadDictionary(string fname, HashMap<string, int> *map);
int calcLD(string word1, string word2);

int main() {
	double start, end, elapsed;
	string dictionaryFile = "dictionary.txt";
	HashMap<string, int> *dictionary = new HashMap<string, int> (1000);

	cout << "Loading dictionary file..." << endl;
	// load dictionary into hash map
	start = clock();
	int loadStatus = loadDictionary(dictionaryFile, dictionary);
	end = clock();
	elapsed = end - start;
	elapsed /= CLOCKS_PER_SEC;

	if (loadStatus == -1) {
		cout << "Failed to load dictionary file!" << endl;
		delete dictionary;
		return 1;
	}

	cout << "Dictionary loaded in " << elapsed << " seconds." << endl;

	string inputbuffer = "";
	bool quit = false;

	while (!quit) {
		cout << "Enter a word to spell check or \"quit\" to exit: ";
		cin >> inputbuffer;

		if (inputbuffer.compare("quit") == 0)
			quit = true;

		else if (dictionary->mapContains(inputbuffer))
			cout << "\n\"" << inputbuffer << "\"" << " is spelled correctly.\n" << endl;

		else {
			cout << "\nDid you mean: " << endl;
			for (int i = 0; i < dictionary->mapCapacity(); i++) {
				HashLink<string, int> *seeker = dictionary->mapTableLink(i);
				while (seeker) {
					string seekerKey = seeker->getKey();
					/* result filters:
                     * the length of the suggestion is at least the length of the misspelled word
                     * the first letter of the misspelled word is correct
                     * levenshtein distance between words is 1 or 2
                     */
					if (seekerKey.length() >= inputbuffer.length() && seekerKey[0] == inputbuffer[0]) {
						// calculate edit distance between mispelled word and filtered words
						int LD = calcLD(inputbuffer, seeker->getKey());
                    	if ((LD == 1 || LD == 2)) 
                    		cout << seekerKey << endl;
                	}

                	seeker = seeker->getNext();
				}
			}
			cout << endl;
		} 
	}

	delete dictionary;
	return 0;
}

// function implementation
int loadDictionary(string fname, HashMap<string, int> *map) {
	string inputbuffer = "";
	ifstream dictionaryFile(fname);
	if (dictionaryFile.is_open()) {
		while (!dictionaryFile.eof()) {
			getline(dictionaryFile, inputbuffer);
			map->mapPut(inputbuffer, 1);
		}
		dictionaryFile.close();	
		return 0;
	}

	else
		return -1;
}

int calcLD(string word1, string word2) {
	int word1Len = word1.length();
	int word2Len = word2.length();

	if (word1Len == 0)
		return word2Len;

	if (word2Len == 0)
		return word1Len;

	// construct matrix containing 0...word1Len + 1 rows and 0...word2Len + 1 columns
	int matrix[word1Len + 1][word2Len + 1];

	// initialize first column to 0...word1Len
	for (int i = 0; i <= word1Len; i++)
		matrix[i][0] = i;

	// initialize first row to 0...word2Len
	for (int i = 0; i <= word2Len; i++)
		matrix[0][i] = i;

	// examine each character of word1
	for (int i = 1; i <= word1Len; i++) {
		char c1 = word1[i - 1];

		// examine each character of word2
		for (int j = 1; j <= word2Len; j++) {
			char c2 = word2[j - 1];

			if (c1 == c2)
				matrix[i][j] = matrix[i - 1][j - 1];

			else {
				int deletion = matrix[i - 1][j] + 1;
				int insert = matrix[i][j - 1] + 1;
				int sub = matrix[i-1][j - 1] + 1;
				int min = deletion;

				if (insert < min)
					min = insert;

				if (sub < min)
					min = sub;

				matrix[i][j] = min;
			}
		}
	}

	return matrix[word1Len][word2Len];
}