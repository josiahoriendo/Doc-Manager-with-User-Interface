#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "document.h"

static void remove_helper(const char *ptr, int paragraph_number,
                          int line_number, Document * doc, int length);

static int replace_helper(const char *ptr, int paragraph_number,
                          int line_number, Document * doc,
                          const char *replace, int length);

static int highlight_helper(const char *ptr, int paragraph_number,
                            int line_number, Document * doc, int length);

/* aux function for remove_text that iterates through the doc and once it
 reaches the address of the first instance of the target, it will skip over 
the target word by incrementing the index by adding the length of the word to
 the ptr for the original line effectively removing the word. After the 
iteration is finished it will copy the new string to the line in the doc */
static void remove_helper(const char *ptr, int paragraph_number,
                          int line_number, Document * doc, int length) {
   int i, j, string_length =
      strlen(doc->paragraphs[paragraph_number].lines[line_number]);
   char new_line[MAX_STR_SIZE + 1] = "\0";

   j = 0;
   for (i = 0; i < string_length - length + 1; i++) {
      if (i == ptr - doc->paragraphs[paragraph_number].lines[line_number]) {
         j += length;
      }
      new_line[i] = doc->paragraphs[paragraph_number].lines[line_number][j];
      j += 1;
   }
   strcpy(doc->paragraphs[paragraph_number].lines[line_number], new_line);
}

/* aux function for replace_text that iterates through doc and once the loop 
reaches the first instance, it will iterate through the word that will replace
the target setting equal to the position at which the target was. Otherwise 
the new_line will copy in the characters from the previous line */
static int replace_helper(const char *ptr, int paragraph_number,
                          int line_number, Document * doc,
                          const char *replace, int length) {
   int i, k, string_length =
      strlen(doc->paragraphs[paragraph_number].lines[line_number]);
   int j = 0;
   int l = 0;
   int replace_length = strlen(replace);
   char new_line[MAX_STR_SIZE + 1] = "\0";
   int end_brack_indx = 0;

   for (i = 0; i < string_length + 1; i++) {
      if (i == ptr - doc->paragraphs[paragraph_number].lines[line_number]) {
         for (k = 0; k < replace_length; k++) {
            new_line[j++] = replace[k];
            while (l < length - 1) {
               i += 1;
               l += 1;
            }
         }

         end_brack_indx = i + (replace_length - length);
         continue;
      }
      new_line[j++] = doc->paragraphs[paragraph_number].lines[line_number][i];
   }
   strcpy(doc->paragraphs[paragraph_number].lines[line_number], new_line);
   return end_brack_indx;
}

/* aux function for highlight_text that iterates through initial line setting
 the characters to the new_line. If the target text is reached, it will place
 the HIGHLIGHT_START_STR and then place the word and once it reaches the end, 
it will place the HIGHLIGHT_END_STR. Once it reaches the end, it will copy the
 new_line to the original line. It returns the end bracket index in order to 
prevent the strstr function from counting the target instance that was already
 found */
static int highlight_helper(const char *ptr, int paragraph_number,
                            int line_number, Document * doc, int length) {
   int i, string_length =
      strlen(doc->paragraphs[paragraph_number].lines[line_number]);
   int j = 0;
   int end_brack_indx = 0;
   char new_line[MAX_STR_SIZE + 1] = "\0";

   for (i = 0; i < string_length + 3; i++) {
      if (i == ptr - doc->paragraphs[paragraph_number].lines[line_number]) {
         new_line[i] = HIGHLIGHT_START_STR[0];
         continue;
      }
      if (i == ptr - doc->paragraphs[paragraph_number].lines[line_number]
          + length + 1) {
         new_line[i] = HIGHLIGHT_END_STR[0];
         end_brack_indx = i;
         continue;
      }
      new_line[i] = doc->paragraphs[paragraph_number].lines[line_number][j];
      j += 1;
   }
   strcpy(doc->paragraphs[paragraph_number].lines[line_number], new_line);
   return end_brack_indx;
}

/* if doc is null, name is null or the length of name is greater then max 
string size then the function will return failure. Otherwise, initializes doc 
to empty by setting number of paragraphs to 0 and copying name to the doc field
 name and returning success*/
int init_document(Document * doc, const char *name) {
   if (doc == NULL || name == NULL || strlen(name) > MAX_STR_SIZE) {
      return FAILURE;
   } else {
      doc->number_of_paragraphs = 0;
      strcpy(doc->name, name);
      return SUCCESS;
   }
}

/* If doc is not null it will reset the doc by setting the field number of 
paragraphs to 0 and return success, otherwise it will return failure */
int reset_document(Document * doc) {
   if (doc != NULL) {
      doc->number_of_paragraphs = 0;
      return SUCCESS;
   } else {
      return FAILURE;
   }
}

