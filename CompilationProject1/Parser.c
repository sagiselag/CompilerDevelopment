#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Parser.h"
#include "SymbolTable.h"

FILE *parserFile;
ErrorNode* currentErrorNode = NULL;
int reachedUnexpectedEOF = 0, lexicalError = 0;
SymbolTable* symbolTable = NULL;
int currDimentionSize = -1;
eType returnedType = TYPE_NOT_DEFINED;

//char* eTokensString23[] =
//{
//	"EMPTY",
//	"ID",
//	"INTEGER_NUMBER",
//	"FLOAT_NUMBER",
//	"IF",
//	"WHILE",
//	"VOID",
//	"RETURN",
//	"INT",
//	"FLOAT",
//	"COMMA",
//	"COLON",
//	"SEMICOLON",
//	"OPEN_PARENTHESES",
//	"CLOSE_PARENTHESES",
//	"OPEN_BRACKETS",
//	"CLOSE_BRACKETS",
//	"OPEN_CURLYBRACES",
//	"CLOSE_CURLYBRACES",
//	"PLUS_OPERATION",
//	"MULIPLICATION_OPERATION",
//	"GREATER",
//	"GREATER_EQUAL",
//	"LOWER",
//	"LOWER_EQUAL",
//	"EQUAL",
//	"NOT_EQUAL",
//	"PLACING_OPERATION",
//	"EOF_tok"
//};

//#define TOKEN_ARRAY_SIZE 1000
#define TOKEN_ARRAY_SIZE 3

