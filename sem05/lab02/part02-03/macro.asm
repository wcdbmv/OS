disable_interrupts macro
	cli

	in	al,70h
	or	al,80h
	out	70h,al
endm

enable_interrupts macro
	in	al,70h
	and	al,7fh
	out	70h,al

	sti
endm

open_a20 macro
	mov	al,0d1h
	out	64h,al
	mov	al,0dfh
	out	60h,al
endm

close_a20 macro
	mov	al,0d1h
	out	64h,al
	mov	al,0ddh
	out	60h,al
endm

far_jump macro d_,lbl,sel
	db	0eah
	d_	offset lbl
	dw	sel
endm

load_offset macro trp,exc
	mov	eax,offset exc
	mov	trp.offs_l,ax
	shr	eax,16
	mov	trp.offs_h,ax
endm

cls	macro
	mov	ax,3
	int	10h
endm

print_r	macro msg
	mov ah,9
	mov edx,offset msg
	int 21h
endm

setcursor macro offst
	mov	ah,2
	xor	bx,bx
	mov	dx,offst
	int	10h
endm

getchar	macro
	mov	ah,8
	int	21h
endm

print_string	macro	msg,msg_size,row,col
	local	screen

	push	ebp
	xor	eax,eax
	mov	ebp,row*160+col*2
	add	ebp,0b8000h
	mov	ecx,msg_size
	xor	esi,esi

screen:
	mov	al,byte ptr msg[esi]
	mov	es:[ebp],al
	add	ebp,2
	inc	esi
	loop	screen

	pop	ebp
endm

print_number	macro	var,row,col
	local	cycle,number,print

	push	ebp
	mov	eax,var
	mov	ebp,row*160+col*2
	mov	ecx,8
	add	ebp,0b8000h
cycle:
	mov	dl,al
	and	dl,0fh
	cmp	dl,10
	jl	number
	add	dl,'a'-10
	jmp	print
number:
	add	dl,'0'
print:
	mov	es:[ebp],dl
	ror	eax,4
	sub	ebp,2
	loop	cycle

	pop	ebp
endm
