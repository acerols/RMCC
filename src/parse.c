#include <rmcc.h>

Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

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

Node *new_num(int val)
{
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

//次のトークンが期待している記号のときにはトークンを１つ読み進める
//それ以外はエラー報告
void expect(char *op)
{
	if(token->kind != TK_RESERVED ||
	   strlen(op) != token->len ||
	   memcmp(token->str, op, token->len))
		error_at(token->str, "expected '%s'", op);
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


bool consume(char *op)
{
	if(token->kind != TK_RESERVED ||
	   strlen(op) != token->len ||
	   memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}


// expr = equality
Node *expr()
{
	return equality();
}

Node *equality()
{
	Node *node = relational();

	for(;;){
		if(consume("=="))
			node = new_binary(ND_EQ, node, relational());
		else if(consume("!="))
			node = new_binary(ND_NE, node, relational());
		else
			return node;
	}
}

Node *relational()
{
	Node *node = add();

	for(;;){
		if(consume("<"))
			node = new_binary(ND_LT, node, add());
		else if(consume("<="))
			node = new_binary(ND_LE, node, add());
		else if(consume(">"))
			node = new_binary(ND_RT, node, add());
		else if(consume(">="))
			node = new_binary(ND_RE, node, add());
		else
			return node;
	}
}

//add  mul ("+" mul | "-" mul)*
Node *add()
{
	Node *node = mul();

	for(;;){
		if(consume("+"))
			node = new_binary(ND_ADD, node, mul());
		else if(consume("-"))
			node = new_binary(ND_SUB, node, mul());
		else return node;
	}

}

// mul unary ("*" unary | "/" unary)
Node *mul()
{
	Node *node = unary();

	for(;;){
		if(consume("*"))
			node = new_binary(ND_MUL, node, unary());
		else if(consume("/"))
			node = new_binary(ND_DIV, node, unary());
		else return node;
	}
}

// unary ("+" | "-")primary()
Node *unary()
{
	if(consume("+"))
		return unary();
	if(consume("-"))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

Node *primary()
{
	if(consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}
	return new_num(expect_number());
}