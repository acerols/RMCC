.intel_syntax noprefix
.global main
main:
	push 1
	push 2
	pop rdi
	pop rax
	cmp rdi, rax
	setle al
	movzb rax, al
	push rax
	pop rax
	ret
