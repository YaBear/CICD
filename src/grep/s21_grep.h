#ifndef s21_grep
#define s21_grep

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

struct flag_list {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
};

int fileCounter(char *argv[], struct flag_list flags, int argc,
                char e_pattern_template[128][128], int e_pattern_count,
                char f_pattern_file[128]);
int printLines(char *argv[], struct flag_list flags, char *file_name, int argc,
               int current_line, char file_arg[128][128], int e_pattern_count,
               char e_pattern_template[128][128]);
int flagWork(char *argv[], int f_sum_check, struct flag_list flags,
             int print_line_number, char *file_name, char *text_buffer,
             int *line_success, int *line_success_invert, int e_pattern_count,
             char e_pattern_template[128][128], int current_line,
             char file_arg[128][128]);
int doFlagO(struct flag_list flags, int f_sum_check, int print_line_number,
            int *line_success, char *text_buffer, char *file_name,
            int *printed);
int doFlagNotO(struct flag_list flags, char *text_buffer, int *line_success,
               int *line_success_invert, int f_sum_check, int print_line_number,
               char *file_name, int *printed);
int doPrintFandE(struct flag_list flags, int need_print, int f_sum_check,
                 int *line_success, int *line_success_invert,
                 int print_line_number, char *text_buffer, char *file_name,
                 char *buffer, int *line, int *printed);
int checkPatterns(int *check_error, struct flag_list flags, int e_pattern_count,
                  char e_pattern_template[128][128], char *text_buffer);

#endif