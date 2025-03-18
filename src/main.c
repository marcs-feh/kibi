#include <stdio.h>
#include "base/base.h"
#include "mimalloc.h"

int main(){
	printf("Yo %p\n", mi_zalloc_aligned(100, 512));
	ensure(2 + 2 == 5, "zamn...");
	return 0;
}
