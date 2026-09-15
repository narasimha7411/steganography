#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

#define RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"


typedef enum{
    e_success,
    e_failure
}status;

typedef enum{
    e_encode,
    e_decode,
    e_invalid
}operations;

#endif