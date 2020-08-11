#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* semanticFile;

char* eTokensString23[] =
{
	"EMPTY",
	"ID",
	"INTEGER_NUMBER",
	"FLOAT_NUMBER",
	"IF",
	"WHILE",
	"VOID",
	"RETURN",
	"INT",
	"FLOAT",
	"COMMA",
	"COLON",
	"SEMICOLON",
	"OPEN_PARENTHESES",
	"CLOSE_PARENTHESES",
	"OPEN_BRACKETS",
	"CLOSE_BRACKETS",
	"OPEN_CURLYBRACES",
	"CLOSE_CURLYBRACES",
	"PLUS_OPERATION",
	"MULIPLICATION_OPERATION",
	"GREATER",
	"GREATER_EQUAL",
	"LOWER",
	"LOWER_EQUAL",
	"EQUAL",
	"NOT_EQUAL",
	"PLACING_OPERATION",
	"EOF_tok"
};

char* eTypeString[] =
{
	"EMPTY",
	"TYPE_INTEGER",
	"TYPE_FLOAT",
	"TYPE_VOID",
	"TYPE_NOT_DEFINED"
};

typedef enum eType
{	
	TYPE_INTEGER = 1,
	TYPE_FLOAT,
	TYPE_VOID,
	//TYPE_ARRAY
	TYPE_NOT_DEFINED
}eType;

typedef enum eRole
{
	ROLE_VARIABLE = 1,
	ROLE_PRE_DEFINITION,
	ROLE_FULL_DEFINITION,
	ROLE_NOT_DEFINED
}eRole;

typedef struct TableID TableID;
typedef struct IDItem IDItem;

typedef struct IntegerList
{
	int value;
	struct IntegerList* next;
} IntegerList;



typedef struct TypeList
{
	eType value;
	struct TypeList* next;
} TypeList;

typedef struct TableID
{
	char* name;
	eRole role;
	eType type;
	struct IDList* list;
	struct IDList* idListHead;
	int hasBeenUsed;
} TableID;

typedef struct IDItem
{
	TableID id;
	struct IDItem* next;
	struct IntegerList* dimentions;
	struct IntegerList* dimentionsHead;
} IDItem;

typedef struct IDList
{
	struct IDItem value;
	struct IDList* next;
} IDList;

typedef struct SymbolTable
{
	struct SymbolTable* father;
	struct IDItem* idList;
} SymbolTable;

SymbolTable* currTable = NULL;

//error id name is already exists
void idAlreadyExistsError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: Duplicated declaration of the same name within same scope is forbidden for: {'";
	strcat(msg, tok->lexeme);
	strcat(msg, "'} Token Kind: {'");
	strcat(msg, eTokensString23[tok->kind]);
	strcat(msg, "'} on line: {");	
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");	
	fprintf(semanticFile, msg);
}

