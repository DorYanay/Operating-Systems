#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "codech.h"

void encodeA(char *str)
{
    while (*str != '\0')
    {
        if (islower(*str))
        {
            *str = toupper(*str);
        }
        else if (isupper(*str))
        {
            *str = tolower(*str);
        }
        str++;
    }
}

void decodeA(char *str)
{
    encodeA(str);
}