/*
* This function creates a token and stores it in the storage.
*/
void create_new_error_node()
{
	// case 1: there is still no tokens in the storage.
	if (currentErrorNode == NULL)
	{
		currentErrorNode = (ErrorNode*)malloc(sizeof(ErrorNode));

		if (currentErrorNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentErrorNode->prev = NULL;
		currentErrorNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node		
		currentErrorNode->next = (ErrorNode*)malloc(sizeof(ErrorNode));

		if (currentErrorNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentErrorNode->next->prev = currentErrorNode;
		currentErrorNode = currentErrorNode->next;
		currentErrorNode->next = NULL;
	}	
}

void printRule(eParseFunction parseFunction) {
	char ruleStr[1000] = "";
	char* ruleDinamicStr;

	strcat(ruleStr, "Rule { ");
	strcat(ruleStr, parseFunctionString[parseFunction]);
	strcat(ruleStr, " -> ");
	strcat(ruleStr, parserRulesExperssionStrings[parseFunction]);
	strcat(ruleStr, " } \n");
	fprintf(parserFile, ruleStr);
}



void lexErrorStopParsingAndContinueOnlyLexAnalysis() {
	if (reachedUnexpectedEOF == 0 && lexicalError == 0) {
		reachedUnexpectedEOF = 1;
		lexicalError = 1;
		fprintf(parserFile, "Parsing stopped because there was a lexical analysis error\n");
		stopParsingBecauseLexErrorContinueOnlyLexicalAnalysis();
	}
}

Token* getCurr() {
	next_token();
	Token* tok = back_token();

	return tok;
}

int getIDListSize(IDList* list) {	
	int res = 0;
	IDList* curr = list;	
	while (curr != NULL) {
		res++;
		curr = curr->next;
	}

	return res;
}

int getArraySize(IntegerList* list) {
	int res = 0;
	IntegerList* curr = list;
	while (curr != NULL) {
		res++;
		curr = curr->next;
	}

	return res;
}

void parse_BEGIN_BLOCK() {
	symbolTable = make_table(symbolTable);
}

void parse_END_BLOCK(int isRegularBlock) {	
	if (isRegularBlock) {
		while (symbolTable->idList != NULL) {
			if (symbolTable->idList->id.hasBeenUsed == 0) {
				// id hasn't been used
				if (symbolTable->idList->id.role == ROLE_FULL_DEFINITION || symbolTable->idList->id.role == ROLE_PRE_DEFINITION) {
					fprintf(semanticFile, "Error: Function {'%s'} hasn't been used.\n", symbolTable->idList->id.name);
				}
				else {
					fprintf(semanticFile, "Error: Variable {'%s'} hasn't been used.\n", symbolTable->idList->id.name);
				}
			}
			symbolTable->idList = symbolTable->idList->next;
		}
	}
	symbolTable = pop_table(symbolTable);
}

void parse(char* parserOutputFileFullPath, char* semanticOutputFileFullPath) {
	parserFile = fopen(parserOutputFileFullPath, "w");	
	semanticFile = fopen(semanticOutputFileFullPath, "w");
	initializeSymbolTable();
	symbolTable = currTable;

	goBackToFirstNode();
	parse_PROG();
	if (reachedUnexpectedEOF) reachedUnexpectedEOF = 0;
	// Stop parsing and continue only lex analysis if had a lex error
	/* 
	but Mark said to continue in such a case 
	so we put the call to lexErrorStopParsingAndContinueOnlyLexAnalysis function on a note  in the lex file 
	*/
	if (lexicalError) {
		while (yylex() != 0);
		lexicalError = 0;
	}
	match(EOF_tok, 0);
	//printAllErrorsToScreenForDevalopmentUse();
	fclose(parserFile);	
	fclose(semanticFile);
}

void parse_PROG() {
	if (reachedUnexpectedEOF) return;
	printRule(PROG);
	parse_GLOBAL_VARS();
	parse_FUNC_PREDEFS();
	parse_FUNC_FULL_DEFS();
	// TODO predef test
	 parse_END_BLOCK(1);
}

void parse_GLOBAL_VARS() {
	if (reachedUnexpectedEOF) return;
	printRule(GLOBAL_VARS);
	parse_VAR_DEC();
	parse_GLOBAL_VARS_FOLLOW();
}

void parse_GLOBAL_VARS_FOLLOW() {
	if (reachedUnexpectedEOF) return;
	printRule(GLOBAL_VARS_FOLLOW);
	// after parse_GLOBAL_VARS_FOLLOW there are two options: 1. VAR_DEC 2. FUNC_PREDEFS
	// FUNC_PREDEFS start with: INT/FLOAT/VOID		-> ID		-> OPEN_PARENTHESES
	// VAR_DEC start with:		INT/FLOAT			-> ID		-> SEMICOLON/OPEN_BRACKETS
	// therefor, if the 3rd Token is OPEN_PARENTHESES we should consider the GLOBAL_VARS_FOLLOW as an epsilon, otherwise we will consider it as VAR_DEC
	Token* token;
	next_token();
	next_token();
	token = next_token();
	back_token();
	back_token();
	back_token();

	switch (token->kind)
	{
	case OPEN_PARENTHESES:		
		break;	
	default:
		parse_VAR_DEC();
		parse_GLOBAL_VARS_FOLLOW();
		break;
	}	
}

IDItem* parse_VAR_DEC()
{
	if (reachedUnexpectedEOF) return;
	printRule(VAR_DEC);
	IDItem* item = parse_TYPE_DEC(0);
	match(SEMICOLON, VAR_DEC);

	return item;
}

IDItem* parse_TYPE_DEC(int isParams) {
	if (reachedUnexpectedEOF) return;
	printRule(TYPE_DEC);	
	eType type = parse_TYPE();	
	match(ID, TYPE_DEC);	
	Token* tok = getCurr();	
	IDItem* idItem = NULL;

	if (isParams)
	{
		idItem = (IDItem*)malloc(sizeof(IDItem));
		idItem->id.name = (char*)calloc(sizeof(char), strlen(tok->lexeme) + 1);
		strcpy(idItem->id.name, tok->lexeme);
		idItem->id.list = idItem->id.idListHead = idItem->dimentions = idItem->dimentionsHead = idItem->next = NULL;
		idItem->id.hasBeenUsed = 0;		
	}
	else
	{		
		idItem = insert(symbolTable, tok);
	}

	if (idItem != NULL) {
		idItem->id.type = type;
		idItem->id.role = ROLE_VARIABLE;
	}

	parse_END_VAR(idItem);

	return idItem;
}

void parse_END_VAR(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(END_VAR);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case OPEN_BRACKETS:
		next_token();
		parse_DIM_SIZES(idItem);
		match(CLOSE_BRACKETS, END_VAR);
	default:
		break;
	}
}

eType parse_TYPE()
{
	if (reachedUnexpectedEOF) return;
	printRule(TYPE);
	Token* token = next_token();
	switch (token->kind)
	{
	case INT:
		return TYPE_INTEGER;
	case FLOAT:
		return TYPE_FLOAT;		
	default:
		parserError(TYPE);
		return TYPE_NOT_DEFINED;
	}
}

void parse_DIM_SIZES(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(DIM_SIZES);	
	match(INTEGER_NUMBER, DIM_SIZES);
	Token* tok = getCurr();
	
	if (idItem != NULL)
	{
		idItem->dimentions = (IntegerList*)malloc(sizeof(IntegerList));
		idItem->dimentions->value = atoi(tok->lexeme);
		idItem->dimentions->next = NULL;
		idItem->dimentionsHead = idItem->dimentions;
	}

	parse_ARRAY_FOLLOW(idItem);
}

void parse_ARRAY_FOLLOW(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(ARRAY_FOLLOW);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case COMMA:
		next_token();
		match(INTEGER_NUMBER, ARRAY_FOLLOW);
		Token* tok = getCurr();
		if (idItem != NULL)
		{
			idItem->dimentions->next = (IntegerList*)malloc(sizeof(IntegerList));
			idItem->dimentions = idItem->dimentions->next;
			idItem->dimentions->next = NULL;
			idItem->dimentions->value = atoi(tok->lexeme);
		}
		parse_ARRAY_FOLLOW(idItem);
		break;
	case CLOSE_BRACKETS:
		break;	
	}
}

void parse_FUNC_PREDEFS()
{
	if (reachedUnexpectedEOF) return;
	printRule(FUNC_PREDEFS);
	IDItem* item = parse_FUNC_PROTOTYPE(0);
	match(SEMICOLON, FUNC_PREDEFS);
	item->id.role = ROLE_PRE_DEFINITION;
	parse_FUNC_PREDEFS_FOLLOW();
}

