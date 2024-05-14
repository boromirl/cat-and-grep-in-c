#ifdef __linux__
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int count_matches(char* filename, char** patterns, int numPatterns,
                  int ignore_case, int inverted);
void output_line(char* string, char* filename, int needFilename, int lineNum,
                 int needLineNum);
int is_line_match(char* line, char** patterns, int numPatterns,
                  int ignore_case);
void output_patterns_in_line(char* line, char** patterns, int numPatterns,
                             char* filename, int needFilename, int lineNum,
                             int needLineNum, int ignore_case);
int find_in_line(char* line, char* pattern, int ignore_case);

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
    fprintf(stderr, "Usage: %s -e pattern filename\n", argv[0]);
    return 1;
  }

  int opt;
  while ((opt = getopt(argc, argv, "e:ivclonhsf:")) != -1) {
    switch (opt) {
      case 'e':
        numPatterns++;
        char** newPtr = realloc(patterns, numPatterns * sizeof(char*));
        if (newPtr == NULL) {
          fprintf(stderr, "Error: Memory allocation failed for patterns\n");
          // Free the old patterns before exiting or handling the error
          for (int i = 0; i < numPatterns - 1; i++) {
            free(patterns[i]);
          }
          free(patterns);      // Free the whole array
          exit(EXIT_FAILURE);  // Or handle the error in an appropriate way
        } else {
          patterns = newPtr;
          patterns[numPatterns - 1] = strdup(optarg);
        }
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
      case 'f':;
        FILE* pattern_file;
        pattern_file = fopen(optarg, "r");
        if (pattern_file == NULL) {
          if (!s_flag)
            fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
          return 1;
        }
        char* pattern = NULL;
        size_t pattern_len = 0;
        ssize_t read;

        while ((read = getline(&pattern, &pattern_len, pattern_file)) != -1) {
          if (pattern[read - 1] == '\n' && pattern[0] != '\n') {
            pattern[read - 1] = '\0';
          }
          numPatterns++;
          patterns = realloc(patterns, numPatterns * sizeof(char*));
          patterns[numPatterns - 1] = strdup(pattern);
        }
        free(pattern);
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
    /*for(int i = 0; i < numPatterns; i++)
    {
      printf("%d: %s: %c", i, patterns[i], patterns[i][strlen(patterns[i]) -
    1]); if(patterns[i][strlen(patterns[i]) - 1] == '\n') printf("-- n --");
      if(patterns[i][strlen(patterns[i])] == '\0') printf("-- 0 --");
    }*/
    // printf("-- %s --", argv[i]);

    int matches_count =
        count_matches(argv[i], patterns, numPatterns, i_flag, v_flag);

    if (matches_count == -1) {
      if (!s_flag)
        fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
      continue;
    }

    char matches_count_str[10];
    sprintf(matches_count_str, "%d", matches_count);

    // printf("== %d == %d == ", matches_count, numPatterns);

    if (l_flag && matches_count > 0)
      printf("%s\n", argv[i]);
    else if (c_flag)
      output_line(matches_count_str, argv[i], show_filename, 0, 0);
    else {
      file = fopen(argv[i], "r");
      if (file == NULL) {
        if (!s_flag) printf("grep: %s: No such file or directory\n", argv[i]);
        continue;
      }

      char* line = NULL;
      size_t line_len = 0;
      ssize_t read;
      int line_num = 1;

      while ((read = getline(&line, &line_len, file)) != -1) {
        int is_match = is_line_match(line, patterns, numPatterns, i_flag);
        if (v_flag) {
          if (is_match)
            is_match = 0;
          else
            is_match = 1;
        }

        if (is_match) {
          if (o_flag && !v_flag)
            output_patterns_in_line(line, patterns, numPatterns, argv[i],
                                    show_filename, line_num, n_flag, i_flag);
          else {
            output_line(line, argv[i], show_filename, line_num, n_flag);
          }
        }

        line_num++;
      }

      free(line);

      fclose(file);
    }
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

  char* line = NULL;
  size_t len = 0;
  ssize_t read;

  int matches_count = 0;

  while ((read = getline(&line, &len, file)) != -1) {
    if (line[read - 1] == '\n') {
      line[read - 1] = '\0';
    }
    int isMatchFoundInLine =
        is_line_match(line, patterns, numPatterns, ignore_case);
    if (isMatchFoundInLine && !inverted) matches_count++;
    if (!isMatchFoundInLine && inverted) matches_count++;
  }

  free(line);
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
    if (find_in_line(line, patterns[i], ignore_case) != -1) {
      patternFound = 1;
      break;
    }
  }
  return patternFound;
}

void output_patterns_in_line(char* line, char** patterns, int numPatterns,
                             char* filename, int needFilename, int lineNum,
                             int needLineNum, int ignore_case) {
  int first_pattern_ind;
  int line_len = strlen(line);
  // char* lineCopy = strdup(line);
  do {
    // printf("--%s--", line);
    first_pattern_ind = -1;
    int first_pattern_len = 0;
    for (int i = 0; i < numPatterns; i++) {
      int pattern_ind = find_in_line(line, patterns[i], ignore_case);
      if (pattern_ind > -1 &&
          (pattern_ind < first_pattern_ind || first_pattern_ind == -1)) {
        first_pattern_ind = pattern_ind;
        first_pattern_len = strlen(patterns[i]);
      }
    }

    if (first_pattern_ind != -1) {
      // char* first_pattern = (char*)malloc(first_pattern_len + 1);
      char* first_pattern =
          strndup(line + first_pattern_ind, first_pattern_len);
      // first_pattern[first_pattern_len] = '\0';

      line_len = line_len - first_pattern_ind - first_pattern_len;

      output_line(first_pattern, filename, needFilename, lineNum, needLineNum);
      // line = strndup(line + first_pattern_len + first_pattern_ind, line_len);
      memmove(line, line + first_pattern_len + first_pattern_ind, line_len);
      line[line_len] = '\0';

      free(first_pattern);
    }
  } while (first_pattern_ind != -1);
  // free(line);
}

int find_in_line(char* line, char* pattern, int ignore_case) {
  int line_len = strlen(line);
  int pattern_len = strlen(pattern);

  int result = -1;

  for (int i = 0; i <= line_len - pattern_len; i++) {
    int match = 1;
    for (int j = 0; j < pattern_len; j++) {
      char line_char = ignore_case ? tolower(line[i + j]) : line[i + j];
      char pattern_char = ignore_case ? tolower(pattern[j]) : pattern[j];

      if (line_char != pattern_char) {
        match = 0;
        break;
      }
    }

    if (match) {
      result = i;
      break;
    }
  }

  return result;
}