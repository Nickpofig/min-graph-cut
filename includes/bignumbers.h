// begins header gurad
#ifndef __include_bignumbers
#define __include_bignumbers
//

#include <stdbool.h>

unsigned int* alloc_big_natural_number
(
    int blocks, 
    unsigned long initial_value
);

unsigned int* alloc_big_natural_number_from_power
(
    int blocks, 
    int power
);

unsigned int* alloc_big_natural_number_from
(
    const char* bitstring, const int length
);

bool add_big_natural_numbers
(
    unsigned int* recepient, 
    unsigned int recepient_length, 
    unsigned int* donor,
    unsigned int donor_length
);

bool add_value_to_big_natural_number
(
    unsigned int* number,
    unsigned int number_blocks,
    unsigned int value
);

int subtract_big_natural_numbers
(
    unsigned int* recepient,
    unsigned int recepient_length,
    unsigned int* donor,
    unsigned int donor_length
);

int compare_big_natural_numbers
(
    const unsigned int* first,
    unsigned int first_length,
    const unsigned int* second,
    unsigned int second_length
);

#endif // ends header guard