void parse_FUNC_PREDEFS_FOLLOW()
{
	if (reachedUnexpectedEOF) return;
	printRule(FUNC_PREDEFS_FOLLOW);
	int entryIndex = 0;
	Token *token = next_token();
	back_token();

	switch (token->kind)
	{
	case INT:
	case FLOAT:
	case VOID:
		while (token->kind != SEMICOLON && token->kind != OPEN_CURLYBRACES && token->kind != EOF_tok) {
			entryIndex++;
			token = next_token();
		}		
		Token* back;
		for (int i = 0; i < entryIndex; i++) {
			back = back_token();
		}
		if (token->kind == SEMICOLON) {
			IDItem* item = parse_FUNC_PROTOTYPE(0);
			match(SEMICOLON, FUNC_PREDEFS_FOLLOW);
			if (item != NULL) {
				item->id.role = ROLE_PRE_DEFINITION;
			}
			parse_FUNC_PREDEFS_FOLLOW();
			break;
		}
		// the next expression is FUNC_FULL_DEFS
		else if (token->kind == OPEN_CURLYBRACES) {
			break;
		}
	}
}

IDItem* parse_FUNC_PROTOTYPE(int oneIfIsFullDef)
{
	if (reachedUnexpectedEOF) return;
	printRule(FUNC_PROTOTYPE);
	eType type = parse_RETURNED_TYPE();	
	Token* funcName = next_token();
	back_token();
	match(ID, FUNC_PROTOTYPE);
	Token* tok = getCurr();
	IDItem* idItem;	

	if (oneIfIsFullDef) {
		idItem = find(symbolTable, tok->lexeme);
		if (idItem != NULL) {
			if (idItem->id.type != type) {
				// diffrent returned types
				returnedTypeError(idItem->id.type, type, tok->lineNumber);
			}
			
			IDItem* temp = (IDItem*)malloc(sizeof(IDItem));
			temp->id.list = temp->id.idListHead = temp->dimentions = temp->dimentionsHead = temp->next = NULL;
			match(OPEN_PARENTHESES, FUNC_PROTOTYPE);
			parse_PARAMS(temp);
			match(CLOSE_PARENTHESES, FUNC_PROTOTYPE);

			if (temp != NULL) {
				temp->id.list = temp->id.idListHead;
				idItem->id.list = idItem->id.idListHead;
				int fullDefParamSize = getIDListSize(temp->id.list);
				int preDefParamSize = getIDListSize(idItem->id.list);

				if (preDefParamSize != fullDefParamSize ) {
					// different func param sizes
					idItem->id.idListHead = temp->id.idListHead;
					funcParamsSizeError(funcName->lexeme, fullDefParamSize, preDefParamSize, tok->lineNumber);
				}
				else {
					temp->id.list = temp->id.idListHead;
					idItem->id.list = idItem->id.idListHead;
					while (idItem->id.list != NULL && temp->id.list != NULL && idItem->id.list->value.id.type == temp->id.list->value.id.type)
					{
						idItem->id.list = idItem->id.list->next;
						temp->id.list = temp->id.list->next;
					}

					Token* token = getCurr();
					if (idItem->id.list != NULL && temp->id.list != NULL) {
						// different param types
						paramTypeError(idItem->id.list, temp->id.list->value.id.type, token->lineNumber);
					}
					temp->id.list = temp->id.idListHead;
					idItem->id.list = idItem->id.idListHead;
				}
			}
		}
		else {
			// full definition with predefinition
			idItem = insert(symbolTable, tok);
			idItem->id.idListHead = idItem->id.list = NULL;
			idItem->id.type = type;
			match(OPEN_PARENTHESES, FUNC_PROTOTYPE);
			parse_PARAMS(idItem);
			match(CLOSE_PARENTHESES, FUNC_PROTOTYPE);
		}
	}
	else {
		idItem = insert(symbolTable, tok);
		if (idItem != NULL) {
			idItem->id.type = type;
		}
		
		match(OPEN_PARENTHESES, FUNC_PROTOTYPE);		
		parse_PARAMS(idItem);		
		match(CLOSE_PARENTHESES, FUNC_PROTOTYPE);
	}


	

	return idItem;
}

void parse_FUNC_FULL_DEFS() 
{
	if (reachedUnexpectedEOF) return;
	printRule(FUNC_FULL_DEFS);
	parse_FUNC_WITH_BODY();
	parse_FUNC_WITH_BODY_FOLLOW();
}

void parse_FUNC_WITH_BODY_FOLLOW() {
	printRule(FUNC_WITH_BODY_FOLLOW);
	if (reachedUnexpectedEOF) return;
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case EOF_tok:	
		break;
	default:
		parse_FUNC_WITH_BODY();
		parse_FUNC_WITH_BODY_FOLLOW();				
		break;
	}
}

