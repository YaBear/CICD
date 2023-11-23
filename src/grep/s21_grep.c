#include "s21_grep.h"
regex_t regeex;
regmatch_t match;

int main(int argc, char *argv[]) {
  char e_pattern_template[128][128] = {0};
  int e_pattern_count = 0;
  char f_pattern_file[128] = {0};
  const char *short_options = "e:ivclnhsf:o";
  const struct option long_options[] = {
      {"pattern", required_argument, NULL, 'e'},
      {"icase", no_argument, NULL, 'i'},
      {"invert", no_argument, NULL, 'v'},
      {"count-exec", no_argument, NULL, 'c'},
      {"counter-files", no_argument, NULL, 'l'},
      {"line-counter", no_argument, NULL, 'n'},
      {"not-print-file", no_argument, NULL, 'h'},
      {"not-print-error", no_argument, NULL, 's'},
      {"file-pattern", required_argument, NULL, 'f'},
      {"single-word", no_argument, NULL, 'o'},
      {NULL, 0, NULL, 0}};
  int rez = 0, option_index = -1;
  struct flag_list flags = {0};
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'e':
        flags.flag_e = 1;
        strcpy(e_pattern_template[e_pattern_count], optarg);
        e_pattern_count++;
        break;
      case 'i':
        flags.flag_i = 1;
        break;
      case 'v':
        flags.flag_v = 1;
        break;
      case 'c':
        flags.flag_c = 1;
        break;
      case 'l':
        flags.flag_l = 1;
        break;
      case 'n':
        flags.flag_n = 1;
        break;
      case 'h':
        flags.flag_h = 1;
        break;
      case 's':
        flags.flag_s = 1;
        break;
      case 'f':
        flags.flag_f = 1;
        strcpy(f_pattern_file, optarg);
        break;
      case 'o':
        flags.flag_o = 1;
        break;
    }
    option_index = -1;
  }
  fileCounter(argv, flags, argc, e_pattern_template, e_pattern_count,
              f_pattern_file);
  return 0;
}

int fileCounter(char *argv[], struct flag_list flags, int argc,
                char e_pattern_template[128][128], int e_pattern_count,
                char f_pattern_file[128]) {
  char file_name[128] = {0};
  char file_arg[128][128] = {0};
  char pattern_file_buffer[128] = {0};
  int current_line = 0;
  int f_error = 0;
  if (flags.flag_f == 1) {
    FILE *pattern_file = NULL;
    pattern_file = fopen(f_pattern_file, "r");
    if (pattern_file != NULL) {
      while (fgets(pattern_file_buffer, sizeof(pattern_file_buffer),
                   pattern_file)) {
        for (int k = 0; k < 128; k++) {
          if (pattern_file_buffer[k] == '\n') break;
          file_arg[current_line][k] = pattern_file_buffer[k];
        }
        current_line++;
      }
      fclose(pattern_file);
    } else {
      f_error = 1;
      if (flags.flag_s == 0) fprintf(stderr, "No such file or directory.\n");
    }
  }
  if (f_error == 0) {
    if (flags.flag_e == 0 && flags.flag_f == 0) {
      for (int i = optind + 1; i < argc; i++) {
        strcpy(file_name, argv[i]);
        printLines(argv, flags, file_name, argc, current_line, file_arg,
                   e_pattern_count, e_pattern_template);
      }
    } else {
      for (int i = optind; i < argc; i++) {
        strcpy(file_name, argv[i]);
        printLines(argv, flags, file_name, argc, current_line, file_arg,
                   e_pattern_count, e_pattern_template);
      }
    }
  }
  return 0;
}

