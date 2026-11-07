%{
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

extern int yylex();
void yyerror(const char* s);

std::shared_ptr<BlockNode> rootAST;
%}

%union {
	int num;
	std::string* str;
	ASTNode* node;
	BlockNode* block;
}

%token TOKEN_INT TOKEN_IF TOKEN_ELSE TOKEN_WHILE
%token <num> TOKEN_NUMBER
%token <str> TOKEN_IDENTIFIER
%token TOKEN_EQ TOKEN_NEQ TOKEN_LE TOKEN_GE

%type <node> statement expr declaration if_stmt while_stmt
%type <block> statement_list block program

%right '='
%left TOKEN_EQ TOKEN_NEQ '<' '>' TOKEN_LE TOKEN_GE
%left '+' '-'
%left '*' '/'

%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%%

program
	: statement_list { rootAST = std::shared_ptr<BlockNode>($1); }
	;

statement_list
	: /* порожньо */ { $$ = new BlockNode(); }
	| statement_list statement {
		$$ = $1;
		if ($2) $$->addStatement(std::shared_ptr<ASTNode>($2));
	}
	;

block
	: '{' statement_list '}' { $$ = $2; }
	;

statement
	: declaration ';' { $$ = $1; }
	| expr ';'        { $$ = $1; }
	| if_stmt         { $$ = $1; }
	| while_stmt      { $$ = $1; }
	;

declaration
	: TOKEN_INT TOKEN_IDENTIFIER '=' expr {
		$$ = new AssignNode(*$2, std::shared_ptr<ASTNode>($4));
		delete $2;
	}
	;

if_stmt
	: TOKEN_IF '(' expr ')' block %prec LOWER_THAN_ELSE {
		$$ = new IfNode(std::shared_ptr<ASTNode>($3), std::shared_ptr<ASTNode>($5));
	}
	| TOKEN_IF '(' expr ')' block TOKEN_ELSE block {
		$$ = new IfNode(std::shared_ptr<ASTNode>($3), std::shared_ptr<ASTNode>($5), std::shared_ptr<ASTNode>($7));
	}
	;

while_stmt
	: TOKEN_WHILE '(' expr ')' block {
		$$ = new WhileNode(std::shared_ptr<ASTNode>($3), std::shared_ptr<ASTNode>($5));
	}
	;

expr
	: TOKEN_IDENTIFIER '=' expr {
		$$ = new AssignNode(*$1, std::shared_ptr<ASTNode>($3));
		delete $1;
	}
	| expr '+' expr { $$ = new BinaryOpNode("+", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr '-' expr { $$ = new BinaryOpNode("-", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr '*' expr { $$ = new BinaryOpNode("*", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr '/' expr { $$ = new BinaryOpNode("/", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr '<' expr { $$ = new BinaryOpNode("<", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr '>' expr { $$ = new BinaryOpNode(">", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| expr TOKEN_EQ expr { $$ = new BinaryOpNode("==", std::shared_ptr<ASTNode>($1), std::shared_ptr<ASTNode>($3)); }
	| '(' expr ')'  { $$ = $2; }
	| TOKEN_NUMBER  { $$ = new NumberNode($1); }
	| TOKEN_IDENTIFIER {
		$$ = new VariableNode(*$1);
		delete $1;
	}
	;

%%

void yyerror(const char* s) {
	std::cerr << "Syntax error: " << s << std::endl;
}