void parse_FUNC_WITH_BODY() 
{
	if (reachedUnexpectedEOF) return;
	printRule(FUNC_WITH_BODY);

	IDItem* item = parse_FUNC_PROTOTYPE(1);	

	item->id.role = ROLE_FULL_DEFINITION;	

	// this is made to mark main function as used 	
	if (strcmp(item->id.name, "main") == 0) {
		find(symbolTable, item->id.name);
	}
	
	parse_BEGIN_BLOCK();
	item->id.list = item->id.idListHead;
	Token token;
	token.lexeme = (char*)malloc(sizeof(char) * 1000);
	IDItem* tempItem;
	
	while (item->id.list != NULL && item->id.type != TYPE_NOT_DEFINED) {
		token.lexeme = item->id.list->value.id.name;
		if (find(symbolTable, token.lexeme) == NULL) {
			tempItem = insert(symbolTable, &token);
			tempItem->id.type = item->id.list->value.id.type;
		}
		item->id.list = item->id.list->next;
	}

	parse_COMP_STMT();
	int errorLine = getCurr()->lineNumber;
	int isMain = !strcmp(item->id.name, "main");

	if (((item->id.type != TYPE_VOID && returnedType != TYPE_VOID) || (item->id.type != TYPE_VOID && returnedType == TYPE_VOID) || (item->id.type == TYPE_VOID && returnedType != TYPE_VOID)) && !isMain) {
		if (item->id.type != returnedType) {
			if (returnedType == TYPE_NOT_DEFINED) {
				fprintf(semanticFile, "Error: Function {'%s'} doesn't return anything, line: {'%d'} \n", item->id.name,errorLine);
			}
			else {
				if (returnedType == TYPE_INTEGER && item->id.type == TYPE_FLOAT) {
					fprintf(semanticFile, "Error: Function {'%s'} return type is {'INTEGER'} instead {'FLOAT'}, line: {'%d'} \n",item->id.name, errorLine);
				}				
				else if (returnedType == TYPE_FLOAT && item->id.type == TYPE_INTEGER) {
					fprintf(semanticFile, "Error: Function {'%s'} return type is {'FLOAT'} instead {'INTEGER'}, line: {'%d'} \n", item->id.name, errorLine);
				}
				else if (returnedType == TYPE_INTEGER && item->id.type == TYPE_VOID) {
					fprintf(semanticFile, "Error: Function {'%s'} return type is {'INTEGER'} instead {'VOID'}, line: {'%d'} \n", item->id.name, errorLine);
				}
				else if (returnedType == TYPE_FLOAT && item->id.type == TYPE_VOID) {
					fprintf(semanticFile, "Error: Function {'%s'} return type is {'FLOAT'} instead {'VOID'}, line: {'%d'} \n", item->id.name, errorLine);
				}
				else {
					fprintf(semanticFile, "Error: Function {'%s'} doesn't return anything, line: {'%d'} \n", item->id.name, errorLine);
				}
			}
		}
	}
	else if (isMain) {
		if (returnedType == TYPE_INTEGER) {
			fprintf(semanticFile, "Error: Function {'%s'} return type is {'INTEGER'} instead {'VOID'}, line: {'%d'} \n", item->id.name, errorLine);
		}
		else if (returnedType == TYPE_FLOAT) {
			fprintf(semanticFile, "Error: Function {'%s'} return type is {'FLOAT'} instead {'VOID'}, line: {'%d'} \n", item->id.name, errorLine);
		}
	}
	returnedType = TYPE_NOT_DEFINED;

	parse_END_BLOCK(1);
}

eType parse_RETURNED_TYPE() {
	if (reachedUnexpectedEOF) return;
	printRule(RETURNED_TYPE);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case VOID:
		next_token();
		return TYPE_VOID;
	default:
		return parse_TYPE();		
	}
}

void parse_PARAMS(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(PARAMS);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case CLOSE_PARENTHESES:
		break;
	default:
		parse_PARAM_LIST(idItem);
		break;
	}
}

void parse_PARAM_LIST(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(PARAM_LIST);
	if (idItem != NULL) {
		idItem->id.list = (IDList*)malloc(sizeof(IDList));
		idItem->id.idListHead = idItem->id.list;
		idItem->id.list->next = NULL;
	}
	parse_PARAM(idItem);
	parse_PARAM_FOLLOW(idItem);
}

void parse_PARAM_FOLLOW(IDItem* idItem) {
	if (reachedUnexpectedEOF) return;
	printRule(PARAM_FOLLOW);
	Token* token = next_token();
	back_token();
	
	switch (token->kind)
	{
	case CLOSE_PARENTHESES:
		break;
	case COMMA:
		next_token();
		if (idItem != NULL) {
			if (idItem->id.list->next == NULL) {
				idItem->id.list->next = (IDList*)malloc(sizeof(IDList));
				idItem->id.list = idItem->id.list->next;
				idItem->id.list->next = NULL;
			}
			else {
				idItem->id.list = idItem->id.list->next;
			}
		}
		parse_PARAM(idItem);
		parse_PARAM_FOLLOW(idItem);
		break;
	}
}

void parse_PARAM(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(PARAM);
	
	if (idItem != NULL && idItem->id.type != TYPE_NOT_DEFINED) {
		idItem->id.list->value = *parse_TYPE_DEC(1);
		char* paramName = idItem->id.list->value.id.name;

		IDList* curr = idItem->id.idListHead;
		int count = 0;
		while (curr->next != NULL) {
			if (strcmp(paramName, curr->value.id.name) == 0) {
				count++;
			}
			curr = curr->next;
		}

		if (count != 0) {
			// douplicate param name
			Token temp;
			temp.lexeme = (char*)calloc(1000, sizeof(char));
			strcpy(temp.lexeme, paramName);
			temp.lineNumber = getCurr()->lineNumber;
			douplicateParamNameError(&temp);
		}
	}
	else {
		parse_TYPE_DEC(0);
	}
}

void parse_COMP_STMT()
{
	if (reachedUnexpectedEOF) return;
	printRule(COMP_STMT);
	match(OPEN_CURLYBRACES, COMP_STMT);
	parse_BEGIN_BLOCK();
	parse_VAR_DEC_LIST();
	parse_STMT_LIST();
	match(CLOSE_CURLYBRACES, COMP_STMT);
	parse_END_BLOCK(1);
}

