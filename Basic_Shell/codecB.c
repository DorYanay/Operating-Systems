#include <stdio.h>
#include <stdlib.h>
#include "codech.h"

void encodeB(char *str)
{
    while (*str != '\0')
    {
        *str = *str + 3;
        str++;
    }
}
void decodeB(char *str)
{
    while (*str != '\0')
    {
        *str = *str - 3;
        str++;
    }
}