int printLines(char *argv[], struct flag_list flags, char *file_name, int argc,
               int current_line, char file_arg[128][128], int e_pattern_count,
               char e_pattern_template[128][128]) {
  int f_sum_check = 0;
  if (flags.flag_e == 0)
    f_sum_check = argc - optind;
  else
    f_sum_check = argc - optind + 1;
  int print_line_number;
  print_line_number = 1;
  int line_success = 0;
  int line_success_invert = 0;
  FILE *file = NULL;
  file = fopen(file_name, "r");
  if (file != NULL) {
    char text_buffer[990];
    if (flags.flag_h == 1) f_sum_check = 2;
    while (fgets(text_buffer, sizeof(text_buffer), file)) {
      flagWork(argv, f_sum_check, flags, print_line_number, file_name,
               text_buffer, &line_success, &line_success_invert,
               e_pattern_count, e_pattern_template, current_line, file_arg);
      print_line_number++;
    }
    if (flags.flag_l == 1 && flags.flag_c == 1) {
      if (f_sum_check > 2) {
        if (flags.flag_v == 0) {
          if (line_success == 0) {
            if (flags.flag_h == 0)
              printf("%s:0\n", file_name);
            else
              printf("0\n");
          } else {
            printf("%s:1\n", file_name);
            printf("%s\n", file_name);
          }
        } else {
          if (line_success_invert == 0) {
            if (flags.flag_h == 0)
              printf("%s:0\n", file_name);
            else
              printf("0\n");
          } else {
            printf("%s:1\n", file_name);
            printf("%s\n", file_name);
          }
        }
      } else {
        if (flags.flag_v == 0) {
          if (line_success == 0) {
            if (flags.flag_h == 0)
              printf("%s:0\n", file_name);
            else
              printf("0\n");
          }
          if (line_success >= 1) {
            printf("1\n%s\n", file_name);
          }
        } else {
          if (line_success_invert == 0) {
            if (flags.flag_h == 0)
              printf("%s:0\n", file_name);
            else
              printf("0\n");
          }
          if (line_success_invert >= 1) {
            printf("1\n%s\n", file_name);
          }
        }
      }
    }
    if (flags.flag_l == 1 && flags.flag_c == 0 && line_success > 0)
      printf("%s\n", file_name);
    if (flags.flag_l == 1 && flags.flag_c == 0 && flags.flag_v == 1 &&
        line_success == 0)
      printf("%s\n", file_name);
    if (flags.flag_c == 1 && flags.flag_l == 0 && f_sum_check == 2) {
      if (flags.flag_v == 0)
        printf("%d\n", line_success);
      else
        printf("%d\n", line_success_invert);
    }
    if (flags.flag_c == 1 && flags.flag_l == 0 && f_sum_check > 2) {
      if (flags.flag_v == 0)
        printf("%s:%d\n", file_name, line_success);
      else
        printf("%s:%d\n", file_name, line_success_invert);
    }
    fclose(file);
  } else {
    if (flags.flag_s == 0) fprintf(stderr, "No such file or directory.\n");
  }
  return 0;
}

int checkPatterns(int *check_error, struct flag_list flags, int e_pattern_count,
                  char e_pattern_template[128][128], char *text_buffer) {
  int need_print = 0;
  if (flags.flag_e == 1 || flags.flag_f == 1) {
    for (int i = 0; i < e_pattern_count; i++) {
      if (flags.flag_i == 0)
        *check_error = regcomp(&regeex, e_pattern_template[i], REG_EXTENDED);
      else
        *check_error = regcomp(&regeex, e_pattern_template[i], REG_ICASE);
      if (*check_error == 0) {
        if (regexec(&regeex, text_buffer, 0, NULL, 0) == 0) need_print = 1;
      }
      regfree(&regeex);
    }
  }
  return need_print;
}

int flagWork(char *argv[], int f_sum_check, struct flag_list flags,
             int print_line_number, char *file_name, char *text_buffer,
             int *line_success, int *line_success_invert, int e_pattern_count,
             char e_pattern_template[128][128], int current_line,
             char file_arg[128][128]) {
  char buffer[990] = {0};
  int line = 0;
  int need_print = 0;
  int printed = 0;
  int check_error = 0;
  for (int i = 0; i < current_line; i++) {
    strcpy(e_pattern_template[e_pattern_count], file_arg[i]);
    e_pattern_count++;
  }
  need_print = checkPatterns(&check_error, flags, e_pattern_count,
                             e_pattern_template, text_buffer);
  if (check_error == 0) {
    if (flags.flag_o == 1) {
      if (flags.flag_f == 0 && flags.flag_e == 0) {
        if (flags.flag_i == 0)
          regcomp(&regeex, argv[optind], REG_EXTENDED);
        else
          regcomp(&regeex, argv[optind], REG_ICASE);
        if (flags.flag_v == 0)
          doFlagO(flags, f_sum_check, print_line_number, line_success,
                  text_buffer, file_name, &printed);
        else
          doFlagNotO(flags, text_buffer, line_success, line_success_invert,
                     f_sum_check, print_line_number, file_name, &printed);
        regfree(&regeex);
      }
      if (flags.flag_e == 1 || flags.flag_f == 1) {
        for (int i = 0; i < e_pattern_count; i++) {
          if (flags.flag_i == 0)
            regcomp(&regeex, e_pattern_template[i], REG_EXTENDED);
          else
            regcomp(&regeex, e_pattern_template[i], REG_ICASE);
          doPrintFandE(flags, need_print, f_sum_check, line_success,
                       line_success_invert, print_line_number, text_buffer,
                       file_name, buffer, &line, &printed);
          regfree(&regeex);
          printed = 0;
        }
      }
    } else {
      if (flags.flag_e == 1 || flags.flag_f == 1) {
        for (int i = 0; i < e_pattern_count; i++) {
          if (flags.flag_i == 0)
            regcomp(&regeex, e_pattern_template[i], REG_EXTENDED);
          else
            regcomp(&regeex, e_pattern_template[i], REG_ICASE);
          doPrintFandE(flags, need_print, f_sum_check, line_success,
                       line_success_invert, print_line_number, text_buffer,
                       file_name, buffer, &line, &printed);
          regfree(&regeex);
        }
      }
      if (flags.flag_f == 0 && flags.flag_e == 0) {
        if (flags.flag_i == 0)
          regcomp(&regeex, argv[optind], REG_EXTENDED);
        else
          regcomp(&regeex, argv[optind], REG_ICASE);
        doFlagNotO(flags, text_buffer, line_success, line_success_invert,
                   f_sum_check, print_line_number, file_name, &printed);
        regfree(&regeex);
      }
    }
  }
  return 0;
}

