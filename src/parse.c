#include <rmcc.h>

Node *code[100];

void program();
Node *stmt();
Node *expr();
Node *assign();
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

bool consume_kind(TokenKind tk)
{
	if(token->kind == tk){
		token = token->next;
		return true;
	}
	return false;
}

Token *consume_ident()
{
	if(token->kind == TK_IDENT){
		Token *tok = token;
		token = token->next;
		return tok;
	}
	return NULL;
}

LVar *find_lvar(Token *tok)
{
	for(LVar *var = local; var; var = var->next){
		if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;

	}
	return NULL;
}

void program()
{
	int i = 0;
	while(!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

Node *stmt()
{
	Node *node;
	if(consume_kind(TK_RETURN)){
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		token = token->next;
		node->lhs = expr();
	}
	else{
		node = expr();
	}
	expect(";");
	return node;
}

// expr = equality
Node *expr()
{
	return assign();
}

Node *assign()
{
	Node *node = equality();
	if(consume("="))
		node = new_binary(ND_ASSIGN, node, assign());
	return node;
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

	Token *tok = consume_ident();
	if(tok){
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		LVar *lvar = find_lvar(tok);
		if(lvar){
			node->offset = lvar->offset;
		}
		else{
			lvar = calloc(1, sizeof(LVar));
			lvar->next = local;
			lvar->name = tok->str;
			lvar->len = tok->len;
			lvar->offset = local->offset + 8;
			node->offset = lvar->offset;
			local = lvar;
		}
		return node;
	}

	return new_num(expect_number());
}