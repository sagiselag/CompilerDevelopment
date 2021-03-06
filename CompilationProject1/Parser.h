#pragma once
#include "../Token.h"
#include "SymbolTable.h"

void parse();
void parse_PROG();
void parse_GLOBAL_VARS();
void parse_GLOBAL_VARS_FOLLOW();
IDItem* parse_VAR_DEC();
IDItem* parse_TYPE_DEC(int isParam);
void parse_END_VAR();
eType parse_TYPE();
void parse_DIM_SIZES();
void parse_ARRAY_FOLLOW();
void parse_FUNC_PREDEFS();
void parse_FUNC_PREDEFS_FOLLOW();
IDItem* parse_FUNC_PROTOTYPE(int oneIfIsFullDef);
void parse_FUNC_FULL_DEFS();
void parse_FUNC_WITH_BODY_FOLLOW();
void parse_FUNC_WITH_BODY();
eType parse_RETURNED_TYPE();
void parse_PARAMS();
void parse_PARAM_LIST();
void parse_PARAM_FOLLOW();
void parse_PARAM();
void parse_COMP_STMT();
void parse_VAR_DEC_LIST();
void parse_STMT_LIST();
void parse_STMT_LIST_FOLLOW();
void parse_STMT();
void parse_IF_STMT();
void parse_CALL();
TypeList* parse_ARGS();
void parse_ARG_LIST();
void parse_ARG_LIST_FOLLOW();
void parse_RETURN_STMT();
void parse_RETURN_STMT_FOLLOW();
void parse_VAR();
void parse_VAR_FOLLOW();
void parse_EXPR_LIST();
void parse_EXPR_LIST_FOLLOW();
void parse_CONDITION();
eType parse_EXPR();
eType parse_EXPR_FOLLOW();
eType parse_TERM();
eType parse_TERM_FOLLOW();
eType parse_FACTOR();
void parse_VAR_OR_CALL_FOLLOW();
void parse_VAR_WITH_EXPR_OR_CALL_FOLLOW();

char* parseFunctionString[] =
{
	"",
	"PROG",
	"GLOBAL_VARS",
	"GLOBAL_VARS_FOLLOW",
	"VAR_DEC",
	"TYPE_DEC",
	"END_VAR",
	"TYPE",
	"DIM_SIZES",
	"ARRAY_FOLLOW",
	"FUNC_PREDEFS",
	"FUNC_PREDEFS_FOLLOW",
	"FUNC_PROTOTYPE",
	"FUNC_FULL_DEFS",
	"FUNC_WITH_BODY_FOLLOW",
	"FUNC_WITH_BODY",
	"RETURNED_TYPE",
	"PARAMS",
	"PARAM_LIST",
	"PARAM_FOLLOW",
	"PARAM",
	"COMP_STMT",
	"VAR_DEC_LIST",
	"STMT_LIST",
	"STMT_LIST_FOLLOW",
	"STMT",
	"IF_STMT",
	"CALL",
	"ARGS",
	"ARG_LIST",
	"ARG_LIST_FOLLOW",
	"RETURN_STMT",
	"RETURN_STMT_FOLLOW",
	"VAR",
	"VAR_FOLLOW",
	"EXPR_LIST",
	"EXPR_LIST_FOLLOW",
	"CONDITION",
	"EXPR",
	"EXPR_FOLLOW",
	"TERM",
	"TERM_FOLLOW",
	"FACTOR",
	"VAR_OR_CALL_FOLLOW",
	"VAR_WITH_EXPR_OR_CALL_FOLLOW"
};

char* parserRulesExperssionStrings[] =
{
	"",
	"GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS",
	"VAR_DEC GLOBAL_VARS'",
	"VAR_DEC GLOBAL_VARS' | e",
	"TYPE_DEC;",
	"TYPE id END_VAR",
	"[DIM_SIZES] | e",
	"int | float",
	"int_num ARRAY'",
	", int_num ARRAY' | e",
	"FUNC_PROTOTYPE; FUNC_PREDEFS'",
	"FUNC_PROTOTYPE; FUNC_PREDEFS' |  e",
	"RETURNED_TYPE id(PARAMS)",
	"FUNC_WITH_BODY FUNC_WITH_BODY'",
	"FUNC_WITH_BODY FUNC_WITH_BODY' | e",
	"FUNC_PROTOTYPE COMP_STMT",
	"TYPE | void",
	"PARAM_LIST | e",
	"PARAM  PARAM'",
	", PARAM  PARAM' | e",
	"TYPE_DEC",
	"{ VAR_DEC_LIST STMT_LIST }",
	"VAR_DEC VAR_DEC_LIST | e",
	"STMT STMT_LIST'",
	"; STMT STMT_LIST'  | e",
	"id VAR_WITH_EXPR_OR_CALL' | COMP_STMT | IF_STMT | RETURN_STMT",
	"if (CONDITION) STMT",
	"id(ARGS)",
	"ARG_LIST | e",
	"EXPR ARG_LIST'",
	", EXPR  ARG_LIST' | e",
	"return RETURN_STMT'",
	"EXPR | e",
	"id VAR'",
	"[EXPR_LIST] | e",
	"EXPR  EXPR_LIST'",
	", EXPR_LIST | e",
	"EXPR rel_op EXPR",
	"TERM EXPR'",
	"+ TERM EXPR' | e",
	"FACTOR TERM'",
	"* FACTOR TERM' | e",
	"id VAR_OR_CALL' | int_num | float_num | ( EXPR )",
	"VAR' | ( ARGS )",
	"VAR' = EXPR | ( ARGS ) ",
};

