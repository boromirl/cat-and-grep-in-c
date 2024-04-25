#include <stdio.h>

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
        printf("Usage: %s [PARAMETER]... [FILE]...", argv[0]);
        return 1;
    }

    // open file
    file = fopen(argv[1], "r");

    // check if file was succesefully opened
    if (file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    // output file character by character
    while ((c = fgetc(file)) != EOF)
    {
        putchar(c);
    }


    // close file
    fclose(file);
    
    return 0;
}