/* if the doc is not null, will print doc name, number of paragraphs and lines
 of the paragraph through a nested for loop. Will have newline characters 
between each paragraphs */
int print_document(Document * doc) {
   int i, j;

   if (doc != NULL) {
      printf("Document name: \"%s\"\n", doc->name);
      printf("Number of Paragraphs: %d\n", doc->number_of_paragraphs);

      for (i = 0; i < doc->number_of_paragraphs; i++) {
         for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            printf("%s\n", doc->paragraphs[i].lines[j]);
         }
         if (doc->paragraphs[i].number_of_lines != 0 &&
             doc->number_of_paragraphs > 1 &&
             i != doc->number_of_paragraphs - 1) {
            printf("\n");
         }
      }

      return SUCCESS;
   } else {
      return FAILURE;
   }
}

/* does this work close to max */
/* adds paragraph after the specified paragraph number by storing the paragraph
 that may already exist in that location and replacing it with the new 
paragraph. If there are other paragraphs that follow it, it will use a for loop
 to iterate through all the paragraphs and move all the following down */
int add_paragraph_after(Document * doc, int paragraph_number) {
   Paragraph prev, curr, add;
   int i = paragraph_number;
   add.number_of_lines = 0;
   add.lines[0][0] = '\0';

   if (doc == NULL || doc->number_of_paragraphs == MAX_PARAGRAPHS ||
       paragraph_number > doc->number_of_paragraphs) {
      return FAILURE;
   } else {
      if (paragraph_number == 0) {
         i = 0;
      }
      prev = doc->paragraphs[i];
      doc->paragraphs[i] = add;
      for (; i < doc->number_of_paragraphs; i++) {
         curr = doc->paragraphs[i + 1];
         doc->paragraphs[i + 1] = prev;
         prev = curr;
      }
      doc->number_of_paragraphs += 1;
      return SUCCESS;
   }
}

int add_line_after(Document * doc, int paragraph_number, int line_number,
                   const char *new_line) {
   char prev[MAX_STR_SIZE + 1] = "\0";
   char curr[MAX_STR_SIZE + 1] = "\0";
   int i = line_number;

   if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
       doc->paragraphs[paragraph_number - 1].number_of_lines ==
       MAX_PARAGRAPH_LINES || line_number >
       doc->paragraphs[paragraph_number - 1].number_of_lines ||
       new_line == NULL || paragraph_number == 0) {
      return FAILURE;
   } else {
      if (line_number == 0) {
         i = 0;
      }
      strcpy(prev, doc->paragraphs[paragraph_number - 1].lines[i]);
      strcpy(doc->paragraphs[paragraph_number - 1].lines[i], new_line);
      for (; i < doc->paragraphs[paragraph_number - 1].number_of_lines; i++) {
         strcpy(curr, doc->paragraphs[paragraph_number - 1].lines[i + 1]);
         strcpy(doc->paragraphs[paragraph_number - 1].lines[i + 1], prev);
         strcpy(prev, curr);
      }
      doc->paragraphs[paragraph_number - 1].number_of_lines += 1;
      return SUCCESS;
   }
}

int get_number_lines_paragraph(Document * doc, int paragraph_number,
                               int *number_of_lines) {
   if (doc == NULL || number_of_lines == NULL ||
       paragraph_number - 1 > doc->number_of_paragraphs ||
       paragraph_number == 0) {
      return FAILURE;
   } else {
      *number_of_lines =
         doc->paragraphs[paragraph_number - 1].number_of_lines;
      return SUCCESS;
   }
}

/* relies on add line after so if add line doesnt work this doesnt either */
int append_line(Document * doc, int paragraph_number, const char *new_line) {
   if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
       doc->paragraphs[paragraph_number - 1].number_of_lines ==
       MAX_PARAGRAPH_LINES || new_line == NULL || paragraph_number == 0) {
      return FAILURE;
   } else {
      add_line_after(doc, paragraph_number,
                     doc->paragraphs[paragraph_number - 1].number_of_lines,
                     new_line);
      return SUCCESS;
   }
}

/* does this work for maximum number of lines */
int remove_line(Document * doc, int paragraph_number, int line_number) {
   int i, max_line;

   if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
       line_number > doc->paragraphs[paragraph_number - 1].number_of_lines ||
       line_number == 0 || paragraph_number == 0) {
      return FAILURE;
   } else {
      max_line = doc->paragraphs[paragraph_number - 1].number_of_lines;
      for (i = line_number - 1; i < max_line - 1; i++) {
         strcpy(doc->paragraphs[paragraph_number - 1].lines[i]
                , doc->paragraphs[paragraph_number - 1].lines[i + 1]);
         if (i == (max_line - 1)) {
            doc->paragraphs[paragraph_number - 1].lines[max_line - 1][0]
               = '\0';
         }
      }
      doc->paragraphs[paragraph_number - 1].number_of_lines -= 1;
      return SUCCESS;
   }
}

/* adds lines from the data to the doc by traversing through the data 2d array
and everytime an instance of the null byte appears, it will signal to make a 
new paragraph.  */
int load_document(Document * doc, char data[][MAX_STR_SIZE + 1],
                  int data_lines) {
   int i, paragraph_counter = 1;
   int line_number = 0;

   if (doc == NULL || data == NULL || data_lines <= 0) {
      return FAILURE;
   } else {
      add_paragraph_after(doc, 0);
      for (i = 0; i < data_lines; i++) {
         if (data[i][0] == '\0') {
            add_paragraph_after(doc, paragraph_counter);
            paragraph_counter += 1;
            line_number = 0;
         } else {
            add_line_after(doc, paragraph_counter, line_number, data[i]);
            line_number += 1;
         }
      }

      return SUCCESS;
   }
}

