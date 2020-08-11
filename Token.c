#pragma once 
#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
Node* currentNode = NULL;
int startReadingAfterLexAnalyzeError = 0;
int justOpened = 1;

//#define TOKEN_ARRAY_SIZE 1000
#define TOKEN_ARRAY_SIZE 3

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	int length = strlen(lexeme)+1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = 
			(Token*) calloc(sizeof(Token),TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = 
			 (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;	
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
	#ifdef _WIN32
		strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
	#else
		strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token() {	
	Token* tokenToReturn;

	if (currentNode == NULL ) {
		tokenToReturn = NULL;
	}	
	else if (currentIndex % TOKEN_ARRAY_SIZE != 0) {
		tokenToReturn = &currentNode->tokensArray[(--currentIndex) % TOKEN_ARRAY_SIZE];
	}
	else if (currentIndex % TOKEN_ARRAY_SIZE == 0 && currentNode->prev != NULL) {
		currentNode = currentNode->prev;
		currentIndex--;
		if (currentIndex < 0)currentIndex = TOKEN_ARRAY_SIZE - 1;
		tokenToReturn = &currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE];
	}
	else {
		tokenToReturn = NULL;
	}

	return tokenToReturn;
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token *next_token() 
{
	Token *tokenToReturn = NULL;
	int nextIndex = (currentIndex + 1) % TOKEN_ARRAY_SIZE;
	if (justOpened) {
		justOpened = 0;
		startReadingAfterLexAnalyzeError = 0;
		nextIndex = 0;
	}

	if (currentNode == NULL) {
		if (yylex() != 0) {}
		//tokenToReturn = &currentNode->tokensArray[currentIndex];
		// if we had a lexical error the currentNode will be NULL
		if (currentNode != NULL) tokenToReturn = &currentNode->tokensArray[currentIndex];
		else {
			tokenToReturn = (Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
			tokenToReturn->kind = -1;
		}
	}	
	else if (nextIndex != 0 && (&currentNode->tokensArray[nextIndex])->lexeme != NULL) {				
		currentIndex = nextIndex;
		tokenToReturn = &currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE];
	}	
	else if (currentNode->next != NULL && nextIndex == 0) {
		currentNode = currentNode->next;
		currentIndex = nextIndex;
		tokenToReturn = &currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE];
	}
	else {
		yylex();
		tokenToReturn = &currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE];
	}	
	
	return tokenToReturn;
}

void stopParsingBecauseLexErrorContinueOnlyLexicalAnalysis() {
	startReadingAfterLexAnalyzeError = 1;
}

void parserError(eParseFunction funcName)
{
	if (currentNode != NULL) {
		create_new_error_node();
		addErrorParseMessage(funcName, currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE]);
		goToFollowToken(funcName);
	}
}

void matchError(eTOKENS expectedTokenKind, eParseFunction funcName)
{
	if (currentNode != NULL) {
		Token currTok = currentNode->tokensArray[currentIndex % TOKEN_ARRAY_SIZE];
		if (expectedTokenKind != EOF_tok && currTok.kind == EOF_tok && currTok.lexeme != NULL) return;
		create_new_error_node();
		addErrorMatchMessage(expectedTokenKind, currTok);
		// Mark said if we got a mach error there is no need in error recovery
		// http://md.hit.ac.il/mod/forum/discuss.php?d=69460#p103203
		//goToFollowToken(funcName);
	}
}

void match(eTOKENS expectedTokenKind, eParseFunction funcName) {
	if (startReadingAfterLexAnalyzeError == 0) {
		Token* currToken = next_token();
		if (currToken->kind != expectedTokenKind) matchError(expectedTokenKind, funcName);
	}	
}

void goBackToFirstNode() {
	while (currentNode != NULL && currentNode->prev != NULL) currentNode = currentNode->prev;
	currentIndex = 0;
}

void printAllTokens() {
	int i = 0;
	Token currToken;

	printf("\n\n%%%%%%%%%%%% print all tokens %%%%%%%%%%%%\n\n");
	goBackToFirstNode();
	currToken = currentNode->tokensArray[0];

	while (currToken.kind != EOF_tok && currentNode != NULL) {
		currToken = currentNode->tokensArray[i % TOKEN_ARRAY_SIZE];
		printf("token %d is: %d lexeme is: %s \n", i, currToken.kind, currToken.lexeme);
		i++;
		if (i % TOKEN_ARRAY_SIZE == 0 && currentNode->next != NULL) currentNode = currentNode->next;
	}
}

void nextAndBackTokenTest() {
	Token* currentToken;

	goBackToFirstNode();
	currentToken = next_token();
	currentToken = back_token();
	currentToken->lexeme = "first";
	currentToken = next_token();
	currentToken->lexeme = "second";
	currentToken = next_token();
	currentToken->lexeme = "third";
	printAllTokens();
	goBackToFirstNode();
	currentToken = next_token();
	currentToken = next_token();
	currentToken = back_token();
	currentToken->lexeme = "2";
	currentToken = back_token();
	currentToken->lexeme = "1";
	printAllTokens();
	goBackToFirstNode();
	currentToken = next_token();
	currentToken = next_token();
	currentToken = next_token();
	currentToken = next_token();
	currentToken = next_token();
	currentToken->lexeme = "token No 5";
	currentToken = back_token();
	currentToken->lexeme = "token No 4";
	currentToken = next_token();
	currentToken = next_token();
	currentToken->lexeme = "token No 6 ";
	printAllTokens();
}

void newFileOpend() {
	currentNode = NULL;
	justOpened = 1;
}