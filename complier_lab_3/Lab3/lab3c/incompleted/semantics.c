/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  // Tìm kiếm đối tượng theo tên trong bảng ký hiệu
  // Tìm từ scope hiện tại đến scope toàn cục
  Scope* scope = symtab->currentScope;
  Object* obj;
  
  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) 
      return obj;
    scope = scope->outer;
  }
  
  return NULL;
}

void checkFreshIdent(char *name) {
  // Kiểm tra xem identifier có phải là mới (chưa được khai báo trong scope hiện tại)
  Object* obj = findObject(symtab->currentScope->objList, name);
  
  if (obj != NULL) {
    // Tất cả các trường hợp trùng tên đều dùng ERR_DUPLICATE_IDENT
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
  }
}

Object* checkDeclaredIdent(char* name) {
  // Kiểm tra xem identifier đã được khai báo chưa
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredConstant(char* name) {
  // Kiểm tra xem tên có phải là hằng đã khai báo không
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  
  if (obj->kind != OBJ_CONSTANT) {
    error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredType(char* name) {
  // Kiểm tra xem tên có phải là kiểu đã khai báo không
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  
  if (obj->kind != OBJ_TYPE) {
    error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredVariable(char* name) {
  // Kiểm tra xem tên có phải là biến đã khai báo không
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  
  if (obj->kind != OBJ_VARIABLE) {
    error(ERR_INVALID_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredFunction(char* name) {
  // Kiểm tra xem tên có phải là hàm đã khai báo không
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  
  if (obj->kind != OBJ_FUNCTION) {
    error(ERR_INVALID_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredProcedure(char* name) {
  // Kiểm tra xem tên có phải là thủ tục đã khai báo không
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  
  if (obj->kind != OBJ_PROCEDURE) {
    error(ERR_INVALID_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  
  return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
  // Kiểm tra xem tên có phải là LValue (có thể gán giá trị được)
  // LValue có thể là: biến, tham số, hoặc tên hàm (để gán giá trị trả về)
  Object* obj = lookupObject(name);
  
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }
  
  switch (obj->kind) {
    case OBJ_VARIABLE:
    case OBJ_PARAMETER:
      return obj;
      
    case OBJ_FUNCTION:
      // Trong một số ngôn ngữ, tên hàm có thể được dùng để gán giá trị trả về
      // Kiểm tra xem có đang ở trong scope của chính hàm đó không
      if (obj == symtab->currentScope->owner) {
        return obj;
      }
      // Nếu không, báo lỗi
      error(ERR_INVALID_LVALUE, currentToken->lineNo, currentToken->colNo);
      break;
      
    default:
      error(ERR_INVALID_LVALUE, currentToken->lineNo, currentToken->colNo);
      break;
  }
  
  return NULL;
}