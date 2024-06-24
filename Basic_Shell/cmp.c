#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int cmp(const char *filename1, const char *filename2, int verbose, int ignore_case)
{
    FILE *file1 = fopen(filename1, "rb");
    FILE *file2 = fopen(filename2, "rb");

    if (file1 == NULL || file2 == NULL)
    {
        printf("One or both files could not be opened.\n");
        return 1;
    }

    int result = 0;
    int char1, char2;

    while (1)
    {
        char1 = fgetc(file1);
        char2 = fgetc(file2);

        if (ignore_case)
        {
            char1 = tolower(char1);
            char2 = tolower(char2);
        }

        if (char1 != char2)
        {
            result = 1;
            if (verbose)
            {
                printf("Files are distinct\n");
            }
            break;
        }

        if (char1 == EOF)
        {
            break;
        }
    }

    fclose(file1);
    fclose(file2);

    if (result == 0 && verbose)
    {
        printf("Files are equal\n");
    }

    return result;
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 5)
    {
        printf("Usage: ./cmp <file1> <file2> [-v] [-i]\n the [-v] and [-i] are optional\n");
        return 1;
    }

    int verbose = 0;
    int ignore_case = 0;
    int i;

    for (i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            ignore_case = 1;
        }
        else
        {
            printf("Invalid flag: %s\n", argv[i]);
            printf("Usage: ./cmp <file1> <file2> [-v] [-i]\n the [-v] and [-i] are optional\n");
            return 1;
        }
    }

    return cmp(argv[1], argv[2], verbose, ignore_case);
}