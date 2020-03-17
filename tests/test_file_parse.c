#include <stdio.h>
#include <stdbool.h>

#include "file_parse.h"

int main(const int argc, const char** args) 
{
    if (argc == 1) 
    {
        printf("Panic! expects filepath as first argument.");
        exit(-1);
    }

    FILE* file = fopen(args[1], "r");

    if (file == NULL) 
    {
        printf("Panic! could not open file at the path: %s", args[1]);
        exit(-1);
    }

    char* word;

    while(true)
	{
		word = read_word_from(file);
		
		if (word == NULL) 
		{
			break;
		}
		
		printf("%s\n", word);

		free(word);
	}
	
	printf("\n[Done]\n");
}