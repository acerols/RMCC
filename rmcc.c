#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "rmcc.h"

Token tokens[100];
int pos = 0;

Node *add();
Node *mul();
Node *term();
int consume(int ty);

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

void expect(int line, int expected, int actual)
{
	if(expected == actual)
		return;
	fprintf(stderr, "%d: %d expected, but got %d\n",
			line, expected, actual);
	exit(1);
}

void runtest()
{
	Vector *vec = new_vector();
	expect(__LINE__, 0, vec->len);

	for(int i = 0; i < 100; i++)
		vec_push(vec, (void *)i);
	
	expect(__LINE__, 100, vec->len);
	expect(__LINE__, 0, (int)vec->data[0]);
	expect(__LINE__, 50, (int)vec->data[50]);
	expect(__LINE__, 99, (int)vec->data[99]);

	printf("OK\n");
}

Vector *new_vector()
{
	
	Vector *vec = malloc(sizeof(Vector));
	vec->data = malloc(sizeof(void *) * 16);
	vec->capacity = 16;
	vec->len = 0;
	return vec;
}

void vec_push(Vector *vec, void *elem)
{
	if(vec->capacity == vec->len){
		vec->capacity *=2;
		vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
	}
	vec->data[vec->len++] = elem;
}

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

Node *add()
{
	Node *node = mul();

    for(;;){
        if(consume('+'))
            node = new_node('+', node, mul());
        else if(consume('-'))
            node = new_node('-', node, mul());
        else return node;
    }
}

Node *mul()
{
	Node *node = term();

    for(;;){
        if(consume('*'))
            node = new_node('*', node, term());
        else if(consume('/'))
            node = new_node('/', node, term());
        else return node;
    }

	printf("mul_out");
	error("想定しないトークンです : %s", tokens[pos].input);
}

Node *term()
{
	//if(tokens[pos].ty == TK_NUM)
	//	return new_node_num(tokens[pos++].val);
	
	if(consume('(')){
        Node *node = add();
        if(!consume(')'))
            error("開き括弧に対応する閉じ括弧がありません: %s", tokens[pos].input);
        return node;
    }
    
    if(tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);

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


	if(!strcmp(argv[1], "-test")){
		runtest();
		return 0;
	}

//	printf("args : %s\n", argv[1]);

	tokennize(argv[1]);
	Node *node = add();
//	printf("\n");
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");

	printf("	ret\n");
	return 0;
}
