#include "base/base.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Lexer Lexer;
typedef u8 CompilerError;
typedef struct CompilerErrorInfo CompilerErrorInfo;

enum CompilerError {
	CompilerError_None = 0,
};

struct CompilerErrorInfo {
	CompilerError error;
	isize offset;
	String source;
	CompilerErrorInfo* next;
};

struct Lexer {
	isize current;
	isize start;
	String source;
	Arena* arena;

	CompilerErrorInfo* error;
};

Lexer lexer_create(String source, Arena* arena){
	return (Lexer){
		.source = source,
		.current = 0,
		.start = 0,
		.arena = arena,
		.error = NULL,
	};
}

// rune lexer_advance(){
// }

int main(){
	Arena arena = arena_create(malloc(4 * 1024), 4 * 1024);

	arena_reset(&arena);
	free(arena.data);
	return 0;
}
