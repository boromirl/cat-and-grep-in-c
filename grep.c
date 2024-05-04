#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1000

int count_matches(char* filename, char** patterns, int numPatterns,
                  int ignore_case, int inverted);
void output_line(char* string, char* filename, int needFilename, int lineNum,
                 int needLineNum);
int is_line_match(char* line, char** patterns, int numPatterns,
                  int ignore_case);
void output_patterns_in_line(char* line, char** patterns, int numPatterns,
                             char* filename, int needFilename, int lineNum,
                             int needLineNum, int ignore_case);
char* find_in_line(char* line, char* pattern, int ignore_case);
void str_toupper(char* str);

int main(int argc, char* argv[]) {
  FILE* file;
  char** patterns = NULL;
  int numPatterns = 0;

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
    if (!s_flag) fprintf(stderr, "Usage: %s -e pattern filename\n", argv[0]);
    return 1;
  }

  int opt;
  while ((opt = getopt(argc, argv, "e:ivclonhsf:")) != -1) {
    switch (opt) {
      case 'e':
        numPatterns++;
        patterns = realloc(patterns, numPatterns * sizeof(char*));
        patterns[numPatterns - 1] = strdup(optarg);
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
        FILE* pattern_file = fopen(optarg, "r");
        if (pattern_file == NULL) {
          if (!s_flag)
            fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
          return 1;
        }
        char pattern[MAX_LINE_LEN];
        while (fgets(pattern, MAX_LINE_LEN, pattern_file) != NULL) {
          size_t len = strlen(pattern);
          if (pattern[len - 1] == '\n') {
            pattern[len - 1] = '\0';
          }
          numPatterns++;
          patterns = realloc(patterns, numPatterns * sizeof(char*));
          patterns[numPatterns - 1] = strdup(pattern);
        }
        fclose(pattern_file);
        break;
      case '?':
        // printf("Invalid option: %c\n", optopt);
        break;
    }
  }

  if (numPatterns == 0) {
    patterns = realloc(patterns, sizeof(char*));
    patterns[0] = strdup(argv[optind]);
    optind++;
    numPatterns++;
  }

  if (!h_flag && (argc - optind > 1)) show_filename = 1;

  for (int i = optind; i < argc; i++) {
    char* filename = strdup(argv[i]);
    // printf("-- %s --", filename);

    int matches_count =
        count_matches(filename, patterns, numPatterns, i_flag, v_flag);

    if (matches_count == -1) {
      if (!s_flag)
        fprintf(stderr, "grep: %s: No such file or directory\n", filename);
      return 1;
    }

    char matches_count_str[10];
    sprintf(matches_count_str, "%d", matches_count);

    // printf("== %d == %d == ", matches_count, numPatterns);

    if (l_flag && matches_count > 0)
      printf("%s\n", filename);
    else if (c_flag)
      output_line(matches_count_str, filename, show_filename, 0, 0);
    else {
      file = fopen(filename, "r");
      if (file == NULL) {
        if (!s_flag) printf("grep: %s: No such file or directory\n", filename);
        return 1;
      }

      char line[MAX_LINE_LEN];
      int line_num = 1;

      while (fgets(line, MAX_LINE_LEN, file) != NULL) {
        int is_match = is_line_match(line, patterns, numPatterns, i_flag);
        if (v_flag) {
          if (is_match)
            is_match = 0;
          else
            is_match = 1;
        }

        if (is_match) {
          if (o_flag)
            output_patterns_in_line(line, patterns, numPatterns, filename,
                                    show_filename, line_num, n_flag, i_flag);
          else {
            output_line(line, filename, show_filename, line_num, n_flag);
          }
        }

        line_num++;
      }

      fclose(file);
    }

    free(filename);
  }

  /*printf("\nCheck for flags:\n -i(%d)   -v(%d)  -c(%d)  -l(%d)  -o(%d)  -n(%d)
  -h(%d)  -s(%d)", i_flag, v_flag, c_flag, l_flag, o_flag, n_flag, h_flag,
  s_flag); printf("\nPatterns:"); for (int i = 0; i < numPatterns; i++)
  {
      printf("\n[%d]: %s", i, patterns[i]);
  }*/

  for (int i = 0; i < numPatterns; i++) {
    free(patterns[i]);
  }
  free(patterns);
  return 0;
}

int count_matches(char* filename, char** patterns, int numPatterns,
                  int ignore_case, int inverted) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) return -1;

  char line[MAX_LINE_LEN];
  int matches_count = 0;

  while (fgets(line, MAX_LINE_LEN, file) != NULL) {
    int isMatchFoundInLine =
        is_line_match(line, patterns, numPatterns, ignore_case);

    if (isMatchFoundInLine && !inverted) matches_count++;
    if (!isMatchFoundInLine && inverted) matches_count++;
  }

  fclose(file);
  return matches_count;
}

void output_line(char* string, char* filename, int needFilename, int lineNum,
                 int needLineNum) {
  if (needFilename) printf("%s:", filename);
  if (needLineNum) printf("%d:", lineNum);
  printf("%s", string);
  if (string[strlen(string) - 1] != '\n') printf("\n");
}

int is_line_match(char* line, char** patterns, int numPatterns,
                  int ignore_case) {
  int patternFound = 0;
  for (int i = 0; i < numPatterns; i++) {
    if (find_in_line(line, patterns[i], ignore_case) != NULL) {
      patternFound = 1;
      break;
    }
  }
  return patternFound;
}

void output_patterns_in_line(char* line, char** patterns, int numPatterns,
                             char* filename, int needFilename, int lineNum,
                             int needLineNum, int ignore_case) {
  int min_str_ind = -1;
  int pattern_ind = -1;
  int string_start = 0;
  char cut_line[MAX_LINE_LEN];
  do {
    min_str_ind = -1;
    // printf(" -1-%d-- ", min_str_ind);
    pattern_ind = -1;

    strcpy(cut_line, line + string_start);

    for (int i = 0; i < numPatterns; i++) {
      char* result = find_in_line(cut_line, patterns[i], ignore_case);

      if (result != NULL) {
        int str_ind = result - line;
        if (str_ind < min_str_ind || str_ind == -1) {
          min_str_ind = str_ind;
          pattern_ind = i;
        }
      }
    }

    if (min_str_ind != -1) {
      output_line(patterns[pattern_ind], filename, needFilename, lineNum,
                  needLineNum);
      string_start = min_str_ind + strlen(patterns[pattern_ind]);
    }

    // printf(" -2-%d-- ", min_str_ind);
  } while (min_str_ind != -1);
}

char* find_in_line(char* line, char* pattern, int ignore_case) {
  char* result;
  if (ignore_case) {
    char* lineCopy = strdup(line);
    char* patternCopy = strdup(pattern);

    str_toupper(lineCopy);
    str_toupper(patternCopy);

    char* ptr = strstr(lineCopy, patternCopy);
    if (ptr == NULL)
      result = NULL;
    else {
      result = line + (ptr - lineCopy);
    }

    free(lineCopy);
    free(patternCopy);
  } else {
    result = strstr(line, pattern);
  }

  return result;
}

void str_toupper(char* str) {
  char* ptr = str;

  while (*ptr) {
    *ptr = toupper(*ptr);
    ptr++;
  }
}