#define REAL_OP  EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, LOWER, LOWER_EQUAL 

int firsts[45][16] =
{
	{NULL},
	{INT, FLOAT},
	{INT, FLOAT},
	{INT, FLOAT},
	{INT, FLOAT},
	{INT, FLOAT},
	{OPEN_BRACKETS},
	{INT, FLOAT},
	{INTEGER_NUMBER},
	{COMMA},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT, VOID},
	{INT, FLOAT},
	{INT, FLOAT},
	{COMMA},
	{INT, FLOAT},
	{OPEN_CURLYBRACES},
	{INT, FLOAT},
	{ID, OPEN_CURLYBRACES, IF, RETURN},
	{SEMICOLON},
	{ID, OPEN_CURLYBRACES, IF, RETURN},
	{IF},
	{ID},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{COMMA},
	{RETURN},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{ID},
	{OPEN_BRACKETS},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{COMMA},
	// Condition firsts error will come from EXPR therefor the Condition firsts array will include real_op only
	{REAL_OP},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{PLUS_OPERATION},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{MULIPLICATION_OPERATION},
	{ID, INTEGER_NUMBER, FLOAT_NUMBER, OPEN_PARENTHESES},
	{OPEN_BRACKETS, OPEN_PARENTHESES},
	{OPEN_BRACKETS, OPEN_PARENTHESES, PLACING_OPERATION}
};

int follows[45][16] =
{
	{NULL},
	{EOF_tok},
	{INT,FLOAT,VOID},
	{INT,FLOAT,VOID},
	{INT,FLOAT,VOID,ID,OPEN_CURLYBRACES, IF,RETURN},
	{SEMICOLON, COMMA, CLOSE_PARENTHESES},
	{SEMICOLON, COMMA, CLOSE_PARENTHESES},
	{ID},
	{CLOSE_BRACKETS},
	{CLOSE_BRACKETS},
	{INT,FLOAT,VOID},
	{INT,FLOAT,VOID},
	{SEMICOLON,OPEN_CURLYBRACES},
	{EOF_tok},
	{EOF_tok},
	{INT,FLOAT,VOID,EOF_tok},
	{ID},
	{CLOSE_PARENTHESES },
	{CLOSE_PARENTHESES },
	{CLOSE_PARENTHESES },
	{CLOSE_PARENTHESES, COMMA},
	{INT,FLOAT,VOID,EOF_tok,SEMICOLON, CLOSE_CURLYBRACES},
	{ID, OPEN_CURLYBRACES, IF, RETURN},
	{CLOSE_CURLYBRACES},
	{CLOSE_CURLYBRACES},
	{CLOSE_CURLYBRACES, SEMICOLON},
	{CLOSE_CURLYBRACES, SEMICOLON},
	{NULL},
	{CLOSE_PARENTHESES},
	{CLOSE_PARENTHESES},
	{CLOSE_PARENTHESES},
	{CLOSE_CURLYBRACES, SEMICOLON},
	{CLOSE_CURLYBRACES, SEMICOLON},
	{NULL},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP, MULIPLICATION_OPERATION, PLACING_OPERATION},
	{CLOSE_BRACKETS},
	{CLOSE_BRACKETS},
	{CLOSE_PARENTHESES},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP, MULIPLICATION_OPERATION},
	{CLOSE_PARENTHESES, PLUS_OPERATION, COMMA, SEMICOLON, CLOSE_CURLYBRACES, CLOSE_BRACKETS, REAL_OP, MULIPLICATION_OPERATION},
	{SEMICOLON, CLOSE_CURLYBRACES}
};




typedef struct ErrorNode
{
	char* erroeMsg;
	struct ErrorNode* prev;
	struct ErrorNode* next;
} ErrorNode;

void lexErrorStopParsingAndContinueOnlyLexAnalysis();
void create_new_error_node();
void addErrorParseMessage(int firstIndex, Token actualToken);
void addErrorMatchMessage(eTOKENS expectedTokenKind, Token actualToken);
void printAllErrorsToScreenForDevalopmentUse();
int goToFollowTokenAndReturnOneIFEOF(eParseFunction parseFunction);