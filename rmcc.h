#ifndef __RMCC_H__
#define __RMCC_H__

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

typedef struct Node{
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
	int op;
} Node;

typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

#endif