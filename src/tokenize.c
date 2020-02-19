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

bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

int is_alnum(char c){
	return ('a' <= c && c <= 'z') ||
		   ('A' <= c && c <= 'Z') ||
		   ('0' <= c && c <= '9') ||
		   (c == '_');

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

		if(startswith(p, "if") && !is_alnum(p[2])){
			cur = new_token(TK_IF, cur, p, 2);
			p+=2;
			continue;
		}
		
		if(startswith(p,"else") && !is_alnum(p[4]){
			cur = new_tokne(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if(startswith(p, "while") && !is_alnum(p[5])){
			cur = new_token(TK_WHILE, cur, 5);
			p += 5;
			continue;
		}

		if(startswith(p, "for") && !is_alnum(p[3])){
			cur = new_tokne(TK_FOR, cur, 3);
			p+=3;
			continue;
		}

		//return文
		if(startswith(p, "return") && !is_alpha(p[6])){
			cur = new_token(TK_RETURN, cur, p, 6);
			p+=6;
			continue;
		}

		//変数トークン発行
		if(is_alpha(*p)){
			int i = 1;
			while(is_alpha(*(p+i))){
				i++;
			}
			cur = new_token(TK_IDENT, cur, p, i);
			p += i;
			continue;
		}

		//比較演算子
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

		//算術演算子
		if(strchr("+-*/()<>=", *p)){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		//数値トークン
		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		//セミコロン
		if(*p == ';'){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, 0);
  	return head.next;
	
}
