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

typedef struct Node Node;

struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
}TokenKind;

typedef struct Token Token;

struct Token{
	TokenKind kind;
	Token *next;
	int val;
	char *str;
};

Token *token;
int pos = 0;

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

bool consume(char op)
{
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

//次のトークンが期待している記号のときにはトークンを１つ読み進める
//それ以外はエラー報告
void expect(char op)
{
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error_at(token->str, "expected '%c'", op);
	token = token->next;
}

//次のトークンが数値の場合、トークンを１つ読み進めてその数値を返す。
//それ以外の場合はエラーを報告する
int expect_number()
{
	if(token->kind != TK_NUM)
		error_at(token->str, "expected a number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof()
{
	return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

Node *new_node(NodeKind kind)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_num(int val){
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}
Node *expr();
Node *mul();
Node *unary();
Node *primary();

Node *expr()
{
	Node *node = mul();
	
	for(;;){
		if(consume('+'))
			node = new_binary(ND_ADD,  node, mul());
		else if(consume('-'))
			node = new_binary(ND_SUB, node, mul());
		else
			return node;		
	}
}

Node *mul()
{
	Node *node = unary();

	for(;;){
		if(consume('*'))
			node = new_binary(ND_MUL, node, unary());
		else if(consume('/'))
			node = new_binary(ND_DIV, node, unary());
		else return node;
	}
}

Node *unary()
{
	if(consume('+'))
		return unary();
	if(consume('-'))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

Node *primary()
{
	if(consume('(')){
		Node *node = expr();
		expect(')');
		return node;
	}
	return new_num(expect_number());
}

Token *tokenize()
{
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

#ifdef debug
	int c = 0;
#endif

	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}
		if(strchr("+-*/()", *p)){
			cur = new_token(TK_RESERVED, cur, p++);
#ifdef debug
			printf("%d", ++c);
#endif
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "invalid token");
	}
	new_token(TK_EOF, cur, p);
	return head.next;
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
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
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

	user_input = argv[1];
	token = tokenize(user_input);
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
