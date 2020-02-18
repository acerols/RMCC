#include <rmcc.h>

int pos = 0;


// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

LVar *new_lvar(LVar *cur, char *name, int len)
{
	LVar *var = calloc(1, sizeof(LVar));
	var->name = name;
	var->len = len;
	var->offset = cur->offset + 8;
	cur->next = var;
	return var;
}

bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
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

		if(is_alpha(*p)){
			int i = 1;
			while(is_alpha(*(p+i))){
				i++;
			}
			cur = new_token(TK_IDENT, cur, p, i);
			p += i;
			continue;
		}

		if(startswith(p, "==") || startswith(p, "!=") ||
		   startswith(p, "<=") || startswith(p, ">=")){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p+=2;
#ifdef debug
			printf("%d", ++c);
			printf("eq");
#endif
			continue;
		}

		if(strchr("+-*/()<>=", *p)){
			cur= new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		if(*p == ';'){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, 0);
  	return head.next;
	
}
