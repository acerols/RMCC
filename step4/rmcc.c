#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum{
	TK_NUM = 256,
	TK_EOF,
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
void error(int i)
{
	fprintf(stderr, "予期せぬトークンです : %s\n",
		tokens[i].input);
	exit(1);
}
*/

Node *expr();
Node *mul();
Node *term();

Node *new_node(int op, Node *lhs, Node *rhs)
{
	Node *node = malloc(sizeof(Node));
	node->op = op;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = malloc(sizeof(Node));
	node->op = TK_NUM;
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
	}
	error("数値でも開きカッコでもないトークンです : %s", tokens[pos].input);
}

void gen(Node *node)
{
	if(node->ty == TK_NUM){
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->ty){
		case '+':
			printf("	add rax, rdi\n");
			break;
		case '-':
			printf("	sub rax, rdi\n");
			break;
		case '*':
			printf("	mul rax, rdi\n");
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

	char *p = "(1+7)*2";

	tokennize(p);
	printf("tokennizedd\n");
	Node *node = expr();

	printf("analys\n");
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");

	printf("	ret\n");
	return 0;
}
