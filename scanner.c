#include <stdio.h>

/* Use the following switch to turn on/off debugging messages in your code. 
   Check the value of DEBUG wherever you want to print a debugging message
   in your code, e.g.:

   if (DEBUG)
      printf(...);
   
   This way you can simply turn on/off your debugging messages by setting 
   DEBUG to 1/0. By default it is off (0). See function printBuffer() for
   an example.
 */
#define DEBUG 0

#define TRUE  1
#define FALSE 0

//----------------------------- token types ------------------------------
#define KEYWORDS   12
#define RESERVED 	38
#define VAR 		1
#define BEGIN 		2
#define END 		3
#define ASSIGN 		4
#define IF 			5
#define WHILE 		6
#define DO 			7
#define THEN 		8
#define PRINT 		9
#define INT 		10
#define REAL 		11
#define STRING 		12
#define PLUS 		13
#define MINUS 		14
#define UNDERSCORE 	15
#define DIV 		16
#define MULT 		17
#define EQUAL 		18
#define COLON 		19
#define COMMA 		20
#define SEMICOLON 	21
#define LBRAC 		22
#define RBRAC 		23
#define LPAREN 		24
#define RPAREN 		25
#define NOTEQUAL 	26
#define GREATER 	27
#define LESS 		28
#define LTEQ 		29
#define GTEQ 		30
#define LSHIFT 		31
#define RSHIFT 		32
#define DOT 		33
#define NUM 		34
#define BASE08 		35
#define BASE16 		36
#define ID 			37
#define ERROR 		38

//------------------- reserved words and token strings -----------------------
char *reserved[] ={"",
    "VAR",
    "BEGIN",
    "END",
    "ASSIGN",
    "IF",
    "WHILE",
    "DO",
    "THEN",
    "PRINT",
    "INT",
    "REAL",
    "STRING",
    "+",
    "-",
    "_",
    "/",
    "*",
    "=",
    ":",
    ",",
    ";",
    "[",
    "]",
    "(",
    ")",
    "<>",
    ">",
    "<",
    "<=",
    ">=",
    "<<",
    ">>",
    ".",
    "NUM",
    "BASE08",
    "BASE16",
    "ID",
    "ERROR"};

int printToken(int ttype) {
    if (ttype <= RESERVED) {
        printf("%s\n", reserved[ttype]);
        return 1;
    } else
        return 0;
}
//---------------------------------------------------------

//---------------------------------------------------------
// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100

char token[MAX_TOKEN_LENGTH]; // token string
int tokenLength;
int line_no = 0;

//---------------------------------------------------------
// Global Variables associated with the input buffer
#define BUFFER_SIZE     200

int buffer[BUFFER_SIZE];
int start_pos = 0; // Signifies the start position in the buffer (#)
int next_pos = 0; // Signifies the next character to be read (^)
int maximal_pos = 0; // Signifies the last character of the maximal 
// token found (*)

//---------------------------------------------------------
// Functions associated with the input buffer

