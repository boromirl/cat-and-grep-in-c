#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char c;

    int display_numbers = 0;
    int display_numbers_nonblank = 0;
    int display_end_of_line = 0;
    int squeeze = 0;
    int display_tabs = 0;

    // check for parameters
    if (argc < 2)
    {
        printf("Usage: %s [OPTION]... [FILE]...", argv[0]);
        return 1;
    }

    // check for command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--number-nonblank") == 0)
        {
            display_numbers_nonblank = 1;
            continue;
        }

        if (strcmp(argv[i], "-E") == 0)
        {
            display_end_of_line = 1;
            continue;
        }

        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--number") == 0)
        {
            display_numbers = 1;
            continue;
        }

        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--squeeze-blank") == 0)
        {
            squeeze = 1;
            continue;
        }

        if (strcmp(argv[i], "-T") == 0)
        {
            display_tabs = 1;
            continue;
        }

        // open file
        file = fopen(argv[i], "r");

        // check if file was succesefully opened
        if (file == NULL)
        {
            printf("Error opening file\n");
            return 1;
        }

        // output file character by character
        while ((c = fgetc(file)) != EOF)
        {
            if(display_end_of_line && c == '\n') putchar('$');
            if(display_tabs && c == '\t')
            {
                putchar('^');
                putchar('I');
                continue;
            }
            putchar(c);
        }

        printf("\n\n-b (%d)   -n (%d)   -E (%d)     -s (%d)     -T (%d)\n\n", display_numbers_nonblank, display_numbers, display_end_of_line, squeeze, display_tabs);


        // close file
        fclose(file);
    }

    
    
    return 0;
}
