#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

unsigned int* alloc_big_natural_number(int blocks, unsigned long initial_value)
{
    if (blocks <= 0) 
    {
        printf("Panic! could not allocate big natuarl number with zero blocks of memory!");
        exit(-1);
    }

    unsigned int* number = malloc(blocks * sizeof(unsigned int));

    unsigned int block_offset = sizeof(unsigned int) * 8;
    unsigned int shifted_value = initial_value;
    
    shifted_value = shifted_value << block_offset;
    shifted_value = shifted_value >> block_offset;
    
    number[0] = shifted_value;

    if (blocks > 1) 
    {
        number[1] = initial_value >> block_offset;
    }

    for (int i = 2; i < blocks; i++) 
    {
        number[i] = 0;
    }

    return number;
}

unsigned int* alloc_big_natural_number_from_power(int blocks, int power) 
{
    if (blocks <= 0) 
    {
        printf("Panic! could not allocate big natuarl number with zero blocks of memory!");
        exit(-1);
    }

    unsigned int* number = malloc(blocks * sizeof(unsigned int));

    for (int i = 0; i < blocks; i++) 
    {
        number[i] = ~0;
    }

    unsigned int shift = (blocks * sizeof(unsigned int)) - power;

    if (shift > 0) 
    {
        number[blocks - 1] = (number[blocks - 1] << shift) >> shift;
    }

    return number;
}

unsigned int* alloc_big_natural_number_from
(
    const char* bitstring, 
    const int length
)
{
    
    printf("Panic! natural number cast from string is not implemented!");
    exit(-1);

}

bool add_big_natural_numbers
(
    unsigned int* recepient, 
    unsigned int recepient_length, 
    unsigned int* donor,
    unsigned int donor_length
)
{
    unsigned int add_level = recepient_length;
    
    if (add_level > donor_length)
        add_level = donor_length;

    unsigned int small_max = ~0; 
    unsigned long max = small_max,
        recepient_value, 
        donor_value,
        hot_value = 0; 

    for (unsigned int block = 0; block < add_level; block++) 
    {
        recepient_value = recepient[block]; 
        donor_value = donor[block];
        hot_value += recepient_value + donor_value;

        if (hot_value > max) 
        {
            recepient[block] = 0;           
            hot_value = 1;
        }
        else 
        {
            recepient[block] = hot_value;
            hot_value = 0;
        }
    }

    return hot_value == 1;
}

bool add_value_to_big_natural_number
(
    unsigned int* number,
    unsigned int number_blocks,
    unsigned int value
) 
{
    unsigned int max = ~0;
    unsigned long block_value;

    for (unsigned int i = 0; i < number_blocks; i++) 
    {
        block_value = (unsigned long)number[i] + (unsigned long)value;
        if (block_value > max) 
        {
            number[i] = 0;
            value = 1;
        }
        else 
        {
            number[i] = number[i] + value;
            return false;
        }
    }

    return true;
}

int subtract_big_natural_numbers
(
    unsigned int* recepient,
    unsigned int recepient_blocks,
    unsigned int* donor,
    unsigned int donor_blocks
)
{
    unsigned int add_level = recepient_blocks;
    
    if (add_level > donor_blocks)
        add_level = donor_blocks;

    unsigned long donor_value,
        recepient_value,
        borrow = 0;

    unsigned int max = ~0;

    for (unsigned int block = 0; block < add_level; block++) 
    {
        recepient_value = recepient[block]; 
        donor_value = donor[block];

        if (recepient_value >= donor_value + borrow) 
        {
            recepient[block] -= donor_value + borrow;
            borrow = 0;           
        }
        else 
        {
            recepient[block] += (max - donor_value) + 1;
            borrow = 1;
        }
    }

    return borrow == 1;
}

int compare_big_natural_numbers
(
    const unsigned int* first,
    unsigned int first_length,
    const unsigned int* second,
    unsigned int second_length
)
{
    int i = first_length - 1, 
        j = second_length - 1;

    while(i > j) 
    {
        if (first[i] > 0) return 1;
        i--;
    }

    while (j > i)
    {
        if (second[j] > 0) return -1;
        j--;
    }
    
    while (i >= 0)
    {
        if (first[i] > second[i]) return 1;
        else if (first[i] < second[i]) return -1;
    }

    return 0;   
}