void parse_VAR_DEC_LIST() {
	if (reachedUnexpectedEOF) return;
	printRule(VAR_DEC_LIST);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case ID:
	case OPEN_CURLYBRACES:
	case IF:
	case RETURN:
		break;
	default:
		parse_VAR_DEC();
		parse_VAR_DEC_LIST();
		break;
	}	
}

void parse_STMT_LIST()
{
	if (reachedUnexpectedEOF) return;
	printRule(STMT_LIST);
	parse_STMT();
	parse_STMT_LIST_FOLLOW();
}

void parse_STMT_LIST_FOLLOW() {
	if (reachedUnexpectedEOF) return;
	printRule(STMT_LIST_FOLLOW);
	Token* token = next_token();
	Token* back = back_token();

	switch (token->kind)
	{
	case CLOSE_CURLYBRACES:
		break;
	case SEMICOLON:
		next_token();
		parse_STMT();
		parse_STMT_LIST_FOLLOW();
		break;
	}	
}

void parse_STMT() {
	if (reachedUnexpectedEOF) return;
	printRule(STMT);
	Token* token = next_token();
	IDItem* item;
	
	switch (token->kind)
	{
	case ID:
		item = find(symbolTable, token->lexeme);
		if (item != NULL) {
			item->id.hasBeenUsed = 1;
		}
		else {
			unidentifiedError(token);
		}
		parse_VAR_WITH_EXPR_OR_CALL_FOLLOW(item);
		break;
	case OPEN_CURLYBRACES:
		back_token();
		parse_COMP_STMT();
		break;
	case IF:
		back_token();
		parse_IF_STMT();
		break;
	case RETURN:
		back_token();
		parse_RETURN_STMT();
		break;
	case CLOSE_CURLYBRACES:
	case SEMICOLON:
		back_token();
		break;
	default:
		parserError(STMT);
		break;
	}	
}

void parse_IF_STMT() {
	if (reachedUnexpectedEOF) return;
	printRule(IF_STMT);
	match(IF, IF_STMT);
	match(OPEN_PARENTHESES, IF_STMT);
	parse_CONDITION();
	match(CLOSE_PARENTHESES, IF_STMT);
	parse_STMT();
}

//void parse_CALL()
//{
//	if (reachedUnexpectedEOF) return;
//	printRule(CALL);
//	match(ID, CALL);
//	match(OPEN_PARENTHESES, CALL);
//	parse_ARGS();
//	match(CLOSE_PARENTHESES, CALL);
//}

TypeList* parse_ARGS()
{
	if (reachedUnexpectedEOF) return;
	printRule(ARGS);
	Token* token = next_token();
	back_token();
	TypeList* typeList = (TypeList*)malloc(sizeof(TypeList));
	TypeList* typeListHead = typeList;
	typeListHead->next = NULL;

	switch (token->kind)
	{
	case CLOSE_PARENTHESES:
		return NULL;
	default:
		parse_ARG_LIST(typeList);
		
		return typeListHead;
	}
}

void parse_ARG_LIST(TypeList* typeList)
{
	if (reachedUnexpectedEOF) return;
	printRule(ARG_LIST);
	eType type = parse_EXPR();	
	typeList->value = type;
	typeList->next = NULL;
	parse_ARG_LIST_FOLLOW(typeList);
}

void parse_ARG_LIST_FOLLOW(TypeList* typeList)
{
	if (reachedUnexpectedEOF) return;
	printRule(ARG_LIST_FOLLOW);	
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case COMMA:
		next_token();
		eType type = parse_EXPR();

		if (typeList->next == NULL)
		{
			typeList->next = (TypeList*)malloc(sizeof(TypeList));
		}
		typeList = typeList->next;

		typeList->value = type;
		typeList->next = NULL;
		parse_ARG_LIST_FOLLOW(typeList);
		break;
	default:		
		break;
	}
}

void parse_RETURN_STMT()
{
	if (reachedUnexpectedEOF) return;
	printRule(RETURN_STMT);
	match(RETURN, RETURN_STMT);
	parse_RETURN_STMT_FOLLOW();
}

void parse_RETURN_STMT_FOLLOW()
{
	if (reachedUnexpectedEOF) return;
	printRule(RETURN_STMT_FOLLOW);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case SEMICOLON:
	case CLOSE_CURLYBRACES:
		returnedType = TYPE_VOID;
		break;
	default:
		returnedType = parse_EXPR();
		break;
	}
}

//void parse_VAR()
//{
//	if (reachedUnexpectedEOF) return;
//	printRule(VAR);
//	match(ID, VAR);
//	parse_VAR_FOLLOW();
//}

