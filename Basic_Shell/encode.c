#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>
#include "codech.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./encode <codec> <message>\n");
        return 1;
    }
    char *method = argv[1];
    char *message = argv[2];
    if (!strcmp(method, "codecA"))
    {
        encodeA(message);
        printf("%s\n", message);
    }
    else if (!strcmp(method, "codecB"))
    {
        encodeB(message);
        printf("%s\n", message);
    }
    else
    {
        fprintf(stderr, "Usage: ./decode <codec> <message>\n");
        return 1;
    }
    return 0;
}