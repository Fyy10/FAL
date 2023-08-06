all: main

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LLVM_CONFIG=`llvm-config --cxxflags --ldflags --libs`
endif

ifeq ($(UNAME), Darwin)
	LLVM_CONFIG=`/opt/homebrew/opt/llvm/bin/llvm-config --cxxflags --ldflags --libs`
endif


lexer: src/lexer.l src/ast.hpp
	flex -o build/lexer.cpp src/lexer.l

parser: src/parser.y src/ast.hpp
	bison -d -o build/parser.cpp src/parser.y

main: src/*.cpp src/*.hpp lexer parser
	g++ src/*.cpp build/lexer.cpp build/parser.cpp -o build/main -Isrc -Ibuild $(LLVM_CONFIG)

test: main
	./build/main example.fa

clean:
	rm build/*