void parse_VAR_FOLLOW(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(VAR_FOLLOW);
	Token* token = next_token();
	TypeList* types = NULL;
	TypeList* typesHead = NULL;
	back_token();

	switch (token->kind)
	{
	case OPEN_BRACKETS:
		next_token();
		types = (TypeList*)malloc(sizeof(TypeList));
		typesHead = types;
		types->next = NULL;
		if (idItem != NULL) {
			if (idItem->dimentionsHead != NULL) {
				idItem->dimentions = idItem->dimentionsHead;
			}
			else {
				// not array
				notArrayError(back_token());
				next_token();
			}
		}
		else {
			// unidentified id
			unidentifiedError(back_token());
			next_token();
		}

		if (idItem != NULL) {
			idItem->dimentions = idItem->dimentionsHead;
		}
		parse_EXPR_LIST(idItem);

		match(CLOSE_BRACKETS, VAR_FOLLOW);		

	case CLOSE_PARENTHESES:
	case PLUS_OPERATION:
	case COMMA:
	case SEMICOLON:
	case CLOSE_CURLYBRACES:
	case CLOSE_BRACKETS:
	case MULIPLICATION_OPERATION:
	case GREATER:
	case GREATER_EQUAL:
	case LOWER:
	case LOWER_EQUAL:
	case EQUAL:
	case NOT_EQUAL:
		break;
	case PLACING_OPERATION:
		if (idItem != NULL) {
			if (idItem->id.role == ROLE_VARIABLE && idItem->dimentionsHead != NULL) {
				// assignments into array 
				illigalAssignmentError(getCurr());
			}
			else if (idItem->id.role != ROLE_VARIABLE) {
				// assignments into func or unknown
				illigalAssignmentError(getCurr());
			}
		}
		break;
	}
}

void parse_EXPR_LIST(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(EXPR_LIST);	
	
	eType type = parse_EXPR();

	if (idItem != NULL && idItem->dimentions != NULL) {
		if (type == TYPE_INTEGER)
		{
			if (idItem->dimentions->value <= currDimentionSize || currDimentionSize < 0) {
				// of over or under flow
				fprintf(semanticFile, "Error: Out of bounds, line: {'%d'} \n", getCurr()->lineNumber);
			}
		}
		else {
			// type
			fprintf(semanticFile, "Error: Expression must include only integer numbers, line: {'%d'} \n", getCurr()->lineNumber);
		}
	}
	else if (idItem != NULL && idItem->dimentionsHead != NULL && idItem->dimentions == NULL && currDimentionSize > 0) {
		// Array dimentions is actually lower
		int arrayDim = getArraySize(idItem->dimentionsHead);
		if (idItem->id.name != NULL) {
			fprintf(semanticFile, "Error: Diffrent array dimensional error, {'%s') is %dD array, line: {'%d'} \n", idItem->id.name, arrayDim, getCurr()->lineNumber);
		}
		else {
			fprintf(semanticFile, "Error: Diffrent array dimensional error, Array dimensional is actually lower, line: {'%d'} \n", idItem->id.name, arrayDim, getCurr()->lineNumber);
		}
	}

	
	currDimentionSize = -1;
	if (idItem != NULL && idItem->dimentions != NULL) {
		idItem->dimentions = idItem->dimentions->next;
	}
	parse_EXPR_LIST_FOLLOW(idItem);
}

void parse_EXPR_LIST_FOLLOW(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(EXPR_LIST_FOLLOW);
	Token* token = next_token();
	back_token();

	switch (token->kind)
	{
	case COMMA:
		next_token();		
		parse_EXPR_LIST(idItem);
		break;
	case CLOSE_BRACKETS:
		break;
	}
}

void parse_CONDITION() {
	if (reachedUnexpectedEOF) return;
	printRule(CONDITION);
	Token* token;
	
	parse_EXPR();
	token = next_token();

	switch (token->kind)
	{
	case GREATER:
	case GREATER_EQUAL:
	case LOWER:
	case LOWER_EQUAL:
	case EQUAL:
	case NOT_EQUAL:
		parse_EXPR();
		break;
	default:
		// firsts error will come from EXPR therefor the Condition firsts array will include real_op only
		parserError(CONDITION);
		break;
	}		
}

eType parse_EXPR()
{
	if (reachedUnexpectedEOF) return;
	printRule(EXPR);

	if (getCurr()->lineNumber == 151) {
		printf("fuck");
	}

	eType termType = parse_TERM();
	eType EXPR_FOLLOW_Type = parse_EXPR_FOLLOW();
	eType typeToReturn;

	if (EXPR_FOLLOW_Type == TYPE_NOT_DEFINED || EXPR_FOLLOW_Type == termType) {
		typeToReturn = termType;
	}	
	else {
		typeToReturn = TYPE_FLOAT;
	}
	
	return typeToReturn;
}

eType parse_EXPR_FOLLOW()
{
	if (reachedUnexpectedEOF) return;
	printRule(EXPR_FOLLOW);
	Token *tmpToken, *token = next_token();
	back_token();
	eType typeToReturn = TYPE_NOT_DEFINED;
	int tempVal;

	switch (token->kind)
	{
	case PLUS_OPERATION:

		//check if it's from the follow or from the first of TERM
		next_token();
		tmpToken = next_token();
		back_token();
		back_token();
		
		switch (tmpToken->kind)
		{
		case ID:			
		case INTEGER_NUMBER:
		case FLOAT_NUMBER:			
		case OPEN_PARENTHESES:
			next_token();
			tempVal = currDimentionSize;
			eType termType = parse_TERM();
			if (termType == TYPE_INTEGER) {
				currDimentionSize += tempVal;
			}

			eType EXPR_FOLLOW_Type = parse_EXPR_FOLLOW();

			if (EXPR_FOLLOW_Type == TYPE_NOT_DEFINED || EXPR_FOLLOW_Type == termType) {
				typeToReturn = termType;
			}
			else {
				typeToReturn = TYPE_FLOAT;
			}

			break;

		default:
			// if not from the first of TERM it's from the folllow
			break;
		}
		break;

	case COMMA:
	case CLOSE_PARENTHESES:
	case SEMICOLON:
	case CLOSE_BRACKETS:
	case CLOSE_CURLYBRACES:
	case GREATER:
	case GREATER_EQUAL:
	case LOWER:
	case LOWER_EQUAL:
	case EQUAL:
	case NOT_EQUAL:
		break;
	}

	return typeToReturn;
}