void readNextLine() {
    int i;
    char c;
    start_pos = 0;
    next_pos = 0;
    maximal_pos = 0;

    for (i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = EOF;

    i = 0;
    c = getchar();
    while (c != '\n' && !feof(stdin)) {
        buffer[i] = c;
        c = getchar();
        i++;
    }
    line_no++;
}

/* Use the following function for Debugging purposes */
void printBuffer() {
    if (DEBUG) {
        char str[BUFFER_SIZE];
        int i;
        for (i = 0; i < BUFFER_SIZE; i++) {
            if (buffer[i] >= 0)
                str[i] = (char) buffer[i];
            else {
                str[i] = '\0';
                break;
            }
        }
        printf("BUFFER: %s\n", str);
        printf("START_POS   = %d\n", start_pos);
        printf("NEXT_POS    = %d\n", next_pos);
        printf("MAXIMAL_POS = %d\n", maximal_pos);
    }
}

int skipSpace() {
    while (isspace((char) buffer[next_pos])) {
        next_pos++;
    }
    maximal_pos = next_pos;
    start_pos = next_pos;
}

//----------------------------------------------------------

int isKeyword(char *s) {
    int i;

    for (i = 1; i <= KEYWORDS; i++)
        if (strcmp(reserved[i], s) == 0)
            return i;
    return FALSE;
}

int isXDigit(char c) {
    // return true if c is a valid digit for base 16
    return isdigit(c) || (c == 'A') || (c == 'B') || (c == 'C') ||
            (c == 'D') || (c == 'E') || (c == 'F');
}

int scan_id_keyword() {
    int ttype;

    while (isalnum(buffer[next_pos])) {
        next_pos++;
    }
    maximal_pos = next_pos - 1;

    int i;
    for (i = start_pos; i <= maximal_pos; i++) {
        token[tokenLength] = (char) buffer[i];
        tokenLength++;
    }
    token[tokenLength] = '\0';

    ttype = isKeyword(token);
    if (ttype == 0)
        ttype = ID;
    return ttype;
}

int scan_number() {
    // This function does not take BASE08 and BASE16 into account!
    // You should consider adding functionality for scanning them. 
    // Depending on how you want to handle the identifying of those 
    // tokens, you might decide to rewrite the function.

    int ttype;

    char c;

    c = buffer[next_pos];
    next_pos++;
    if (isdigit(c)) { // The following code scans for a NUM 
        // 0 is a NUM by itself
        if (c == '0') {
            ttype = NUM;
            maximal_pos = next_pos - 1; // 0 is a maximal NUM token
            // Check for x08 or x16 after the zero
            c = buffer[next_pos];
            next_pos++;
            
            //if there is an x
            if (c == 'x') {
                c = buffer[next_pos];
                next_pos++;
                if (c == '0') {
                    c = buffer[next_pos];
                    next_pos++;
                    if (c == '8') {
                        maximal_pos = next_pos - 1; // 0x08 is a maximal 
                        // BASE08 token
                        ttype = BASE08;
                    }
                } else if (c == '1') {
                    c = buffer[next_pos];
                    next_pos++;
                    if (c == '6') {
                        maximal_pos = next_pos - 1; // 0x16 is a maximal 
                        // BASE16 token
                        ttype = BASE16;
                    }
                }
            }
        } else { //if it does not start with 0
            ttype = NUM;
            int canBe08 = 1; //keep track whether it can be base08
            int canBe16 = 1; //keep track whether it can be base16
            
            while (isdigit(c)) {
                
		maximal_pos = next_pos - 1;
               
                
                //see if base08 is disqualified
                if (atoi(&c) > 7){
                    canBe08 = 0;
                    
                }
                
                //check next character
                c = buffer[next_pos];
                next_pos++;
                
                //if the next char is x then it can be base08 or base16
                if (c == 'x') {
                    c = buffer[next_pos];
                    next_pos++;
                    //check to see if it is base08 or if base08 has been disqualified
                    if (c == '0' && canBe08 == 1) {
                        c = buffer[next_pos];
                        next_pos++;
                        if (c == '8') {
                            maximal_pos = next_pos - 1; // 'number'x08 is a maximal 
                            // BASE08 token
                            ttype = BASE08;
                        } else { //if a number is followed by x0 but not 8 then reset positions
                            next_pos = maximal_pos + 1;
                            c = buffer[next_pos];
                            next_pos++;
                        }
                    } else if (c == '1') { //see if it is base 16
                        c = buffer[next_pos];
                        next_pos++;
                        if (c == '6') {
                            maximal_pos = next_pos - 1; // 'number'x16 is a maximal 
                            // BASE16 token
                            ttype = BASE16;
                        } 
                        //if a number is followed by x1 but not 6 reset positions
                        else { 
                            next_pos = maximal_pos + 1;
                            c = buffer[next_pos];
                            next_pos++;
                            
                        }
                    }
                    //if a number is followed by x but not 16 or 08 reset positions
                    else { 
                        next_pos = maximal_pos + 1;
                        c = buffer[next_pos];
                        next_pos++;
                        
                    }
                } 
                //if a number is followed by A-F check to see if it is hexadecimal
                else if (isXDigit(c) && !isdigit(c)) { 
                    //check to see whether base16 is still viable
                    while (isXDigit(c) && canBe16 == 1) {
                        c = buffer[next_pos];
                        next_pos++;
                        //if next character is x it can be base16
                        if (c == 'x') {
                            c = buffer[next_pos];
                            next_pos++;
                            if (c == '1') {
                                c = buffer[next_pos];
                                next_pos++;
                                if (c == '6') {
                                    maximal_pos = next_pos - 1; // 'number'x16 is a maximal 
                                    // BASE16 token
                                    ttype = BASE16;
                                } else {  //if a number is followed by x1 but not 6 reset positions
                                    next_pos = maximal_pos + 1;
                                    canBe16 = 0;
                                    c = buffer[next_pos];
                                    next_pos++;
                                }
                            } else { //if a number is followed by x but not 16 reset positions
                                next_pos = maximal_pos + 1;
                                canBe16 = 0;
                                c = buffer[next_pos];
                                next_pos++;
                            }
                        }

                    }
                }
                
                
            }
            		
	
            
        }
    } else if (isXDigit(c)) {
        // It starts with a letter (A..F) so we should keep track of an ID
        // Add code here to handle this case
        // Remove the following line:
        //by default assume its id unless proven base16
        ttype = ID;
        
        while (isXDigit(c)) {
            
            maximal_pos = next_pos - 1;
            
            //get next position
            c = buffer[next_pos];
            next_pos++;
            
            //case that disqualifies base 16, scan for keyword or id
            if(c != 'x' && !isXDigit(c)){
                ttype = scan_id_keyword();
                return ttype;
             
            }
            
            //case that matches base16
            if (c == 'x') {
                c = buffer[next_pos];
                next_pos++;
                if (c == '1') {
                    c = buffer[next_pos];
                    next_pos++;
                    //matched base16
                    if (c == '6') {
                        maximal_pos = next_pos - 1; // 0x16 is a maximal 
                        // BASE16 token
                        ttype = BASE16;
                        // Copy token from buffer
                        int i;
                        for (i = start_pos; i <= maximal_pos; i++) {
                            token[tokenLength] = (char) buffer[i];
                            tokenLength++;
                        }
                        token[tokenLength] = '\0';
                        // Rewind the next_pos pointer to proper position
                        next_pos = maximal_pos + 1;
                        return ttype;
                        
                    } else { // in case number followed by x1 but not 6 scan for id
                        ttype = scan_id_keyword();
                        return ttype;
                    }
                } else { //in case number followed by x but not 16 scan for id
                    ttype = scan_id_keyword();
                    return ttype;
                }
            }

        }
      
    } else
        ttype = ERROR;
    // Copy token from buffer
    int i;
    for (i = start_pos; i <= maximal_pos; i++) {
        token[tokenLength] = (char) buffer[i];
        tokenLength++;
    }
    token[tokenLength] = '\0';
    // Rewind the next_pos pointer to proper position
    next_pos = maximal_pos + 1;
    return ttype;
}

int getToken() {
    char c;
    int ttype;
    tokenLength = 0;

    skipSpace();

    c = buffer[next_pos];
    next_pos++;

    switch (c) {
        case '.': ttype = DOT;
            break;
        case '+': ttype = PLUS;
            break;
        case '-': ttype = MINUS;
            break;
        case '_': ttype = UNDERSCORE;
            break;
        case '/': ttype = DIV;
            break;
        case '*': ttype = MULT;
            break;
        case '=': ttype = EQUAL;
            break;
        case ':': ttype = COLON;
            break;
        case ',': ttype = COMMA;
            break;
        case ';': ttype = SEMICOLON;
            break;
        case '[': ttype = LBRAC;
            break;
        case ']': ttype = RBRAC;
            break;
        case '(': ttype = LPAREN;
            break;
        case ')': ttype = RPAREN;
            break;
        case '<':
            c = buffer[next_pos];
            next_pos++;
            if (c == '=')
                ttype = LTEQ;
            else if (c == '>')
                ttype = NOTEQUAL;
                // Add code to handle LSHIFT
            else if (c == '<') {
                ttype = LSHIFT;
            } else {
                next_pos--;
                ttype = LESS;
            }



            break;
        case '>':
            c = buffer[next_pos];
            next_pos++;
            if (c == '=')
                ttype = GTEQ;
                // Add code to handle RSHIFT
            else if (c == '>')
                ttype = RSHIFT;
            else {
                next_pos--;
                ttype = GREATER;
            }



            break;
        default:
            if (isXDigit(c)) // token is either NUM, BASE08, BASE16 or ID
            {
                next_pos--;
                ttype = scan_number();
            } else if (isalpha(c)) // Token is either a keyword or ID
            {
                next_pos--;
                ttype = scan_id_keyword();
            } else if (c == EOF)
                ttype = EOF;
            else
                ttype = ERROR;
            break;
    } // End Switch
    // Don't need to update buffer positions for the next 
    // call to getToken() since skipSpace() handles it.
    return ttype;
}

int main() {
    int ttype;

    while (!feof(stdin)) {
        readNextLine();
        //printBuffer();
        while ((ttype = getToken()) != EOF) { // EOF indicates that we have reached the end of the buffer
            //printBuffer();
            printf("%d %d ", line_no, ttype);
            if ((ttype == NUM) || (ttype == ID) || (ttype == BASE08) ||
                    (ttype == BASE16)) {
                printf("%s \n", token);
            } else {
                printToken(ttype);
            }
        }
    }
    return 0;
}


