.intel_syntax noprefix
.global main
main:
	push rbp
	mov rbp, rsp
	sub rsp, 208
	push 1
	push 2
	push 3
	pop rax
	mov rsp, rbp
	pop rbp
	ret