/* replaces target text everywhere in the doc by traversing through the lines 
2d array and calling the helper method on each of the lines. Everytime the 
helper method is called, will have the for loop return to that iteration to 
call the helper method on it again to check for any other occurences. */
int replace_text(Document * doc, const char *target, const char *replacement) {
   char *ptr = NULL;
   int i, j, length = strlen(target);
   int check_again = 0;
   int end_brack_indx = 0;

   if (doc == NULL || target == NULL || replacement == NULL) {
      return FAILURE;
   } else {
      if (strcmp(replacement, "") == 0) {
         remove_text(doc, target);
         return SUCCESS;
      }
      for (i = 0; i < doc->number_of_paragraphs; i++) {
         for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            ptr = strstr(doc->paragraphs[i].lines[j] + (end_brack_indx *
                                                        check_again), target);
            if (ptr != NULL) {
               end_brack_indx = replace_helper(ptr, i, j, doc, replacement,
                                               length);
               j -= 1;
               check_again = 1;
            } else {
               check_again = 0;
               end_brack_indx = 0;
            }
            ptr = NULL;
         }
      }
      return SUCCESS;
   }
}

/* highlights the target tet anywhere in the doc by traversing through the 2d
array and calling the helper method on each of the lines. Everytime the helper
method is called, will have the for loop return to that iteration to call the
helper method on the line again to check for any other occurences of the 
target. Will do ptr arithmetic if it is the second iteration of a cetain line
 as the strstr function will find the first occurence of the target if not 
otherwise altered */
int highlight_text(Document * doc, const char *target) {
   char *ptr = NULL;
   int i, j, length = strlen(target);
   int check_again = 0;
   int end_brack_indx = 0;

   if (doc == NULL || target == NULL) {
      return FAILURE;
   } else {
      for (i = 0; i < doc->number_of_paragraphs; i++) {
         for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            ptr = strstr(doc->paragraphs[i].lines[j] + (end_brack_indx *
                                                        check_again), target);
            if (ptr != NULL) {
               end_brack_indx = highlight_helper(ptr, i, j, doc, length);
               j -= 1;
               check_again = 1;
            } else {
               check_again = 0;
               end_brack_indx = 0;
            }
            ptr = NULL;
         }
      }
      return SUCCESS;
   }
}

/* removes the target text anywhere in the doc by traversing through the 2d
array and calling the helper method on each of the lines. Everytime the helper
method is called, will have the for loop return to that iteration to call the
helper method on the line again to check for any other occurences of the 
target. */
int remove_text(Document * doc, const char *target) {
   char *ptr = NULL;
   int i, j, length = strlen(target);

   if (doc == NULL || target == NULL) {
      return FAILURE;
   } else {
      for (i = 0; i < doc->number_of_paragraphs; i++) {
         for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            ptr = strstr(doc->paragraphs[i].lines[j], target);
            if (ptr != NULL) {
               remove_helper(ptr, i, j, doc, length);
               j -= 1;
            }
            ptr = NULL;
         }
      }
      return SUCCESS;
   }
}

int load_file(Document * doc, const char *filename) {
   FILE *input;
   int i, k, par_num = 1;
   int line_number = 0;
   int empty = 1;
   char line[MAX_STR_SIZE + 1] = "\0";

   if (doc == NULL || filename == NULL) {
      return FAILURE;
   } else {
      input = fopen(filename, "r");
      if (input == NULL) {
         return FAILURE;
      }
      add_paragraph_after(doc, 0);
      while (fgets(line, MAX_STR_SIZE + 1, input) != NULL) {
         int max = strlen(line);
         for (i = 0; i < max; i++) {
            empty = isspace(line[i]);
            if (!empty) {
               break;
            }
         }
         for (k = 0; k < max; k++) {
            if (line[k] == '\n') {
               line[k] = '\0';
            }
         }

         if (empty) {
            add_paragraph_after(doc, par_num);
            par_num += 1;
            line_number = 0;

         } else {
            add_line_after(doc, par_num, line_number, line);
            line_number += 1;
         }
         empty = 0;
      }
      fclose(input);
      return SUCCESS;
   }
}

int save_document(Document * doc, const char *filename) {
   FILE *output;
   int i, j;

   if (doc == NULL || filename == NULL) {
      return FAILURE;
   } else {
      output = fopen(filename, "w");
      if (output == NULL) {
         return FAILURE;
      }
      for (i = 0; i < doc->number_of_paragraphs; i++) {
         for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            fputs(doc->paragraphs[i].lines[j], output);
            fputs("\n", output);
         }
         if (doc->paragraphs[i].number_of_lines != 0 &&
             doc->number_of_paragraphs > 1 &&
             i != doc->number_of_paragraphs - 1) {
            fputs("\n", output);
         }
      }
      fclose(output);

      return SUCCESS;
   }
}
