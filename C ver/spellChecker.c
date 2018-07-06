/*
 * Name: Alex Li
 * Date: 3/8/17
 */

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * calculates the Levenshtein edit distance between two words
 * uses a modified version of the code from this example:
 * https://www.lemoda.net/c/levenshtein/
 * and algorithm logic based on this resource:
 * http://people.cs.pitt.edu/~kirk/cs1501/Pruhs/Spring2006/assignments/editdistance/Levenshtein%20Distance.htm
 * @param first word
 * @param second word
 * @return Levenshtein edit distance for given words
 */
int calcLD(char *word1, char *word2)
{
    int word1Len = strlen(word1);
    int word2Len = strlen(word2);   

    if (word1Len == 0)
        return word2Len;

    if (word2Len == 0)
        return word1Len;

    // construct matrix containing 0...word1Len rows and 0...word2Len columns
    int matrix[word1Len + 1][word2Len + 2];

    // initialize first row to 0...word1Len
    for (int i = 0; i <= word1Len; i++)
        matrix[i][0] = i;

    // initialize first column to 0...word2Len
    for (int i = 0; i <= word2Len; i++)
        matrix[0][i] = i;

    // examine each character of word1
    for (int i = 1; i <= word1Len; i++)
    {
        char w1 = word1[i - 1];

        // examine each character of word2
        for (int c = 0; c <= word2Len; c++)
        {
            char w2 = word2[c - 1];

            if (w1 == w2)
                matrix[i][c] = matrix[i - 1][c - 1];

            else
            {
                int delete = matrix[i - 1][c] + 1;
                int insert = matrix[i][c - 1] + 1;
                int sub = matrix[i - 1][c - 1] + 1;
                int min = delete;

                if (insert < min)
                    min = insert;

                if (sub < min)
                    min = sub;

                matrix[i][c] = min;
            }
        }
    }

    return matrix[word1Len][word2Len];
}


/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    char *next = nextWord(file);
    while (next)
    {
        hashMapPut(map, next, 1);    

        free(next);
        next = nextWord(file);
    }
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    int quit = 0;

    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
        
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }   

        // spell checker logic
        // if the map contains the inputBuffer key, input is spelled correctly
        else if (hashMapContainsKey(map, inputBuffer))
            printf("%s is spelled correctly.\n\n", inputBuffer);

        else
        {
            for (int i = 0; i < hashMapCapacity(map); i++)
            {
                HashLink *temp = map->table[i];

                while (temp)
                {
                    // calculate edit distance between misspelled word and every word in dictionary
                    int levenshtein = calcLD(inputBuffer, temp->key);

                    /* result filters:
                     * levenshtein distance between words is 1 or 2
                     * the length of the suggestion is at least the length of the misspelled word
                     * the first letter of the misspelled word is correct
                    */

                    if ((levenshtein == 1 || levenshtein == 2) && strlen(temp->key) >= strlen(inputBuffer)
                            && temp->key[0] == inputBuffer[0])
                        printf("Did you mean %s?\n", temp->key);

                    temp = temp->next;
                }
            }
        }
    }

    hashMapDelete(map);
    return 0;
}