eType parse_TERM()
{
	if (reachedUnexpectedEOF) return;
	printRule(TERM);


	eType factorType = parse_FACTOR();
	eType termFollowType = parse_TERM_FOLLOW();
	eType typeToReturn = TYPE_NOT_DEFINED;

	if (termFollowType == TYPE_NOT_DEFINED || factorType == termFollowType) {
		typeToReturn = factorType;
	}
	else if (factorType != TYPE_NOT_DEFINED) {
		typeToReturn = TYPE_FLOAT;
	}
	
	return typeToReturn;

}

eType parse_TERM_FOLLOW()
{
	if (reachedUnexpectedEOF) return;
	printRule(TERM_FOLLOW);
	Token *token = next_token();
	back_token();
	eType typeToReturn = TYPE_NOT_DEFINED;
	int tempVal;

	switch (token->kind)
	{
	case MULIPLICATION_OPERATION:
		next_token();
		tempVal = currDimentionSize;
		eType factorType = parse_FACTOR();
		if (factorType == TYPE_INTEGER) {
			currDimentionSize *= tempVal;
		}
		else {
			currDimentionSize = -1;
		}

		eType termFollowType = parse_TERM_FOLLOW();

		if (termFollowType == TYPE_NOT_DEFINED) {
			typeToReturn = factorType;
		}
		else if(factorType != TYPE_NOT_DEFINED){
			typeToReturn = TYPE_FLOAT;
		}

		break;

	case CLOSE_PARENTHESES:
	case PLUS_OPERATION:
	case COMMA:
	case SEMICOLON:
	case CLOSE_BRACKETS:
	case CLOSE_CURLYBRACES:
	case GREATER:
	case GREATER_EQUAL:
	case LOWER:
	case LOWER_EQUAL:
	case EQUAL:
	case NOT_EQUAL:
		break;
	}

	return typeToReturn;
}

eType parse_FACTOR()
{
	if (reachedUnexpectedEOF) return;
	printRule(FACTOR);
	eType typeToReturn = TYPE_NOT_DEFINED;
	Token* token = next_token();
	IDItem* item = NULL;

	switch (token->kind)
	{
	case ID:
		item = find(symbolTable, token->lexeme);
		parse_VAR_OR_CALL_FOLLOW(item);
		if (item != NULL) 
		{
			typeToReturn = item->id.type;
		}
		else {
			// undentified
			unidentifiedError(token);
		}
		break;
	case INTEGER_NUMBER:
		typeToReturn = TYPE_INTEGER;
		currDimentionSize = atoi(token->lexeme);
		break;
	case FLOAT_NUMBER:
		typeToReturn = TYPE_FLOAT;
		break;
	case OPEN_PARENTHESES:
		typeToReturn = parse_EXPR();
		match(CLOSE_PARENTHESES, FACTOR);
		break;
	default:
		parserError(FACTOR);
		break;
	}

	return typeToReturn;
}

void parse_VAR_OR_CALL_FOLLOW(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(VAR_OR_CALL_FOLLOW);
	Token* funcNameTok = getCurr();
	Token* token = next_token();
	TypeList* types = NULL;
	
	switch (token->kind)
	{
	case OPEN_PARENTHESES:
		types = parse_ARGS();
		match(CLOSE_PARENTHESES, VAR_OR_CALL_FOLLOW);

		if (idItem != NULL) {
			IDList* listTemp = idItem->id.idListHead;

			while (types != NULL && listTemp != NULL && types->value != TYPE_NOT_DEFINED && listTemp->value.id.type != TYPE_NOT_DEFINED && types->value == listTemp->value.id.type) {
				types = types->next;
				listTemp = listTemp->next;
			}
			if ((types == NULL && listTemp != NULL) || (types != NULL && listTemp == NULL)) {
				// TODO diffrent size error				
				fprintf(semanticFile, "Error: Number of args is not currect, line: {'%d'} \n", getCurr()->lineNumber);
			}
			else if (types != NULL || listTemp != NULL) {				
				// TODO types error
				fprintf(semanticFile, "Error: Diffrent args types, line: {'%d'} \n", getCurr()->lineNumber);				
			}
		}
		
		break;
	default:
		back_token();
		parse_VAR_FOLLOW(idItem);
		break;
	}
}

