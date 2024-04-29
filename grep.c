#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define MAX_PATTERNS 100
#define MAX_LINE_LEN 1000

int main(int argc, char* argv[])
{
    FILE *file;
    char* patterns[MAX_PATTERNS];
    
    int i_flag = 0;
    int v_flag = 0;
    int c_flag = 0;
    int l_flag = 0;
    int o_flag = 0;
    int n_flag = 0;
    int h_flag = 0;
    int s_flag = 0;


    if (argc < 3) {
        fprintf(stderr, "Usage: %s -e pattern filename\n", argv[0]);
        return 1;
    }

    int opt;
    int pattern_num = 0;
    while((opt = getopt(argc, argv, "e:ivclonhsf:")) != -1)
    {
        switch(opt){
            case 'e':
                patterns[pattern_num] = optarg;
                pattern_num++;
                break;
            case 'i':
                i_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'c':
                c_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 'o':
                o_flag = 1;
                break;
            case 'n':
                n_flag = 1;
                break;
            case 'h':
                h_flag = 1;
                break;
            case 's':
                s_flag = 1;
                break;
            case 'f':
                FILE *pattern_file = fopen(optarg, "r");
                char pattern[MAX_LINE_LEN];
                while(fgets(pattern, MAX_LINE_LEN, pattern_file) != NULL)
                {
                    size_t len = strlen(pattern);
                    if (pattern[len - 1] == '\n'){
                        pattern[len - 1] = '\0';
                    }
                    patterns[pattern_num] = strdup(pattern);
                    pattern_num++;
                }
                fclose(pattern_file);
                break;
            case '?':
                //printf("Invalid option: %c\n", optopt);
                break;
        }
    }

    if(pattern_num == 0)
    {
        patterns[pattern_num] = argv[optind];
        optind++;
        pattern_num++;
    }

    for(int i = optind; i < argc; i++)
    {
        file = fopen(argv[i], "r");
        
        if(file == NULL)
        {
            printf("\ngrep: %s: No such file or directory", argv[i]);
            return 1;
        }

        char* filename = strdup(argv[i]);
        if((strncmp(filename, "./", 2) == 0) || (strncmp(filename, ".\\", 2) == 0)){
            memmove(filename, filename + 2, strlen(filename) - 1);
        }

        char line[MAX_LINE_LEN];
        while(fgets(line, MAX_LINE_LEN, file) != NULL)
        {
            printf("%s: %s", filename, line);
        }

        fclose(file);
    }


    printf("Check for flags:\n -i(%d)   -v(%d)  -c(%d)  -l(%d)  -o(%d)  -n(%d)  -h(%d)  -s(%d)", i_flag, v_flag, c_flag, l_flag, o_flag, n_flag, h_flag, s_flag);
    printf("\nPatterns:");
    for (int i = 0; i < pattern_num; i++)
    {
        printf("\n[%d]: %s", i, patterns[i]);
    }
    return 0;
}