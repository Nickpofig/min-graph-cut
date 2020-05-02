// begins header guard
#ifndef __include_log
#define __include_log
//

#include <stdio.h>

#ifdef LOGS_ON
#define __log(__message, ...) printf((__message), ##__VA_ARGS__)
#else 
#define __log(__message, ...) 
#endif

#endif // ends header guard