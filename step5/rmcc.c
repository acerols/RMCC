#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

enum{
	TK_NUM = 256,
	TK_EOF,
};

enum{
	ND_NUM = 256,
};

typedef struct {
	int ty;
	int val;
	char *input;
}Token;

Token tokens[100];
int pos = 0;

typedef struct Node{
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
	int op;
} Node;


Node *expr();
Node *mul();
Node *term();
int consume(int ty);

void tokennize(char *p)
{
	int i = 0;
	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}
		
		if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(isdigit(*p)){
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}
		fprintf(stderr, "トークナイズできません : %s\n", p);
		exit(1);
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}
/*
	int j;
	for(j = 0; j <= i; j++){
		printf("%d\n", tokens[j].ty);
	}
	
}
*/

void error(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	exit(1);
	/*fprintf(stderr, "予期せぬトークンです : %s\n",
		tokens[i].input);
	*/
	exit(1);
}


void print_tk(Token tks)
{
	if(tks.ty == TK_NUM){
		printf("%d", tks.val);
		return;
	}
	printf("%c", (char)tks.ty);
}

Node *new_node(int op, Node *lhs, Node *rhs)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->op = op;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = (Node *)malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->op = ND_NUM;
	node->val = val;
	return node;
}

Node *expr()
{
	Node *lhs = mul();
	if(tokens[pos].ty == TK_EOF){
		return lhs;
	}
	if(tokens[pos].ty == '+'){
		pos++;
		return new_node('+', lhs, expr());
	}
	if(tokens[pos].ty == '-'){
		pos++;
		return new_node('-', lhs, expr());
	}
	if(tokens[pos].ty == '(' || tokens[pos].ty ==')')
		return lhs;
	printf("out_expr\n");
	printf("tokens[%d].ty == %d", pos, tokens[pos].ty);
	error("想定しないトークンです : %s", tokens[pos].input);
}

Node *mul()
{
	Node *lhs = term();
	
	if(tokens[pos].ty == TK_EOF)
		return lhs;
	if(tokens[pos].ty == '*'){
		pos++;
		return new_node('*', lhs, mul());
	}
	if(tokens[pos].ty == '/'){
		pos++;
		return new_node('/', lhs, mul());
	}
	if(lhs->op == ND_NUM || lhs->op == '+' || lhs->op == '-'){
		return lhs;
	}
	printf("mul_out");
	error("想定しないトークンです : %s", tokens[pos].input);
}

Node *term()
{
	if(tokens[pos].ty == TK_NUM)
		return new_node_num(tokens[pos++].val);
	
	if(tokens[pos].ty == '('){
		pos++;
		Node *node = expr();
		if(tokens[pos].ty != ')')
			error("開きカッコに対応する閉じカッコが存在しません: %s", tokens[pos].input);
		pos++;
		return node;
	}

	printf("term_out\n");
	error("数値でも開きカッコでもないトークンです : %s", tokens[pos].input);
}

int consume(int ty)
{
	if(tokens[pos].ty != ty)
		return 0;
	pos++;
	return 1;
}

void gen(Node *node)
{
	if(node->op == ND_NUM){
		printf("	push %d\n", node->val);
		return;
	}
	if(node->lhs != NULL){
		gen(node->lhs);
	}
	if(node->rhs != NULL){
		gen(node->rhs);
	}
	
	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->op){
		case '+':
			printf("	add rax, rdi\n");
			break;
		case '-':
			printf("	sub rax, rdi\n");
			break;
		case '*':
			printf("	imul rax, rdi\n");
			break;
		case '/':
			printf("	div rax, rdi\n");
			break;
	}

	printf("	push rax\n");
}

int main(int argc, char **argv)
{
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

//	printf("args : %s\n", argv[1]);

	tokennize(argv[1]);
	Node *node = expr();
//	printf("\n");
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");

	printf("	ret\n");
	return 0;
}
