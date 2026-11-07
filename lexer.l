%{
#include <string>
#include <memory>
#include "ast.h"
#include "parser.tab.hpp"
%}

%option noyywrap
%option nounput noinput

%%

[ \t\r\n]+             ; 
"//".*                 ; 

"int"                  { return TOKEN_INT; }
"if"                   { return TOKEN_IF; }
"else"                 { return TOKEN_ELSE; }
"while"                { return TOKEN_WHILE; }

[0-9]+                 { yylval.num = atoi(yytext); return TOKEN_NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = new std::string(yytext); return TOKEN_IDENTIFIER; }

"=="                   { return TOKEN_EQ; }
"!="                   { return TOKEN_NEQ; }
"<="                   { return TOKEN_LE; }
">="                   { return TOKEN_GE; }
"<"                    { return '<'; }
">"                    { return '>'; }

"+"                    { return '+'; }
"-"                    { return '-'; }
"*"                    { return '*'; }
"/"                    { return '/'; }
"="                    { return '='; }
";"                    { return ';'; }
"{"                    { return '{'; }
"}"                    { return '}'; }
"("                    { return '('; }
")"                    { return ')'; }

.                      { std::cerr << "Unknown symbol: " << yytext << std::endl; }

%%
