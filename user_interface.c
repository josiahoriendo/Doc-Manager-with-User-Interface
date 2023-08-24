#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sysexits.h>
#include "document.h"

#define MAX_LINE_SIZE 1024
#define MAX_COMMANDS

int check_digit(char param[]);

int edit_asterisk(char *param, int length, char const ln[]);

void two_qoute(char *first_param, char *sec_param, int length,
               char const ln[]);

void one_qoute(char *first_param, int length, char const ln[]);

int quote_check(int length, char const ln[]);

int main(int argc, char *argv[]) {
   Document main_document;
   FILE *input;
   char line[MAX_LINE_SIZE + 1] = "\0";
   char buff[MAX_LINE_SIZE + 1] = "\0";
   char command[MAX_LINE_SIZE + 1] = "\0";
   char first_spec[MAX_LINE_SIZE + 1] = "\0";
   char sec_spec[MAX_LINE_SIZE + 1] = "\0";
   char entry[MAX_LINE_SIZE + 1] = "\0";
   int i, k, run;
   int empty = 1;
   int asterisk = 0;

   if (argc > 2) {
      fprintf(stderr,
              "Usage: user_interface\nUsage: user_interface <filename>\n");
      return EX_USAGE;
   } else if (argc == 1) {
      input = stdin;
   } else if (argc == 2) {
      input = fopen(argv[1], "r");
      if (input == NULL) {
         fprintf(stderr, "%s cannot be opened.\n", argv[1]);
         return EX_OSERR;
      }
   }
   init_document(&main_document, "main_document");

   while (fgets(line, MAX_LINE_SIZE + 1, input) != NULL) {
      int max = strlen(line);
      int buff_max = 0;
      if (argc == 1) {
         printf("> ");
      }
      for (i = 0; i < max; i++) {
         empty = isspace(line[i]);
         if (empty == 0) {
            break;
         }
      }
      if (empty) {
         continue;
      }

      for (k = 0; k < max; k++) {
         if (line[k] == '\n') {
            buff[k] = '\0';
            break;
         }
         buff[k] = line[k];
      }

      buff_max = strlen(buff);
      sscanf(buff, " %s %s %s %s", command, first_spec, sec_spec, entry);
      if (command[0] == '#') {
         entry[0] = '\0';
         command[0] = '\0';
         first_spec[0] = '\0';
         sec_spec[0] = '\0';
         continue;
      }

      if (!strcmp(command, "add_paragraph_after")) {
         if (first_spec[0] == '\0' || sec_spec[0] != '\0' ||
             entry[0] != '\0' || !check_digit(first_spec) ||
             atoi(first_spec) < 0) {
            printf("Invalid Command\n");
         } else {
            run = add_paragraph_after(&main_document, atoi(first_spec));
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (!strcmp(command, "add_line_after")) {
         asterisk = edit_asterisk(entry, buff_max, buff);
         if (first_spec[0] == '\0' || sec_spec[0] == '\0' ||
             entry[0] == '\0' || !check_digit(first_spec) ||
             atoi(first_spec) <= 0 || !check_digit(sec_spec) ||
             atoi(sec_spec) < 0 || !asterisk) {
            printf("Invalid Command\n");
         } else {
            run = add_line_after(&main_document, atoi(first_spec),
                                 atoi(sec_spec), entry);
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (!strcmp(command, "print_document")) {
         if (first_spec[0] != '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            print_document(&main_document);
         }
      } else if (!strcmp(command, "quit")) {
         if (first_spec[0] != '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            return 0;
         }
      } else if (!strcmp(command, "exit")) {
         if (first_spec[0] != '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            return 0;
         }
      } else if (!strcmp(command, "append_line")) {
         asterisk = edit_asterisk(sec_spec, buff_max, buff);
         if (first_spec[0] == '\0' || !check_digit(first_spec) ||
             atoi(first_spec) <= 0 || !asterisk) {
            printf("Invalid Command\n");
         } else {
            run = append_line(&main_document, atoi(first_spec), sec_spec);
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (strcmp(command, "remove_line") == 0) {
         if (first_spec[0] == '\0' || !check_digit(first_spec) ||
             atoi(first_spec) <= 0 || sec_spec[0] == '\0' ||
             !check_digit(sec_spec) || atoi(sec_spec) <= 0 ||
             entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            run = remove_line(&main_document, atoi(first_spec),
                              atoi(sec_spec));
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (strcmp(command, "load_file") == 0) {
         if (first_spec[0] == '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            run = load_file(&main_document, first_spec);
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (strcmp(command, "replace_text") == 0) {

         if (!quote_check(buff_max, buff)) {
            printf("Invalid command\n");
         } else {
            two_qoute(first_spec, sec_spec, buff_max, buff);
            run = replace_text(&main_document, first_spec, sec_spec);
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (strcmp(command, "highlight_text") == 0) {
         if (first_spec[0] == '\0') {
            printf("Invalid command\n");
         } else {
            one_qoute(first_spec, buff_max, buff);
            highlight_text(&main_document, first_spec);
         }
      } else if (strcmp(command, "remove_text") == 0) {
         if (first_spec[0] == '\0') {
            printf("Invalid Command\n");
         } else {
            one_qoute(first_spec, buff_max, buff);
            remove_text(&main_document, first_spec);
         }
      } else if (strcmp(command, "save_document") == 0) {
         if (first_spec[0] == '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            run = save_document(&main_document, first_spec);
            if (run) {
               printf("%s failed\n", command);
            }
         }
      } else if (strcmp(command, "reset_document") == 0) {
         if (first_spec[0] != '\0' || sec_spec[0] != '\0' || entry[0] != '\0') {
            printf("Invalid Command\n");
         } else {
            reset_document(&main_document);
         }
      } else {
         printf("Invalid Command\n");
      }
      entry[0] = '\0';
      command[0] = '\0';
      first_spec[0] = '\0';
      sec_spec[0] = '\0';
      asterisk = 0;
   }
   fclose(input);
   return 0;
}

/* checks to see if param is a digit by iterating through and calling isdigit
returns 1 if digit and 0 if not */
int check_digit(char param[]) {
   int j, digit = 0;
   int stopper = strlen(param);
   for (j = 0; j < stopper; j++) {
      digit = isdigit(param[j]);
      if (!digit) {
         return 0;
      }
   }
   return 1;
}

/* edits param only if it has an asterisk to skip past the asterisk, returns 1
if it was able to do so and 0 otherwise */
int edit_asterisk(char *param, int length, char const ln[]) {
   int j, e = 0;
   int checker = 0;

   if (param[0] == '*') {
      for (j = 0; j < length; j++) {
         if (ln[j] != '\0' && ln[j] == '*' && ln[j + 1] == '\0') {
            strcpy(param, " ");
            return 1;
         }
         if (ln[j - 1] == '*') {
            checker = 1;
         }
         if (checker && ln[j] != '\0') {
            param[e++] = ln[j];
         }
      }

      param[e] = '\0';
      return 1;
   }
   return 0;

}

/* for use if user calls replace_text, simultaneously edits the first and
second parameters to just the actual content of the strings and not the "" */ 
void two_qoute(char *first_param, char *sec_param, int length,
               char const ln[]) {
   int i, j = 0;
   int k = 0;
   int checker = 0;
   int empty = 0;

   for (i = 0; i < length; i++) {
      if (ln[i] == '"') {
         checker += 1;
         continue;
      }
      if (checker && checker < 2) {
         first_param[j++] = ln[i];
      }
      if (checker > 2 && checker < 4) {
         sec_param[k++] = ln[i];
      }
   }
   first_param[j] = '\0';
   if (!empty) {
      sec_param[k] = '\0';
   }

}

/* for use in highlight and remove text, alters the param to remove the "" */
void one_qoute(char *first_param, int length, char const ln[]) {
   int i, j = 0;
   int checker = 0;

   for (i = 0; i < length; i++) {
      if (ln[i] == '"') {
         checker += 1;
         continue;
      }
      if (checker && checker < 2) {
         first_param[j++] = ln[i];
      }

   }
   first_param[j] = '\0';

}

/* checks if there are 4 quotes */
int quote_check(int length, char const ln[]) {
   int i, counter = 0;

   for (i = 0; i < length; i++) {
      if (ln[i] == '"') {
         counter += 1;
      }
   }
   if (counter == 4) {
      return 1;
   } else {
      return 0;
   }
}
