#include "base/base.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Lexer Lexer;
typedef u8 CompilerError;
typedef struct CompilerErrorInfo CompilerErrorInfo;

enum CompilerError {
	CompilerError_None = 0,
	CompilerError_BadCodepoint = 1,
};

struct CompilerErrorInfo {
	CompilerError error;
	isize offset;
	String source_file;
	CompilerErrorInfo* next;
};

struct Lexer {
	isize current;
	isize start;
	String source;
	Arena* arena;

	CompilerErrorInfo* error;
};

#include "tokens.gen.c"

Lexer lexer_create(String source, Arena* arena){
	return (Lexer){
		.source = source,
		.current = 0,
		.start = 0,
		.arena = arena,
		.error = NULL,
	};
}

void lexer_push_error(Lexer* lex, CompilerError err){
	CompilerErrorInfo* info = arena_make(lex->arena, CompilerErrorInfo);
	if(!info){ return; }

	info->error = err;
	info->offset = lex->current;
	// info->source_file = lex->source_file;
	info->next = lex->error;
	lex->error = info;
}

int main(){
	return 0;
}

