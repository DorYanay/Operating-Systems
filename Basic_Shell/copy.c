#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int copy(const char *source_filename, const char *target_filename, int verbose, int force)
{
    FILE *source_file = fopen(source_filename, "rb");
    if (source_file == NULL)
    {
        printf("Error: could not open source file.\n");
        return 1;
    }

    if (access(target_filename, F_OK) != -1 && !force)
    {
        printf("Error: target file already exists. Use -f flag to overwrite.\n");
        return 1;
    }

    FILE *target_file = fopen(target_filename, "wb");
    if (target_file == NULL)
    {
        printf("Error: could not create target file.\n");
        return 1;
    }

    int char_read;
    while ((char_read = fgetc(source_file)) != EOF)
    {
        fputc(char_read, target_file);
    }

    fclose(source_file);
    fclose(target_file);

    if (verbose)
    {
        printf("Success\n");
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 5)
    {
        printf("Usage: ./copy <file1> <file2> [-v] [-f]\n the [-v] and [-f] are optional\n");
        return 1;
    }

    int verbose = 0;
    int force = 0;

    int i;
    for (i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            force = 1;
        }
        else
        {
            printf("Error: invalid flag.\n");
            printf("Usage: ./copy <file1> <file2> [-v] [-f]\n the [-v] and [-f] are optional\n");
            return 1;
        }
    }

    return copy(argv[1], argv[2], verbose, force);
}
