#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NORW 14
#define IMAX 32767
#define CMAX 11
#define STRMAX 256

typedef enum {skipsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6, slashsym = 7, fisym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym =
12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
semicolonsym = 18, periodsym = 19, becomessym = 20,
beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26,
callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33} token_type;

/* list of reserved word names */
char *word[] = { "begin", "fi", "call", "const", "do", "else", "end", "if",
  "procedure", "read", "then", "var", "while", "write" };

/* internal representation of reserved words */
int wsym [ ] = { beginsym, fisym, callsym, constsym, dosym, elsesym, endsym, ifsym,
  procsym, readsym, thensym, varsym, whilesym, writesym};

void printing(token_type *lexeme, char** iden, int lexemeMax, int intarr[], char** substrarr, FILE *fp2, int skip[]);
int isSymbol(char c);
int isLet(char c);
int isNum(char c);
int isReserved(char* string);
int convert(char* string);
char* substring(const char* input, int start, int end);

int main(int argc, char** argv) {
    // Declaring too many variables (lol)
    //printf("yo-2");
    char words[500];
    token_type *lexeme = malloc(500 * sizeof(token_type)); // Allocate memory for lexeme
    FILE* fp = fopen(argv[1], "r");
    FILE* fp2 = fopen("output.txt", "w");
    char** iden = malloc(500 * sizeof(char*)); // Name array
    int* intarr = malloc(500 * sizeof(int)); // int array
    char** substrarr = malloc(500*sizeof(char*)); // char array
    for (int i = 0; i < 500; i++)
        substrarr[i] = malloc (500 * sizeof(char));
    int index = 0; //which token
    int count = 0; //with char in string
    int idenCount = 0; 
    int intCount = 0;
    int hold;
    int start = 0;
    char* substr = (char*) malloc (100 * sizeof(char));
    int templen = 0;
    int substrind = 0;
    int skip[500];
    skip[0] = -1;
    int skipind = 0;
    //char* error = (char*) malloc (25 * sizeof(char));
    fprintf(fp2, "Source Program:\n");
    char symb[500];
    while(fgets(words, 500, fp) != NULL ) // Loop through current line
    {
        fprintf(fp2, "%s", words);
        count = 0; // Index of token
        while (count != strlen(words)) // Loop through each identifier/ symbol/ operator/ reserved word/ number
        {
            start = count;
            if (isspace(words[count])) {
                count++;
                continue;
            }
            if(isLet(words[count])) // Start of reserved word/ identifier
            {
                while (isLet(words[count]) || isNum(words[count])) // Loop through input until there's a symbol or whitespace
                    count++;
                strcpy(substr, substring(words, start, count));
                if(count - start > CMAX + 1) // Error: Name too long
                {
                    substr = realloc(substr, strlen(substr) + 24);
                    strcat(substr, "\tError: Name too long\n");
                    strcpy(substrarr[substrind++], substr);
                    skip[skipind++] = index;
                    continue;
                }
                
                
                hold = isReserved(substr); // Check if substring is a Reserved word
                if(hold != NORW) { // Reserved word found
                    lexeme[index] = wsym[hold]; // Add (reserved word) to lexeme
                    index++;
                    strcpy(substrarr[substrind], substr);
                    substrind++;
                } else { // Identifier found
                    templen = strlen(substr);
                    lexeme[index] = identsym; // Add 2 (identsym) to lexeme
                    index++;
                    iden[idenCount] = malloc(templen + 1);
                    strcpy(iden[idenCount], substr);
                    iden[idenCount][templen] = '\0';
                    idenCount++;
                }
                continue;
            } else if(isNum(words[count])) {  // Start of number
                int num;
                while (isNum(words[count]))
                    count++;
                strcpy(substr, substring(words, start, count));
                num = convert(substr);
                if(num > IMAX) { // Error: Number too long
                    substr = realloc(substr, strlen(substr) + 26);
                    strcat(substr, "\tError: Number too long\n");
                    strcpy(substrarr[substrind++], substr);
                    skip[skipind++] = index;
                    continue;
                }
                intarr[intCount++] = num; 
                lexeme[index++] = numbersym;
            } // Check if a SYMBOL is read:
              // ‘+’, ‘-‘, ‘*’, ‘/’, ‘(‘, ‘)’, ‘=’, ’,’, ‘.’, ‘<’, ‘>’, ‘;’, ’:’ 
            else if (isSymbol(words[count])) { // Symbol found
                if (words[count]  == '+') {
                    strcpy(substrarr[substrind], "+");
                    substrind++;
                    lexeme[index++] = plussym;
                } else if (words[count]  == '-') {
                    strcpy(substrarr[substrind], "-");
                    substrind++;
                    lexeme[index++] = minussym;
                } else if (words[count]  == '*') {
                    strcpy(substrarr[substrind], "*");
                    substrind++;
                    lexeme[index++] = multsym;
                } else if(words[count] == '/') {
                    if (words[count + 1] == '*') { // Comment found
                        count += 2;
                        while (words[count] != '*' ||  words[count + 1] != '/') {
                            count++;
                            if (count == strlen(words)) {
                                fgets(words, 500, fp);
                                count = 0;
                                while(words[0] == '\n')
                                    fgets(words, 500, fp);
                            }
                        }
                        count += 2;
                    } else {
                        lexeme[index++] = slashsym;
                        strcpy(substrarr[substrind], "/");
                        substrind++;
                    }
                }
                else if(words[count] == '(') {
                    strcpy(substrarr[substrind], "(");
                    substrind++;
                    lexeme[index++] = lparentsym;
                } else if(words[count] == ')') {
                    strcpy(substrarr[substrind], ")");
                    substrind++;
                    lexeme[index++] = rparentsym;
                } else if(words[count] == '=') {
                    strcpy(substrarr[substrind], "=");
                    substrind++;
                    lexeme[index++] = eqlsym;
                } else if(words[count] == ',') {
                    strcpy(substrarr[substrind], ",");
                    substrind++;
                    lexeme[index++] = commasym;
                } else if(words[count] == '.') {
                    strcpy(substrarr[substrind], ".");
                    substrind++;
                    lexeme[index++] = periodsym;
                } else if(words[count] == '<') {
                    if (words[count + 1] == '=') {
                        strcpy(substrarr[substrind], "<=");
                        substrind++;
                        lexeme[index++] = leqsym;
                        count++;
                    }else if (words[count + 1] == '>') {
                        strcpy(substrarr[substrind], "<>");
                        substrind++;
                        lexeme[index++] = neqsym;
                        count++;
                    } else {
                        strcpy(substrarr[substrind], "<");
                        substrind++;
                        lexeme[index++] = lessym;
                    }
                } else if(words[count] == '>') {
                    if (words[count + 1] == '=') {
                        strcpy(substrarr[substrind], ">=");
                        substrind++;
                        lexeme[index++] = geqsym;
                        count++;
                    } else {
                        strcpy(substrarr[substrind], ">");
                        substrind++;
                        lexeme[index++] = gtrsym;
                     }
                } else if(words[count] == ';') {
                    strcpy(substrarr[substrind], ";");
                    substrind++;
                    lexeme[index++] = semicolonsym;
                } else if(words[count] == ':') {
                    if (words[count + 1] == '=') {
                        strcpy(substrarr[substrind], ":=");
                        substrind++;
                        lexeme[index++] = becomessym;
                        count++;
                    } // Case for " : ", display invalid symbols error message
                    else {
                        for (int i = 0; i < strlen(words); i++)
                            substr[i] = '\0';
                        substr = realloc(substr, strlen(substr) + 25);
                        substr[0] = ':';
                        substr = strcat(substr, "\t\tError: Invalid symbol\n");
                        strcpy(substrarr[substrind++], substr);
                        skip[skipind++] = index;
                    }
                 } 
                count++;
                continue;
            } else {  // Error: Invalid symbols
                for (int i = 0; i < strlen(words); i++)
                    substr[i] = '\0';
                substr = realloc(substr, strlen(substr) + 25);
                substr[0] = words[count++];
                substr = strcat(substr, "\t\tError: Invalid symbol\n");
                strcpy(substrarr[substrind++], substr);
                skip[skipind++] = index;
             }
        }
    }
    idenCount = 0;
    intCount = 0;
    skipind = 0;
    // REAL PRINTING CODE:
    printing(lexeme, iden, index, intarr, substrarr, fp2, skip);
    fprintf(fp2, "\nToken List:\n");
    for(int i = 0; i<index ; i++){
        fprintf(fp2, "%d ", lexeme[i]);
        if(lexeme[i] == 2)
            fprintf(fp2, "%s ", iden[idenCount++]);
        if(lexeme[i] == 3)
            fprintf(fp2, "%d ", intarr[intCount++]);
    }

    // Free all elements
    for (int i = 0; i < 500; i++)
        free(substrarr[i]);
    free(substrarr);
    free(substr);
    for (int i = 0; i < idenCount; i++)
        free(iden[i]);
    free(iden);
    free(intarr);
    free(lexeme);

    fclose(fp);
    fclose(fp2);
    
    return 0;
  
}
void printing(token_type *lexeme, char** iden, int lexemeMax, int intarr[], char** substrarr, FILE *fp2, int skip[]) {
    fprintf(fp2, "\n\nLexeme Table:\n");
    fprintf(fp2, "\nlexeme\ttoken type\n");
    int track1 = 0;
    int track2 = 0;
    int track3 = 0;
    int skipind = 0;
    for (int i = 0; i < lexemeMax; i++) { // Loop through every lexeme, print from substrarr (substring array) too
        if(i == skip[skipind]){ // Print error message
            fprintf(fp2, "%s", substrarr[track3++]);
            skip[skipind++] = -1;
            i--;
        }
        else if (lexeme[i] == 2) { // Print identifier
            fprintf(fp2, "%s", iden[track1]);
            track1++;
            fprintf(fp2, "\t\t2\n");
        } else if (lexeme[i] == 3) { // Print number
            fprintf(fp2, "%d", intarr[track2]);
            track2++;
            fprintf(fp2, "\t\t3\n");
        } else { // Print all other cases (not error, identifier, or number)
           fprintf(fp2, "%s", substrarr[track3]);
            track3++;
            if (strlen(substrarr[track3 - 1]) > 5) fprintf(fp2, "\t%d\n", lexeme[i]);
            else fprintf(fp2, "\t\t%d\n", lexeme[i]);
        }
    }
}
int isSymbol(char c) {
    //‘+’, ‘-‘, ‘*’, ‘/’, ‘(‘, ‘)’, ‘=’, ’,’ , ‘.’, ‘ <’, ‘>’, ‘;’ , ’:’ 
    if (c == '+'  || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '=' || c == ',' || c == '.' || c == '<' || c == '>' || c == ';' || c == ':') return 1;
    else return 0;
}

int isLet(char c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) 
    return 1;
  else 
    return 0; 
}

int isNum(char c) {
  if (c >= '0' && c <= '9')
      return 1;
    else 
      return 0; 
}

int isReserved(char* string){
  for(int i = 0; i < NORW; i++)
    if(strcmp(string, word[i]) == 0){
      return i;
    }
  return NORW;
}

int convert (char* string) {
  return atoi(string);
}

char* substring(const char* input, int start, int end) {
    int length = end - start;
    char* sub = (char*)malloc((length + 1) * sizeof(char)); // Allocate memory for the substring (+1 for the null terminator)
    strncpy(sub, input + start, length);
    sub[length] = '\0'; // Null terminate the substring
    return sub;
}