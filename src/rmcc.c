#include <rmcc.h>

extern char *user_input;

int main(int argc, char **argv)
{
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	local = calloc(1, sizeof(LVar));
	local->offset = 0;
	local->next = NULL;
	program();
//	printf("\n");
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, 208\n");

	for(int i = 0; code[i]; i++){
		gen(code[i]);
	}

	return 0;
}
