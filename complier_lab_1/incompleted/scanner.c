#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;
extern CharCode charCodes[];


void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

void skipComment() {
  int startLineNo = lineNo;
  int startColNo = colNo - 1;
  
  readChar(); 
  
  while (currentChar != EOF) {
    if (charCodes[currentChar] == CHAR_TIMES) {
      readChar();
      if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, startLineNo, startColNo);
      }
      if (charCodes[currentChar] == CHAR_RPAR) {
        readChar();
        return;
      }
    } else {
      readChar();
    }
  }
  
  error(ERR_ENDOFCOMMENT, startLineNo, startColNo);
}

Token* readIdentKeyword(void) {
  Token* token = makeToken(TK_IDENT, lineNo, colNo);
  int count = 0;
  
  while (currentChar != EOF && 
         (charCodes[currentChar] == CHAR_LETTER || 
          charCodes[currentChar] == CHAR_DIGIT)) {
    if (count >= MAX_IDENT_LEN) {
      error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
    }
    token->string[count++] = (char)currentChar;
    readChar();
  }
  
  token->string[count] = '\0';
  
  token->tokenType = checkKeyword(token->string);
  
  return token;
}

Token* readNumber(void) {
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);
  int value = 0;
  
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
    value = value * 10 + (currentChar - '0');
    readChar();
  }
  
  token->value = value;
  return token;
}

Token* readConstChar(void) {
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  
  readChar();
  
  if (currentChar == EOF) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  
  token->string[0] = (char)currentChar;
  token->string[1] = '\0';
  
  readChar(); 
  
  if (currentChar == EOF || charCodes[currentChar] != CHAR_SINGLEQUOTE) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  
  readChar(); 
  
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: 
    skipBlank(); 
    return getToken();
    
  case CHAR_LETTER: 
    return readIdentKeyword();
    
  case CHAR_DIGIT: 
    return readNumber();
    
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_TIMES: 
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_SLASH: 
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_LT:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else {
      return makeToken(SB_LT, ln, cn);
    }
    
  case CHAR_GT:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else {
      return makeToken(SB_GT, ln, cn);
    }
    
  case CHAR_EXCLAIMATION:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else {
      error(ERR_INVALIDSYMBOL, ln, cn);
    }
    
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_COMMA: 
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_PERIOD:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_RPAR) {
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    } else {
      return makeToken(SB_PERIOD, ln, cn);
    }
    
  case CHAR_COLON:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else {
      return makeToken(SB_COLON, ln, cn);
    }
    
  case CHAR_SEMICOLON: 
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_SINGLEQUOTE: 
    return readConstChar();
    
  case CHAR_LPAR:
    ln = lineNo; 
    cn = colNo;
    readChar();
    if (currentChar != EOF) {
      if (charCodes[currentChar] == CHAR_TIMES) {
        skipComment();
        return getToken();
      } else if (charCodes[currentChar] == CHAR_PERIOD) {
        readChar();
        return makeToken(SB_LSEL, ln, cn);
      }
    }
    return makeToken(SB_LPAR, ln, cn);
    
  case CHAR_RPAR: 
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar(); 
    return token;
    
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}

/******************************************************************/

void printToken(Token *token);

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == 0) {
    printf("Can not open input file!\n");
    return -1;
  }

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return 0;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == 0) {
    printf("Scan successfully!\n");
    return 0;
  } else {
    printf("Scan unsuccessfully! %s\n", argv[1]);
    return -1;
  }
}