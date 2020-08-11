%option noyywrap

%{
#pragma once 
#include <stdio.h>
#include "Token.h"
#pragma warning(disable:4996)

int lineCounter=1;

char* eTokensString[] =
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
%}

DIGIT    	[0-9]
ALPHA	 	[a-zA-Z]
				
%%
 

"+"																										{ return lexTokenHandel(PLUS_OPERATION);	}
"*"																										{ return lexTokenHandel(MULIPLICATION_OPERATION); }
">"																										{ return lexTokenHandel(GREATER); }
"<" 																									{ return lexTokenHandel(LOWER); }
">=" 																									{ return lexTokenHandel(GREATER_EQUAL);}
"<=" 																									{ return lexTokenHandel(LOWER_EQUAL); }
"=" 																									{ return lexTokenHandel(PLACING_OPERATION); }	
"==" 																									{ return lexTokenHandel(EQUAL); }	
"!=" 																									{ return lexTokenHandel(NOT_EQUAL); }
"int" 																									{ return lexTokenHandel(INT); }
"float" 																								{ return lexTokenHandel(FLOAT); }
"void" 																									{ return lexTokenHandel(VOID); }
"if" 																									{ return lexTokenHandel(IF); }
"return"																								{ return lexTokenHandel(RETURN); }
";"		 																								{ return lexTokenHandel(SEMICOLON); }	
","		 																								{ return lexTokenHandel(COMMA); }
":"		 																								{ return lexTokenHandel(COLON); }
"("		 																								{ return lexTokenHandel(OPEN_PARENTHESES); }
")"		 																								{ return lexTokenHandel(CLOSE_PARENTHESES); }
"["		 																								{ return lexTokenHandel(OPEN_BRACKETS); }
"]"		 																								{ return lexTokenHandel(CLOSE_BRACKETS); }
"{"		 																								{ return lexTokenHandel(OPEN_CURLYBRACES); }
"}"		 																								{ return lexTokenHandel(CLOSE_CURLYBRACES); }
(0|[1-9]{DIGIT}*)"."({DIGIT}+("E"|"e")("+"|"-"|"")([1-9]{DIGIT}*|0))			 		 				{ return lexTokenHandel(FLOAT_NUMBER); }
0|([1-9]{DIGIT}*)		 																				{ return lexTokenHandel(INTEGER_NUMBER); }
[a-z](({ALPHA}|{DIGIT})*("_"({ALPHA}|{DIGIT})+)*) 		 												{ return lexTokenHandel(ID); }

"/*"([^*]|\*+[^*/])*\*+"/" {	
	int commetLines = 1, commetLength = strlen(yytext);
	char c;
	
	for (int i = 0 ; i < commetLength ; i++){
		c = yytext[i];		
        if (c == '\n') commetLines++;
	}
	
	if(commetLines > 1) lineCounter += commetLines-1;
	return yylex();
	}
	
"/*" {
	fprintf(yyout, "End of file reached unexpectedly - comment opened but not closed. \n");
	return 0;
	}
	
[ \t] {	
	}
	
[\n] {
	lineCounter++;	
	}
	
. {
	fprintf(yyout ,"Character '%s' in line: {%d} does not begin any legal token in the language.  \n", yytext, lineCounter);  
	/*lexErrorStopParsingAndContinueOnlyLexAnalysis();*/
	return yylex();
	}
	
<<EOF>> {
	create_and_store_token(EOF_tok, "EOF", lineCounter);  
	fprintf(yyout, "Token of type: '{%s}', lexeme: '{%s}', found in line: {%d}\n", eTokensString[EOF_tok],yytext, lineCounter);
	return 0;
	}

%%

int lexTokenHandel(eTOKENS kind){
	create_and_store_token(kind,yytext,lineCounter);
	fprintf(yyout, "Token of type '{%s}', lexeme: '{%s}', found in line: {%d}\n", eTokensString[kind],yytext, lineCounter);
	return 1;
}

void parseDidntCalledBecauseOfLexError(char* outputFileFullPath){
	FILE* parserFile = fopen(outputFileFullPath, "w");
	fprintf(parserFile, "the parser didn't called because od lexical error in the input file\n");
	fclose(parserFile);
}

int main(int argc, char **argv ){
	++argv, --argc;  /* skip over program name */
	
	char files[8][1000] = {"C:\\temp\\test1.txt", "C:\\temp\\test2.txt", 
	"C:\\temp\\test1_lex.txt", "C:\\temp\\test2_lex.txt", 
	"C:\\temp\\test1_syntactic.txt", "C:\\temp\\test2_syntactic.txt", 
	"C:\\temp\\test1_semantic.txt", "C:\\temp\\test2_semantic.txt"};
	
	int ourUse = 0;
	
	ourUse = 1;
	
	if ( ourUse == 0 ){
		printf("\n\t\t\t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\t Test Mode \t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
		printf("\n\ntest1\n\n");
		for(int i=0 ; i<2; i++){
			yyin = fopen(files[i], "r" );
			yyout = fopen(files[i+2], "w" );
			if(yyin && yyout){					
				parse(files[i+4]);
				//printAllTokens();			
				//nextAndBackTokenTest();				
				newFileOpend();
			}							

			yyrestart(0);
			lineCounter=1;
			if(i==0) printf("\n\ntest2\n\n");
		}
	}
	else{
		if ( argc > 0 ) yyin = fopen( argv[0], "r" );
		else yyin = stdin;		
		if ( argc > 1 ) yyout = fopen( argv[1], "w" );
		else yyout = stdout;

		for (int i = 0; i < 2; i++) {
			yyin = fopen(files[i], "r");
			yyout = fopen(files[i + 2], "w");
			if (yyin && yyout) {
				parse(files[i + 4], files[i + 6]);							
				newFileOpend();
			}
			yyrestart(0);
			lineCounter = 1;
		}
	}		
}