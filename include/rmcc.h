
#define _GNU_SOURCE
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
	ND_EQ,  //==
	ND_NE, //!=
	ND_LT, //<
	ND_LE, //<=
	ND_RT, // >
	ND_RE, // >=
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
	int len;
};

Token *token;
char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool at_eof();

Node *expr();
Token *tokenize();
void gen(Node *node);

