// Jake Weber
// Eduardo Salcedo Fuentes
// 3/10/24

// Parser/code generator implementation (Only used as intermediary code for compiler.c)

// Implement a Recursive Descent Parser and Intermediate Code Generator for tiny PL/0
// code copied over from lex.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NORW 14
#define IMAX 32767
#define CMAX 11
#define STRMAX 256
#define CODE_SIZE 1000

typedef struct {
int kind; // const = 1, var = 2, proc = 3
char name[10]; // name up to 11 chars
int val; // number (ASCII value)
int level; // L level
int addr; // M address
int mark; // to indicate unavailable or deleted
} symbol;


typedef enum {oddsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6, slashsym = 7, fisym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym =
12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
semicolonsym = 18, periodsym = 19, becomessym = 20,
beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26,
callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33} token_type;

typedef enum { LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8, SYS = 9
} opCode;

char *opCodeNames[] = {"", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS"};

typedef enum { ADD = 2, SUB = 2, MUL = 2, DIV = 2, EQL = 2, NEQ = 2, LSS = 2, LEQ = 2, GTR = 2, GEQ = 2, ODD = 2
} OPR_type;

typedef enum { ONE = 1, TWO = 2, THREE = 3
} SYS_type;

typedef struct IR {
    char op[500];
    //int op;
    int L, M;
} IR;

// Global Variables
char** tokenList;
int token = 0;
symbol symbol_table[500];
int symbolIndex = 0;
IR text[500];
int cx = 0;
int jpcIdx = 0;

/* list of reserved word names */
char *word[] = { "begin", "fi", "call", "const", "do", "else", "end", "if", "procedure", "read", "then", "var", "while", "write" };

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

// Function Prototypes
void program();
void block();
void const_declaration();
int var_declaration();
void factor();
void emit(int op, int L, int M);
void term();
void statement();
void condition();
void expression();
void factor();
void printEmit();

int main(int argc, char** argv) {
    // Declaring too many variables (lol)
    char words[500];
    token_type *lexeme = malloc(500 * sizeof(token_type)); // Allocate memory for lexeme
    FILE* fp = fopen(argv[1], "r");
    FILE* fp2 = fopen(argv[2], "w");
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
    fprintf(fp2, "Source program:\n");
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
                    printf("Error: Name too long\n");
                    exit(0);
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
                    printf("Number too long\n");
                    exit(0);
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
                        printf("Error: Invalid symbol\n");
                        exit(0);
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
                printf("Error: Invalid symbol\n");
                exit(0);
             }
        }
    }
    idenCount = 0;
    intCount = 0;
    skipind = 0;
    tokenList = malloc(500 * sizeof(char*));
    for (int i = 0; i < 500; i++)
        tokenList[i] = malloc(500 * sizeof(char));
    
    // REAL PRINTING CODE:
    printing(lexeme, iden, index, intarr, substrarr, fp2, skip);

    int curSymbolIndex = 0;
    int symbolFound = -1;
    fprintf(fp2, "\nToken List:\n");
    int tokenListCount = 0;
    for(int i = 0; i < index ; i++) {
        sprintf(tokenList[tokenListCount], "%d", lexeme[i]);
        fprintf(fp2, "%s ", tokenList[tokenListCount++]);
        if(lexeme[i] == 2) {
            sprintf(tokenList[tokenListCount], "%s", iden[idenCount++]);
            fprintf(fp2, "%s ", tokenList[tokenListCount++]);
        } else if(lexeme[i] == 3) {
            sprintf(tokenList[tokenListCount], "%d", intarr[intCount++]);
            fprintf(fp2, "%s ", tokenList[tokenListCount++]);
        }
    }

    //Reads and evaluates syntax and checks for eroor
    program ();
    //print code and symbol table
    printf("Assembly Code: \n\n"); //printf("%d |\t\t |",)
    printEmit();

    printf("\nSymbol Table:\n\n");
    printf("Kind | Name\t\t\t| Value | Level | Address | Mark\n");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < symbolIndex; i++) {
        printf("   %d |%13s", symbol_table[i].kind, symbol_table[i].name);
        printf(" |\t  %d |\t  %d", symbol_table[i].val, symbol_table[i].level);
        printf(" |\t\t%d |\t  %d\n", symbol_table[i].addr, symbol_table[i].mark);
    }
    //printf("%d |\t\t |",)
    //printf("Line\tOP\tL\tM\n"); <--- USE LATER

    //printf("Kind | Name\t\t| Value | Level | Address | Mark\n---------------------------------------");       //printf("%d |\t\t |",)
    
                
    // Free all elements
    for (int i = 0; i < 500; i++)
        free(substrarr[i]);
    free(substrarr);
    free(substr);
    for (int i = 0; i < 500; i++)
        free(tokenList[i]);
    free(tokenList);
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

