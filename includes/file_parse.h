// begins header guard
#ifndef __include_file_parse
#define __include_file_parse
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool is_whitespace(char letter);

char* read_word_from(FILE* file);

#endif // end header guard