int doPrintFandE(struct flag_list flags, int need_print, int f_sum_check,
                 int *line_success, int *line_success_invert,
                 int print_line_number, char *text_buffer, char *file_name,
                 char *buffer, int *line, int *printed) {
  if (flags.flag_e == 1 || flags.flag_f == 1) {
    if (flags.flag_v == 0 && need_print == 1) {
      if (flags.flag_o == 1) {
        if (*printed == 0)
          doFlagO(flags, f_sum_check, print_line_number, line_success,
                  text_buffer, file_name, printed);
      } else {
        if (*printed == 0)
          doFlagNotO(flags, text_buffer, line_success, line_success_invert,
                     f_sum_check, print_line_number, file_name, printed);
      }
    }
    if (flags.flag_v == 1 && need_print == 0) {
      if (strcmp(buffer, text_buffer) != 0 || *line != print_line_number) {
        doFlagNotO(flags, text_buffer, line_success, line_success_invert,
                   f_sum_check, print_line_number, file_name, printed);
        strcpy(buffer, text_buffer);
        *line = print_line_number;
      }
    }
  }
  return 0;
}

int doFlagO(struct flag_list flags, int f_sum_check, int print_line_number,
            int *line_success, char *text_buffer, char *file_name,
            int *printed) {
  int offset = 0;
  int flag_duplicate = 0;
  while (!regexec(&regeex, text_buffer + offset, 1, &match, 0)) {
    if (flags.flag_c == 0 && flags.flag_l == 0 && flag_duplicate == 0) {
      if (flags.flag_h == 1 || f_sum_check == 2) {
        if (flags.flag_n == 1) printf("%d:", print_line_number);
      } else {
        if (flags.flag_n == 1)
          printf("%s:%d:", file_name, print_line_number);
        else
          printf("%s:", file_name);
      }
    }
    for (int i = offset + match.rm_so; i < offset + match.rm_eo; i++) {
      if (flags.flag_c == 0 && flags.flag_l == 0) printf("%c", text_buffer[i]);
    }
    if (flags.flag_c == 0 && flags.flag_l == 0) printf("\n");
    if ((flags.flag_c == 1 || flags.flag_l == 1) && flag_duplicate == 0)
      (*line_success)++;
    offset += match.rm_eo;
    flag_duplicate = 1;
    *printed = 1;
  }
  return 0;
}

int doFlagNotO(struct flag_list flags, char *text_buffer, int *line_success,
               int *line_success_invert, int f_sum_check, int print_line_number,
               char *file_name, int *printed) {
  int pattern_comp = 0;
  int length = strlen(text_buffer);
  pattern_comp = regexec(&regeex, text_buffer, 0, NULL, 0);
  if (pattern_comp == 0) {
    if (flags.flag_c == 1 || flags.flag_l == 1) (*line_success)++;
    if (flags.flag_n == 1 && flags.flag_v == 0 && flags.flag_c == 0 &&
        f_sum_check == 2 && flags.flag_l == 0)
      printf("%d:", print_line_number);
    if (flags.flag_l == 0 && flags.flag_c == 0 && flags.flag_v == 0) {
      if (f_sum_check == 2) {
        if (text_buffer[length - 1] == '\n' && text_buffer[length] != '\n')
          printf("%s", text_buffer);
        else
          printf("%s\n", text_buffer);
      }
      if (f_sum_check > 2) {
        if (flags.flag_n == 1 && flags.flag_v == 0) {
          if (text_buffer[length - 1] == '\n' && text_buffer[length] != '\n')
            printf("%s:%d:%s", file_name, print_line_number, text_buffer);
          else
            printf("%s:%d:%s\n", file_name, print_line_number, text_buffer);
        } else {
          if (text_buffer[length - 1] == '\n' && text_buffer[length] != '\n')
            printf("%s:%s", file_name, text_buffer);
          else
            printf("%s:%s\n", file_name, text_buffer);
        }
      }
    }
    *printed = 1;
  } else {
    (*line_success_invert)++;
    if (flags.flag_v == 1 && flags.flag_l == 0 && flags.flag_c == 0) {
      if (f_sum_check == 2) {
        if (text_buffer[length - 1] == '\n' && text_buffer[length] != '\n') {
          if (flags.flag_n == 1) printf("%d:", print_line_number);
          printf("%s", text_buffer);
        } else {
          if (flags.flag_n == 1) printf("%d:", print_line_number);
          printf("%s\n", text_buffer);
        }
      }
      if (f_sum_check > 2) {
        if (text_buffer[length - 1] == '\n' && text_buffer[length] != '\n') {
          if (flags.flag_n == 1)
            printf("%s:%d:%s", file_name, print_line_number, text_buffer);
          else
            printf("%s:%s", file_name, text_buffer);
        } else {
          if (flags.flag_n == 1)
            printf("%s:%d:%s\n", file_name, print_line_number, text_buffer);
          else
            printf("%s:%s\n", file_name, text_buffer);
        }
      }
    }
  }
  return 0;
}