#include "file_parse.h"

char* read_word_from(FILE* file)
{
    char letter;
    char* word;
    int count;
    int capacity;

    // trims whitespaces at the beginning
    do 
    {
        letter = fgetc(file);
    }
    while (is_whitespace(letter));

    // returns null 
    // when no word has found
    if (letter == EOF) 
    {
        return NULL;
    }

    // allocates buffer with a default size
    word = malloc(sizeof(char) * 16);
    // writes last found letter
    word[0] = letter;
    // starts counting
    count = 1;
    // keeps capacity value for later expansion
    capacity = 16;

    while(true)
    {
        // reads letter
        letter = fgetc(file);

        // stops word building
        // when end of file or a whitespace occurs
        if (feof(file) || is_whitespace(letter)) 
        {
            break;
        }

        // extends buffer size
        // when limit has been reached 
        if (count == capacity) 
        {
            capacity += 16;
            word = realloc(word, sizeof(char) * capacity);
        }

        // writes the letter
        word[count] = letter;

        // continues the counting 
        count++;
    }

    // ensures that word size 
    // equals to the number of letters 
    if (count != capacity) 
    {
        word = realloc(word, count);
    }
    
    return word;
}

bool is_whitespace(char letter)
{
    return letter == ' ' || 
           letter == '\n'||
           letter == '\t'||
           letter == '\r'; 
}