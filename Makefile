CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: compiler

parser.tab.cpp parser.tab.hpp: parser.y
	bison -d parser.y -o parser.tab.cpp

lex.yy.c: lexer.l parser.tab.hpp
	flex lexer.l

compiler: lex.yy.c parser.tab.cpp main.cpp ast.h
	$(CXX) $(CXXFLAGS) lex.yy.c parser.tab.cpp main.cpp -o compiler

clean:
	rm -f compiler lex.yy.c parser.tab.cpp parser.tab.hpp ast.json
