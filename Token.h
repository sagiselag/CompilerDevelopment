#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout;

typedef enum eTOKENS
{
	//Identifiers
	ID = 1,

	//Numbers
	INTEGER_NUMBER,
	FLOAT_NUMBER,

	//Keywords
	IF,
	WHILE,
	VOID,
	RETURN,
	INT,
	FLOAT,	

	//Separation signs
	COMMA,
	COLON,
	SEMICOLON,
	OPEN_PARENTHESES,
	CLOSE_PARENTHESES,
	OPEN_BRACKETS,
	CLOSE_BRACKETS,
	OPEN_CURLYBRACES,
	CLOSE_CURLYBRACES,	

	//Operations 
	//arithmetic
	PLUS_OPERATION,
	MULIPLICATION_OPERATION,

	//comparison
	GREATER,
	GREATER_EQUAL,
	LOWER,
	LOWER_EQUAL,
	EQUAL,
	NOT_EQUAL,
	PLACING_OPERATION,

	//EOF
	EOF_tok

}eTOKENS;

typedef enum eParseFunction
{
	PROG = 1,
	GLOBAL_VARS,
	GLOBAL_VARS_FOLLOW,
	VAR_DEC,
	TYPE_DEC,
	END_VAR,
	TYPE,
	DIM_SIZES,
	ARRAY_FOLLOW,
	FUNC_PREDEFS,
	FUNC_PREDEFS_FOLLOW,
	FUNC_PROTOTYPE,
	FUNC_FULL_DEFS,
	FUNC_WITH_BODY_FOLLOW,
	FUNC_WITH_BODY,
	RETURNED_TYPE,
	PARAMS,
	PARAM_LIST,
	PARAM_FOLLOW,
	PARAM,
	COMP_STMT,
	VAR_DEC_LIST,
	STMT_LIST,
	STMT_LIST_FOLLOW,
	STMT,
	IF_STMT,
	CALL,
	ARGS,
	ARG_LIST,
	ARG_LIST_FOLLOW,
	RETURN_STMT,
	RETURN_STMT_FOLLOW,
	VAR,
	VAR_FOLLOW,
	EXPR_LIST,
	EXPR_LIST_FOLLOW,
	CONDITION,
	EXPR,
	EXPR_FOLLOW,
	TERM,
	TERM_FOLLOW,
	FACTOR,
	VAR_OR_CALL_FOLLOW,
	VAR_WITH_EXPR_OR_CALL_FOLLOW
}eParseFunction;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void stopParsingBecauseLexErrorContinueOnlyLexicalAnalysis();
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();
void match(eTOKENS tokenKind, eParseFunction funcName);
void parserError(eParseFunction);
//int parserErrorAndReturnOneIfEOF(eParseFunction);
void matchError(eTOKENS expectedTokenKind, eParseFunction funcName);
void goBackToFirstNode();

#endif