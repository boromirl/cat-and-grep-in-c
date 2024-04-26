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
    int display_non_printed;

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

        if ((strcmp(argv[i], "-E") == 0) || strcmp(argv[i], "-e") == 0)
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

        if ((strcmp(argv[i], "-T") == 0) || strcmp(argv[i], "-t") == 0)
        {
            display_tabs = 1;
            continue;
        }

        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "-e") == 0) || (strcmp(argv[i], "-t") == 0))
        {
            display_non_printed = 1;
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

        char prev = 0;
        int lineNumber = 1;
        int isPrevBlank = 0;
        // output file character by character
        while ((c = fgetc(file)) != EOF)
        {
            int isBlank = ((c == '\n' && prev == '\n') || (c == '\n' && prev == 0));
            
            if(squeeze && isBlank && isPrevBlank)
            {
                continue;
            }

            if(display_numbers_nonblank)
            {
                if((prev == '\n' || prev == 0) && !isBlank)
                {
                    printf("%d\t", lineNumber);
                    lineNumber++;
                }
            }
            else if(display_numbers)
            {
                if(prev == '\n' || prev == 0) 
                {
                    printf("%d\t", lineNumber);
                    lineNumber++;
                }
            }

            if(display_end_of_line && c == '\n') putchar('$');
            if(display_tabs && c == '\t')
            {
                putchar('^');
                putchar('I');
            }
            else if(display_non_printed && (c < 32 || c > 126) && c != 10 && c != 9){
                printf("^%c", c + 64);
            }
            else{
                putchar(c);
            }
            
            prev = c;
            isPrevBlank = isBlank;
        }

        printf("\n-b (%d)   -n (%d)   -E (%d)     -s (%d)     -T (%d)", display_numbers_nonblank, display_numbers, display_end_of_line, squeeze, display_tabs);

        prev = 0;
        // close file
        fclose(file);
    }

    
    
    return 0;
}