int symbolTableCheck (char* identifier) {
    for (int i = 0; i < 500; i++) {
        if (strcmp(identifier, symbol_table[i].name) == 0)
            return i;
    }
    return -1;
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


//Program code
void program () {
    block();
    //check for period at end
    if (atoi(tokenList[token]) != periodsym) {
        printf("Error: program must end with period\n");
        exit(0);
    }
    //emit sys end commands
    emit(SYS, 0, 3);
}

//block function
void block () {
    //start with jmp
    emit(JMP, 0, 3);
    //declare constants
    const_declaration();
    //declare vars
    int numVars = var_declaration();
    //emit inc
    emit(INC, 0, 3 + numVars);
    //statement
    statement();
}

void const_declaration () {
    //check for constantsym
    if (atoi(tokenList[token]) == constsym) {
        do {
            // get next token;
            token++;
            //check for indentifier next
            if (atoi(tokenList[token]) != identsym) {
                printf("Error: const, var, and read keywords must be followed by identifier\n");
                exit(0);
            }

            token++;
            if (symbolTableCheck(tokenList[token]) != -1) {
                printf("Error: symbol name has already been declared\n");
                exit(0);
            }
            //save ident name
            char *ident = malloc(12 * sizeof(char));
            ident = tokenList[token];
            
            // get next token;
            token++;
            //printf("THIS TOKEN: %s", tokenList[token]);
            if (atoi(tokenList[token]) != eqlsym) {
                printf("Error: constants must be assigned with =\n");
                exit(0);
            }
            // get next token;
            token++;
            if (atoi(tokenList[token]) != numbersym) {
                printf("Error: constants must be assigned an integer value\n");
                exit(0);
            }
            token++;
            //add to symbol table (kind 1, saved name, number, 0, 0)
            symbol_table[symbolIndex].kind = 1;
            strcpy(symbol_table[symbolIndex].name, ident);
            symbol_table[symbolIndex].val = atoi(tokenList[token]);
            symbol_table[symbolIndex].addr = 0;
            symbol_table[symbolIndex].level = 0;
            symbol_table[symbolIndex].mark = 0;
            symbolIndex++;
            
            // get next token;
            token++;
        }
        while (atoi(tokenList[token]) == commasym);
        if (atoi(tokenList[token]) != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon\n");
            exit(0);
        }
        // get next token;
        token++;
    }
}
            
int var_declaration () {
    int numVars = 0;
    if (atoi(tokenList[token]) == varsym) {
        do {
            //printf("token: %d\n", token);
            numVars++;
            // get next token;
            token++;
            //check for indentifier next
            if (atoi(tokenList[token]) != identsym) {
                printf("Error: const, var, and read keywords must be followed by identifier%d\n", numVars);
                exit(0);
            }
            token++;
            //printf("token: %d\n", token);
            //printf("tokenList[token]: %s, numVars: %d\n", tokenList[token], numVars);
            if (symbolTableCheck(tokenList[token]) != -1) {
                printf("Error: symbol name has already been declared\n\n");
                exit(0);
            }
            //add to symbol table (kind 2, ident, 0, 0, var# + 2)
            symbol_table[symbolIndex].kind = 2;
            strcpy(symbol_table[symbolIndex].name, tokenList[token]);
            symbol_table[symbolIndex].val = 0;
            symbol_table[symbolIndex].addr = numVars + 2;
            symbol_table[symbolIndex].level = 0;
            symbol_table[symbolIndex].mark = 0;
            symbolIndex++;
            //printf("   %d",symbol_table[symbolIndex-1].addr);
            // get next token;
            token++;
        }
        while (atoi(tokenList[token]) == commasym);
        if (atoi(tokenList[token]) != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon%d\n", numVars);
            exit(0);
        }
        // get next token
        token++;
    }
    return numVars;
}

void statement() {
    if (atoi(tokenList[token]) == identsym){
        token++;
        int curSymbolIndex = symbolTableCheck(tokenList[token]);
        if (curSymbolIndex == -1) {
            printf("Error: undeclared identifier\n");
            exit(0);
        }
        if (symbol_table[curSymbolIndex].kind != 2){
            printf("Error: only variable values may be altered\n");
            exit(0);
        }
        // get next token
        token++;
        if (atoi(tokenList[token]) != becomessym) {
            printf("Error: assignment statements must use :=\n");
            exit(0);
        }
        // get next token
        token++;
        //printf("current token: %s\n", tokenList[token]);
        expression();
        //token++;
        emit(STO, 0, symbol_table[curSymbolIndex].addr);
        symbol_table[curSymbolIndex].mark = 1;
        //printf("CHOLE");
        return;
    }
    if (atoi(tokenList[token]) == beginsym){
        do {
            // get next token
            token++;
            //printf("current token: %d\n", atoi(tokenList[token]));
            statement();
            //printf("current token: %d\n", atoi(tokenList[token]));
        } while (atoi(tokenList[token]) == semicolonsym);
        if (atoi(tokenList[token]) != endsym) {
            printf("\n\ntokenList[token]: %s\n", tokenList[token]);
            printf("Error: begin must be followed by end\n");
            exit(0);
        }
        // get next token
        token++;
        return;
    }
    if (atoi(tokenList[token]) == ifsym){
        // get next token
        token++;
        condition();
        jpcIdx = cx;
        emit(JPC, 0, 3*jpcIdx);
        if (atoi(tokenList[token]) != thensym) {
            printf("Error: if must be followed by then\n");
            exit(0);
        }
        // get next token
        token++;
        statement();
        if (atoi(tokenList[token]) != fisym) {
            printf("Error: if must end with a fi\n");
            exit(0);
        }
        token++;
        text[jpcIdx].M = cx;
        return;
    }
    if (atoi(tokenList[token]) == whilesym){
        // get next token
        token++;
        int loopIdx = cx;
        condition();
        if (atoi(tokenList[token]) != dosym) {
            printf("Error: while must be followed by do\n");
            exit(0);
        }
        // get next token
        token++;
        jpcIdx = cx;
        emit(JPC, 0, 3*jpcIdx);
        statement();
        emit(JPC, 0, 3*loopIdx);
        text[jpcIdx].M = 3*cx;
        return;
    }
    if (atoi(tokenList[token]) == readsym) {
        // get next token
        token++;
        if (atoi(tokenList[token]) != identsym) {
            printf("Error: const, var, and read keywords must be followed by identifier\n");
            exit(0);
        }
        symbolIndex = symbolTableCheck(tokenList[token]);
        if (symbolIndex == -1) {
            printf("Error: undeclared identifier\n");
            exit(0);
        }
        if (symbol_table[symbolIndex].kind != 2) {
            printf("Error: only variable values may be altered\n");
            exit(0);
        }
        // get next token
        token++;
        emit(SYS, 0, 2);
        emit (STO, 0, symbol_table[symbolIndex].addr);
        return;
    }
    if (atoi(tokenList[token]) == writesym) {
        // get next token
        token++;
        expression();
        emit(SYS, 0, 1);
        return;
    }
}  // eduardo is a cutie pie
    
void condition() {
    if (atoi(tokenList[token]) == oddsym) {
        // get next token
        token++;
        expression();
        emit(ODD, 0, 11);
    } else {
        expression();
        if (atoi(tokenList[token]) == eqlsym) {
            // get next token
            token++;
            expression();
            emit(EQL, 0, 5);
        } else if (atoi(tokenList[token]) == neqsym) {
            // get next token
            token++;
            expression();
            emit(NEQ, 0, 6);
        } else if (atoi(tokenList[token]) == lessym) {
            // get next token
            token++;
            expression();
            emit(LSS, 0, 7);
        } else if (atoi(tokenList[token]) == leqsym) {
            // get next token
            token++;
            expression();
            emit(LEQ, 0, 8);
        } else if (atoi(tokenList[token]) == gtrsym) {
            // get next token
            token++;
            expression();
            emit(GTR, 0, 9);
        } else if (atoi(tokenList[token]) == geqsym) {
            // get next token
            token++;
            expression();
            emit(GEQ, 0, 10);
        } else {
            printf("Error: condition must contain comparison operator\n");
            exit(0);
        }
    }
}
    
void expression() { // (HINT: modify it to match the grammar)
    term();
    while (atoi(tokenList[token]) == plussym || atoi(tokenList[token]) == minussym) {
        if (atoi(tokenList[token]) == plussym) {
            // get next token
            token++;
            term();
            emit(ADD, 0, 1);
        } else if (atoi(tokenList[token]) == minussym) {
            // get next token
            token++;
            term();
            emit(SUB, 0, 2);
        } else if (atoi(tokenList[token]) == semicolonsym) {
            //token++;
            //return;
        }
        /*  else if (atoi(tokenList[token]) == multsym) {
            // get next token
            token++;
            term();
            emit(MUL, 0, 3);
        }  else if (atoi(tokenList[token]) == slashsym) {
            // get next token
            token++;
            term();
            emit(DIV, 0, 4);
        }*/
    }
    //token++;
    //printf("expression: current token: %s\n", tokenList[token]);
}
    
void term() {
    factor();
    //printf("\ncur token before hold: %d\n", atoi(tokenList[token]));
    while (atoi(tokenList[token]) == multsym || atoi(tokenList[token]) == slashsym) { // IGNORE modsym
        if (atoi(tokenList[token]) == multsym) {
            // get next token
            token++;
            factor();
            //printf("\nCUR TOKEN IN LOOP TERM1: %d\n", atoi(tokenList[token]));
            emit(MUL, 0, 3);
        }
        else if (atoi(tokenList[token]) == slashsym) {
            // get next token
            token++;
            factor();
            //printf("\nCUR TOKEN IN LOOP TERM2: %d\n", atoi(tokenList[token]));
            emit(DIV, 0, 4);
        }
    }
    //printf("\nCUR TOKEN IN TERM: %d\n", atoi(tokenList[token]));
}
    //Wo ai ni
void factor() {
    //printf("token: %s\n", tokenList[token]);
    if(atoi(tokenList[token]) == identsym) {
        token++;
        //printf("oh %s oh\n", tokenList[token]);
        int curSymbolIndex = symbolTableCheck(tokenList[token]);
        if (curSymbolIndex == -1) {
            printf("Error: undeclared indentifier\n");
            exit(0);
        }
        if (symbol_table[curSymbolIndex].kind == 1) { // const
            emit(LIT, 0, symbol_table[curSymbolIndex].val);
            symbol_table[curSymbolIndex].mark = 1;
        } else { // var
            //printf("CHOLE\n");
            emit(LOD,  0, symbol_table[curSymbolIndex].addr);
            symbol_table[curSymbolIndex].mark = 1;
        }
        // get next token
        token++;
    } else if (atoi(tokenList[token]) == numbersym) {
        //printf("NUMBERSYM IN FACTOR\n");
        token++;
        emit(LIT, 0, atoi(tokenList[token]));
        // get next token
        token++;
        //printf("tokenHERUIFBS YES: %s\n", tokenList[token]);
        return;
    } else if (atoi(tokenList[token]) == lparentsym) {
        // get next token
        token++;
        expression();
        if (atoi(tokenList[token]) != rparentsym) {
            printf("Error: right parenthesis must follow left parenthesis\n");
            exit(0);
        }
        // get next token
        token++;
    //else if (atoi(tokenList[token]) == semicolonsym) {
        //token++;
        //return;
    } else {
        //return;
        //printf("ABOVE ARITH\ttoken: %s\n", tokenList[token] );
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
        exit(0);
    }
}

    //copy IRs
void emit(int op, int L, int M) {
    strcpy(text[cx].op, opCodeNames[op]); // opcode
    text[cx].L = L; // lexicographical level
    text[cx].M = M; // modifier
    cx++;
}

    //print emit table
void printEmit() {
    printf("Line\tOP\t\tL\t\tM\n");
    for(int i = 0; i < cx; i++) {
        if (strlen(text[i].op) >= 4)
            printf(" %d\t\t%s\t%d\t\t%d\n", i , text[i].op, text[i].L, text[i].M );
        else
            printf(" %d\t\t%s\t\t%d\t\t%d\n", i , text[i].op, text[i].L, text[i].M );
    }
}
