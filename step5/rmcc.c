#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
}NodeKind;

typedef struct {
	int ty;
	int val;
	char *input;
}Token;

Token tokens[100];
int pos = 0;

typedef struct Node Node;

struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};


//エラーを報告関数
//printfと同じ引数
void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

char *user_input;
void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	fprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *expr()
{
	Node *node = mul();
	
	for(;;){
		if(consume('+'))
			node = new_node(ND_ADD, node, mul());
		else if(consume('-'))
			node = new_node(ND_SUB, node, mul());
		else
			return node;		
	}
}

Node *mul()
{
	Node *node = primary();

	for(;;){
		if(consume('*'))
			node = new_node(ND_MUL, node, primary());
		else if(consume('/'))
			node = new_node(ND_DIV, node, primary());
		else return node;
	}
}

Node *primary()
{
	if(consume('(')){
		Node *node = expr();
		expect(')');
		return node;
	}
	return new_node_num(expect_number());
}

bool consume(char op)
{
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

void gen(Node *node)
{
	if(node->kind == ND_NUM){
		printf("	push %d\n", node->val);
		return;
	}
	gen(node->lhs);
	gen(node->rhs);
	
	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->kind){
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
			printf(" 	cqo\n");
			printf("	idiv rdi\n");
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
