#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char c;

    int display_numbers = 0;
    int display_numbers_nonblank = 0;
    int display_end_of_line = 0;
    int squeeze = 0;
    int display_tabs = 0;
    int display_non_printed = 0;

    static struct option long_options[] = {
        {"number", no_argument, 0, 'n'},
        {"number-nonblank", no_argument, 0, 'b'},
        {"squeeze-blank", no_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    // check for parameters
    if (argc < 2)
    {
        printf("Usage: %s [OPTION]... [FILE]...", argv[0]);
        return 1;
    }

    // find command line arguments
    int opt;
    while ((opt = getopt_long(argc, argv, "benstvET", long_options, NULL)) != -1)
    {
        switch(opt){
            case 'E':
                display_end_of_line = 1;
                break;
            case 'T':
                display_tabs = 1;
                break;
            case 'b':
                display_numbers_nonblank = 1;
                break;
            case 'e':
                display_end_of_line = 1;
                display_non_printed = 1;
                break;
            case 'n':
                display_numbers = 1;
                break;
            case 's':
                squeeze = 1;
                break;
            case 't':
                display_tabs = 1;
                display_non_printed = 1;
                break;
            case 'v':
                display_non_printed = 1;
                break;
            case '?':
                //printf("Invalid option: %c\n", optopt);
                break;
        }
    }

    for(int i = optind; i < argc; i++)
    {
        file = fopen(argv[i], "r");

        if(file == NULL)
        {
            printf("\ncat: %s: No such file or directory", argv[i]);
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
                    printf("     %d\t", lineNumber);
                    lineNumber++;
                }
            }
            else if(display_numbers)
            {
                if(prev == '\n' || prev == 0) 
                {
                    printf("     %d\t", lineNumber);
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

        fclose(file);
    }

        

    //printf("\n-b (%d)   -n (%d)   -E (%d)     -s (%d)     -T (%d)", display_numbers_nonblank, display_numbers, display_end_of_line, squeeze, display_tabs);
    
    return 0;
}
