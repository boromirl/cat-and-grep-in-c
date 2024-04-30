#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define MAX_PATTERNS 100
#define MAX_LINE_LEN 1000

int count_matches(char* filename, char* patterns[MAX_PATTERNS], int pattern_count);
void output_line(char* string, char* filename, int needFilename, int lineNum, int needLineNum);
int is_line_match(char* line, char* patterns[MAX_PATTERNS], int pattern_count);
void output_patterns_in_line(char* line, char* patterns[MAX_PATTERNS], int pattern_count, char* filename, int needFilename, int lineNum, int needLineNum);

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

    int show_filename = 0;


    if (argc < 3) {
        fprintf(stderr, "Usage: %s -e pattern filename\n", argv[0]);
        return 1;
    }

    int opt;
    int pattern_count = 0;
    while((opt = getopt(argc, argv, "e:ivclonhsf:")) != -1)
    {
        switch(opt){
            case 'e':
                patterns[pattern_count] = optarg;
                pattern_count++;
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
                    patterns[pattern_count] = strdup(pattern);
                    pattern_count++;
                }
                fclose(pattern_file);
                break;
            case '?':
                //printf("Invalid option: %c\n", optopt);
                break;
        }
    }

    if(pattern_count == 0)
    {
        patterns[pattern_count] = argv[optind];
        optind++;
        pattern_count++;
    }

    if(!h_flag && (argc - optind > 1)) show_filename = 1;

    for(int i = optind; i < argc; i++)
    {
        char* filename = strdup(argv[i]);
        //printf("-- %s --", filename);

        int matches_count = count_matches(filename, patterns, pattern_count);
        char matches_count_str[10];
        sprintf(matches_count_str, "%d", matches_count);

        //printf("== %d == %d == ", matches_count, pattern_count);

        if(l_flag && matches_count > 0) printf("%s\n", filename);
        else if(c_flag) output_line(matches_count_str, filename, show_filename, 0, 0);
        else {
            file = fopen(filename, "r");
            if(file == NULL)
            {
                printf("\ngrep: %s: No such file or directory", filename);
                return 1;
            }

            char line[MAX_LINE_LEN];
            int line_num = 1;

            while(fgets(line, MAX_LINE_LEN, file) != NULL)
            {
                int is_match = is_line_match(line, patterns, pattern_count);

                if(is_match)
                {
                    if(o_flag) output_patterns_in_line(line, patterns, pattern_count, filename, show_filename, line_num, n_flag);
                    else{
                        output_line(line, filename, show_filename, line_num, n_flag);
                    }
                }

                line_num++;
            }

            fclose(file);

        }
        
    }


    /*printf("\nCheck for flags:\n -i(%d)   -v(%d)  -c(%d)  -l(%d)  -o(%d)  -n(%d)  -h(%d)  -s(%d)", i_flag, v_flag, c_flag, l_flag, o_flag, n_flag, h_flag, s_flag);
    printf("\nPatterns:");
    for (int i = 0; i < pattern_count; i++)
    {
        printf("\n[%d]: %s", i, patterns[i]);
    }*/
    return 0;
}


int count_matches(char* filename, char* patterns[MAX_PATTERNS], int pattern_count)
{
    FILE *file = fopen(filename, "r");

    char line[MAX_LINE_LEN];
    int matches_count = 0;

    while(fgets(line, MAX_LINE_LEN, file) != NULL)
    {
        int isMatchFoundInLine = 0;

        for(int i = 0; i < pattern_count; i++)
        {
            if(strstr(line, patterns[i]) != NULL) isMatchFoundInLine = 1;
        }
        if(isMatchFoundInLine) matches_count++;
    }

    fclose(file);
    return matches_count;
}

void output_line(char* string, char* filename, int needFilename, int lineNum, int needLineNum)
{
    if(needFilename) printf("%s:", filename);
    if(needLineNum) printf("%d:", lineNum);
    printf("%s", string);
    if(string[strlen(string) - 1] != '\n') printf("\n");
}

int is_line_match(char* line, char* patterns[MAX_PATTERNS], int pattern_count)
{
    int patternFound = 0;
    for (int i = 0; i < pattern_count; i++)
    {
        if(strstr(line, patterns[i]) != NULL)
        {
            patternFound = 1;
            break;
        }
    }
    return patternFound;
}

void output_patterns_in_line(char* line, char* patterns[MAX_PATTERNS], int pattern_count, char* filename, int needFilename, int lineNum, int needLineNum)
{
    int min_str_ind = -1;
    int pattern_ind = -1;
    int string_start = 0;
    char cut_line[MAX_LINE_LEN];
    do
    {
        min_str_ind = -1;
        //printf(" -1-%d-- ", min_str_ind);
        pattern_ind = -1;
        
        strcpy(cut_line, line + string_start);

        for(int i = 0; i < pattern_count; i++)
        {
            char* result = strstr(cut_line, patterns[i]);

            if(result != NULL)
            {
                int str_ind = result - line;
                if(str_ind < min_str_ind || str_ind == -1)
                {
                    min_str_ind = str_ind;
                    pattern_ind = i;
                }
            }


        }

        if(min_str_ind != -1)
        {
            output_line(patterns[pattern_ind], filename, needFilename, lineNum, needLineNum);
            string_start = min_str_ind + strlen(patterns[pattern_ind]);
        }

        //printf(" -2-%d-- ", min_str_ind);
    } while(min_str_ind != -1); 
}