void parse_VAR_WITH_EXPR_OR_CALL_FOLLOW(IDItem* idItem)
{
	if (reachedUnexpectedEOF) return;
	printRule(VAR_WITH_EXPR_OR_CALL_FOLLOW);
	Token* token = next_token();
	eType exprType;
	TypeList* funcTypes;	
	

	switch (token->kind)
	{
	case OPEN_PARENTHESES:
		funcTypes = parse_ARGS();	
		match(CLOSE_PARENTHESES, VAR_WITH_EXPR_OR_CALL_FOLLOW);
		if (idItem != NULL && funcTypes != NULL) {
			if (idItem->id.role == ROLE_FULL_DEFINITION || idItem->id.role == ROLE_PRE_DEFINITION)
			{
				IDList* tempList = idItem->id.list = idItem->id.idListHead;
				TypeList* funcTypesHead = funcTypes;

				while ((tempList != NULL && funcTypes != NULL) && (tempList->value.id.type == funcTypes->value || (tempList->value.id.type == TYPE_FLOAT && funcTypes->value == TYPE_INTEGER))) {
					tempList = tempList->next;
					funcTypes = funcTypes->next;
				}
				if ((tempList != NULL && funcTypes == NULL) || (tempList == NULL && funcTypes != NULL)){
					// different sizes
					fprintf(semanticFile, "Error: Number of args is not currect, line: {'%d'} \n", getCurr()->lineNumber);
				}
				else if (tempList != NULL || funcTypes != NULL) {
					// different types
					fprintf(semanticFile, "Error: Diffrent args types, line: {'%d'} \n", getCurr()->lineNumber);
				}
				funcTypes = funcTypesHead;
				idItem->id.list = idItem->id.idListHead;
			}			
			else if(symbolTable->father == NULL) {
				// not function			
				fprintf(semanticFile, "Error: Call to undefined function, line: {'%d'} \n", getCurr()->lineNumber);
			}
		}		
		break;
	default:
		back_token();
		parse_VAR_FOLLOW(idItem);
		match(PLACING_OPERATION, VAR_WITH_EXPR_OR_CALL_FOLLOW);		
		exprType = parse_EXPR();
		
		if (idItem != NULL && idItem->id.type != TYPE_VOID && exprType != TYPE_NOT_DEFINED) {
			if (idItem->id.type == TYPE_INTEGER && exprType == TYPE_FLOAT) {
				// diffrent params				
				fprintf(semanticFile, "Error: Diffrent param types, line: {'%d'} \n", getCurr()->lineNumber);
			}
		}
		break;
	}
}


void addErrorParseMessage(int firstIndex, Token actualToken) {
	char errorMsg[1000] = "Expected token of type '{";
	char* errorMsgToStore;
	int len;
	int i = 0;
	int toPrint = firsts[firstIndex][i];
	char snum[100];
	char* str;

	while (toPrint != NULL) {
		str = eTokensString23[toPrint];
		strcat(errorMsg, str);
		strcat(errorMsg, ", ");
		toPrint = firsts[firstIndex][++i];
	}

	len = strlen(errorMsg);
	errorMsg[len - 2] = '\0';
	strcat(errorMsg, "}' at line: {");
	sprintf(snum, "%d", actualToken.lineNumber);
	strcat(errorMsg, snum);
	strcat(errorMsg, "},\nActual token of type '{");
	str = eTokensString23[actualToken.kind];
	strcat(errorMsg, str);
	strcat(errorMsg, "}', lexeme : '{");
	str = actualToken.lexeme;
	strcat(errorMsg, str);
	strcat(errorMsg, "}'.\n");

	errorMsgToStore = (char*)malloc(sizeof(char) * (strlen(errorMsg) + 1));
	strcpy(errorMsgToStore, errorMsg);
	currentErrorNode->erroeMsg = errorMsgToStore;
	fprintf(parserFile, errorMsgToStore);
}

void addErrorMatchMessage(eTOKENS expectedTokenKind, Token actualToken) {
	char errorMsg[1000] = "Expected token of type '{";
	char* errorMsgToStore;
	char* str = eTokensString23[expectedTokenKind];
	char snum[100];

	strcat(errorMsg, str);
	strcat(errorMsg,"}' at line : {");	
	sprintf(snum, "%d", actualToken.lineNumber);
	strcat(errorMsg, snum);
	strcat(errorMsg, "},\nActual token of type '{");
	str = eTokensString23[actualToken.kind];
	strcat(errorMsg, str);
	strcat(errorMsg,"}', lexeme : '{");
	str = actualToken.lexeme;
	strcat(errorMsg, str);
	strcat(errorMsg, "}'.\n");

	errorMsgToStore = (char*)malloc(sizeof(char)* (strlen(errorMsg)+1));
	strcpy(errorMsgToStore, errorMsg);
	currentErrorNode->erroeMsg = errorMsgToStore;
	fprintf(parserFile, errorMsgToStore);
}

void printAllErrorsToScreenForDevalopmentUse() {
	if (currentErrorNode != NULL) {
		while (currentErrorNode->prev != NULL) currentErrorNode = currentErrorNode->prev;
		while (currentErrorNode != NULL) {
			char* str = currentErrorNode->erroeMsg;
			printf(str);
			currentErrorNode = currentErrorNode->next;
		}
	}
}

void goToFollowToken(eParseFunction parseFunction) {
	int i;
	eTOKENS tokToCheck;
	Token* currentToken;
	int followTokFound = 0;

	currentToken = back_token();
	currentToken = next_token();	

	i = 0;
	tokToCheck = follows[parseFunction][i];
	while ((followTokFound != 1) && (currentToken->kind != EOF_tok) && (tokToCheck != NULL)) {		
		do {
			followTokFound = (currentToken->kind != EOF_tok) && (tokToCheck == currentToken->kind);
			tokToCheck = follows[parseFunction][++i];
		} while ((tokToCheck != NULL) && (followTokFound != 1));

		if (currentToken->kind != EOF_tok && followTokFound != 1) currentToken = next_token();
	}	

	if (followTokFound) back_token();
	else if (currentToken->kind == EOF_tok) {		
		reachedUnexpectedEOF = 1;
	}
}

void addSemanticErrorMsg(char* errorMsg) {
	fprintf(semanticFile, errorMsg);
}