//error id name is already exists
void idFunctionPredefinitionAlreadyExistsError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: Function predefinition declaration of the same name within same scope is forbidden for: {'";	
	strcat(msg, tok->lexeme);
	strcat(msg, "'} Token Kind: {'");
	strcat(msg, eTokensString23[tok->kind]);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

// error diffrent returned types
void returnedTypeError(eType expectedType, eType actualType, int lineNumber) {
	char snum[100];
	char msg[1000] = "Error: Returned type is: {'";
	strcat(msg, eTypeString[actualType]);
	strcat(msg, "'} insted: {'");
	strcat(msg, eTypeString[expectedType]);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void paramTypeError(IDItem* param, eType actualType, int lineNumber) {
	char snum[100];
	char msg[1000] = "Error: Param {'";
	strcat(msg, param->id.name);
	strcat(msg, "'} type is : {'");
	strcat(msg, eTypeString[actualType]);
	strcat(msg, "'} insted: {'");
	strcat(msg, eTypeString[param->id.type]);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void funcParamsSizeError(char* funcName, int paramSize, int actualSize, int lineNumber) {
	char snum[100];
	char msg[1000] = "Error: Function call {'";
	strcat(msg, funcName);
	strcat(msg, "'} with: {'");
	sprintf(snum, "%d", paramSize);
	strcat(msg, snum);
	strcat(msg, "'} params insted: {'");
	sprintf(snum, "%d", actualSize);
	strcat(msg, snum);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void douplicateParamNameError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: Param name {'";
	strcat(msg, tok->lexeme);
	strcat(msg, "'} can't be use for diffrent params ");
	strcat(msg, "on line: {");
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void notArrayError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: {'";
	strcat(msg, tok->lexeme);
	strcat(msg, "'} is not an array ");
	strcat(msg, "on line: {");
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void unidentifiedError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: Missing variable declaration for {'";
	strcat(msg, tok->lexeme);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

void illigalAssignmentError(Token* tok) {
	char snum[100];
	char msg[1000] = "Error: Illigal assignments {'";
	strcat(msg, tok->lexeme);
	strcat(msg, "'} on line: {");
	sprintf(snum, "%d", tok->lineNumber);
	strcat(msg, snum);
	strcat(msg, "}. \n");
	fprintf(semanticFile, msg);
}

IDItem* lookup(SymbolTable* current_table, char* id_name)
{
	/* a series of local searches */
	SymbolTable* tab = current_table;

	if(current_table != NULL)
	{
		IDItem* id_entry = current_table->idList;

		while (id_entry != NULL)
		{
			if (strcmp(id_name, id_entry->id.name) == 0)
				return (id_entry);
			else
				id_entry = id_entry->next;
		}
	}
	
	return NULL;
}

IDItem* find(SymbolTable* current_table, char* id_name)
{
	/* hierarchical search is implemented as a series of local searches */
	SymbolTable* tab = current_table;
	IDItem* id_entry;
	
	while (tab != NULL)
	{
		id_entry = lookup(tab, id_name);
		if (id_entry != NULL) {			
			id_entry->id.hasBeenUsed = 1;
			return (id_entry);
		}
		else
			tab = tab->father;
	}
	//printf(“ERROR: undeclared identifier % s \n”, id_name);
	return NULL;
}

SymbolTable* pop_table(SymbolTable* current_table)
{
	// free memory
	SymbolTable* res = current_table->father;
	//free(current_table->idList);

	return res;
}

IDItem* create_new_entry(SymbolTable* current_table, char* id_name) {
	IDItem* curr = current_table->idList;
	if (curr == NULL) {
		current_table->idList = curr = (IDItem*) malloc (sizeof(IDItem));
		curr->id.list = curr->id.idListHead = curr->dimentions = curr->dimentionsHead = curr->next = NULL;

		//curr->dimentionsHead = curr->dimentions = NULL;
		//curr->next = NULL;
	}
	else 
	{
		while (curr->next != NULL)
		{
			curr = curr->next;
		}
		curr->next = (IDItem*)malloc(sizeof(IDItem));
		curr = curr->next;
		curr->id.list = curr->id.idListHead = curr->dimentions = curr->dimentionsHead = curr->next = NULL;
	}
	
	curr->id.name = (char*)calloc(sizeof(char), strlen(id_name) + 1);
	strcpy(curr->id.name ,id_name);
	curr->next = NULL;
	curr->id.hasBeenUsed = 0;

	return curr;
}

IDItem* insert(SymbolTable* current_table, Token *tok)
{
	IDItem* etr;
	etr = lookup(current_table, tok->lexeme); /* local search! */
	if (etr != NULL)
	{
		//ERROR(“Duplicated declaration of % s”, id_name); 
		if(etr->id.role == ROLE_VARIABLE || etr->id.role == ROLE_NOT_DEFINED)
		{
			idAlreadyExistsError(tok);
		}
		else {
			idFunctionPredefinitionAlreadyExistsError(tok);
		}

		return NULL;
	}
	else return (create_new_entry(current_table, tok->lexeme));
}

SymbolTable* make_table(SymbolTable* current_table)
{
	SymbolTable* tab;
	tab = (SymbolTable*)malloc(sizeof(SymbolTable));
	tab->idList = NULL;
	tab->father = current_table;	
	return tab;
}

void set_id_type(IDItem id_entry, eType id_type) {
	id_entry.id.type = id_type;
}

eType get_id_type(IDItem id_entry)
{
	return id_entry.id.type;
}

void initializeSymbolTable()
{
	currTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	currTable->father = NULL;
	currTable->idList = NULL;
	/*currTable->idList = (IDItem*)malloc(sizeof(IDItem));
	currTable->idList->id.name = "main";
	currTable->idList->id.idListHead = NULL;
	currTable->idList->id.list = NULL;*/
}