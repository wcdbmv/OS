.386p

include	macro.asm

segdesc	struc
	limit	dw 0
	base_l	dw 0
	base_m	db 0
	attr_1	db 0
	arrt_2	db 0
	base_h	db 0
segdesc	ends

intdesc	struc
	offs_l	dw 0
	sel	dw 0
	rsrv	db 0
	attr	db 0
	offs_h	dw 0
intdesc	ends

pseg	segment	'code' use32
	assume	cs:pseg

gdt	label	byte
	gdt_null   segdesc<>
	gdt_4gb    segdesc<0ffffh,,,92h,0cfh>
	gdt_code16 segdesc<rseg_size-1,,,98h>
	gdt_code32 segdesc<pseg_size-1,,,98h,0cfh>
	gdt_data   segdesc<pseg_size-1,,,92h,0cfh>
	gdt_stack  segdesc<sseg_size-1,,,92h,0cfh>
gdt_size = $-gdt

	gdtr	dw gdt_size-1	; Лимит GDT
		dd ?		; Линейный адрес GDT

	sel_4gb    equ 8
	sel_code16 equ 16
	sel_code32 equ 24
	sel_data   equ 32
	sel_stack  equ 40

					;           | Тип шлюза fh — trap gate, eh — interrupt gate
idt	label	byte			; 8fh = 10001111
	trap1	intdesc	13 dup (<,sel_code32,,8fh>)	; Первые 32 элемента таблицы (зарезервированы под исключения)
	trap13	intdesc	<0, sel_code32,,8fh>		; Исключение общей защиты
	trap2	intdesc	18 dup (<,sel_code32,,8fh>)	; Первые 32 элемента таблицы (зарезервированы под исключения)
	int08	intdesc	<,sel_code32,,8eh>		; Дескриптор прерывания от таймера
	int09	intdesc	<,sel_code32,,8eh>		; Дескриптор прерывания от клавиатуры
idt_size = $-idt

	idtr	dw idt_size-1	; Лимит IDT
		dd ?		; Линейный адрес IDT

	idtr_r	dw 3ffh,0,0	; содержимое регистра IDTR в реальном режиме
	; In the 8086 processor, the interrupt table is called IVT (interrupt vector table)
	; The IVT always resides at the same location in memory, ranging from 0000h to 03ffh
	; and consists of 256 four-byte real mode far pointers (256x4=1024 bytes of memory)

	msgp	db 'Processor works in protected mode'
msgp_size = $-msgp
	msgr	db 'Processor works in real mode', '$'
msgr_size = $-msgr-1
	msgt	db 'Timer:'
msgt_size = $-msgt
	msgm	db 'Memory:'
msgm_size = $-msgm

	scan2ascii db 0,1bh,'1','2','3','4','5','6','7','8','9','0','-','=',8
		   db ' ','q','w','e','r','t','y','u','i','o','p','[',']','$'
		   db ' ','a','s','d','f','g','h','j','k','l',';','""',0
		   db '\','z','x','c','v','b','n','m',',','.','/',0,0,0,' ',0,0
		   db 0,0,0,0,0,0,0,0,0,0,0,0

	screen	dd 4*160	; Позиция печати вводимого текста
	timer	dd 0		; Счетчик прошедших тиков таймера

	master	db 0		; Маска прерываний ведущего контроллера
	slave	db 0		; Ведомого

p_entry:
	; Установить 32-х битный стек и другие регистры (загруженные в дескрипторы)
	mov	ax,sel_4gb
	mov	ds,ax
	mov	es,ax
	mov	ax,sel_stack
	mov	ebx,sseg_size
	mov	ss,ax
	mov	esp,ebx

	enable_interrupts

	print_string msgp,msgp_size,1,0
	print_string msgm,msgm_size,1,40
	print_string msgt,msgt_size,1,60

	call	compute_memory

	; Крутимся в бесконечном цикле, периодически натыкаясь на прерывания клавиатуры и таймера
	; Выход из цикла - по нажатию ESC
	jmp	short $

dummy_exc proc
	iretd
dummy_exc endp

exc13	proc
	pop	eax
	iretd
exc13	endp

int08_handler:
	push	eax
	push	ecx
	push	edx

	print_number timer,1,75

	inc	eax
	mov	timer,eax

; Посылаем сигнал EOI контроллеру прерываний
	mov	al,20h
	out	20h,al

	pop	edx
	pop	ecx
	pop	eax
	iretd

int09_handler:
	push	eax
	push	ebx
	push	es
	push	ds

	in	al,60h			; Прочитать скан-код нажатой клавиши из порта клавиатуры
	cmp	al,01h			; Сравниваем с кодом ESC
	je	esc_pressed		; Если ESC, выходим в реальный режим
	cmp	al,39h			; Сравним какой скан-код пришел: обслуживаемой клавиши или нет?
	ja	skip_translate		; Если нет, то ничего не выводим
	mov	bx,sel_data		; Иначе
	mov	ds,bx			; DS:EBX - таблица для перевода скан-кода
	mov	ebx,offset scan2ascii	; в ASCII
	xlatb				; Преобразовать
	mov	bx,sel_4gb
	mov	es,bx			; ES:EBX - адрес текущей
	mov	ebx,screen		; позиции на экране
	cmp	al,8			; Если не была нажата Backspace
	je	bs_pressed
	mov	es:[ebx+0b8000h],al	; Вывести символ на экран
	add	dword ptr screen,2	; Увеличить адрес позиции на 2
	jmp	short skip_translate
bs_pressed:				; Иначе
	mov	al,' '			; нарисовать пробел
	sub	ebx,2			; в позиции предыдущего символа
	mov	es:[ebx+0b8000h],al	; Вывести символ на экран
	mov	screen,ebx		; и сохранить адрес предыдущего символа как текущий
skip_translate:
; Разрешить работу клавиатуры
	in	al,61h
	or	al,80h
	out	61h,al
; Посылаем сигнал EOI контроллеру прерываний
	mov	al,20h
	out	20h,al

	pop	ds
	pop	es
	pop	ebx
	pop	eax
	iretd

esc_pressed:
; Разрешить работу клавиатуры, послать EOI и восстановить регистры.
	in	al,61h
	or	al,80h
	out	61h,al
	mov	al,20h
	out	20h,al
	pop	ds
	pop	es
	pop	ebx
	pop	eax

	disable_interrupts

	; Возврат в реальный режим
	db	0eah
	dd	r_return
	dw	sel_code16

compute_memory proc
	push	ds
	mov	ax,sel_4gb	; Кладем в него сегмент на 4 ГБ - все доступное виртуальное адресное пространство
	mov	ds,ax
	mov	ebx,100001h	; Пропускаем первый мегабайт сегмента (потому, что в противном случае может произойти
				; попытка редактирования процедуры собственного кода)
	mov	dl,11101011b	; Сигнатура
	mov	ecx,0FFEFFFFFh	; В ECX кладём количество оставшейся памяти - чтобы не было переполнения
				; лимит в 4 Гб = 4096 Мб, без одного Мб = 4293918719 байт
check:
	mov	dh,ds:[ebx]	; 
	mov	ds:[ebx],dl	; Пишем сигнатуру
	cmp	ds:[ebx],dl	; Читаем
	jnz	end_of_memory	; если не совпали - то мы достигли конца памяти, выходим из цикла
	mov	ds:[ebx],dh	; если не достигли - кладём обратно сохранённое значение
	inc	ebx		; Проверяем следующий байт и тд (размер памяти можно задать в настройках DOSBOX по умолчанию 16 Мб)
	loop	check

end_of_memory:
	pop	ds
	xor	edx,edx
	mov	eax,ebx
	mov	ebx,100000h	; делим, чтобы получить результат в Мб
	div	ebx

	push	ecx
	push	edx

	print_number eax,1,55

	pop	edx
	pop	ecx

	ret
compute_memory endp

	pseg_size = $-gdt
pseg	ends

rseg	segment para public 'CODE' use16
	assume cs:rseg, ds:pseg, ss:sseg
r_start:
	cls

; Подготовить сегментные регистры
	push	pseg
	pop	ds

	mov ah, 09h
	mov edx, offset msgr
	int 21h

; Вычислить базы для всех используемых дескрипторов сегментов
	xor	eax,eax
	mov	ax,rseg
	shl	eax,4
	mov	word ptr gdt_code16+2,ax	; Базой code16 будет rseg (base_l)
	shr	eax,16
	mov	byte ptr gdt_code16+4,al	; (base_m)
	mov	ax,pseg
	shl	eax,4
	push	eax				; Для вычисления адреса idt
	push	eax				; Для вычисления адреса gdt
	mov	word ptr gdt_code32+2,ax	; Базой всех 32bit будет (base_l)
	mov	word ptr gdt_stack+2,ax		; (base_l)
	mov	word ptr gdt_data+2,ax		; (base_l)
	shr	eax,16
	mov	byte ptr gdt_code32+4,al	; (base_m)
	mov	byte ptr gdt_stack+4,al		; (base_m)
	mov	byte ptr gdt_data+4,al		; (base_m)

; Вычислим линейный адрес GDT
	pop	eax
	add	eax,offset gdt			; В eax будет полный линейный адрес GDT (адрес сегмента + смещение GDT относительно него)
	mov	dword ptr gdtr+2,eax		; Кладём полный линейный адрес в младшие 4 байта переменной gdtr
	mov	word ptr gdtr,gdt_size-1	; В старшие 2 байта заносим размер gdt
						; настоящий размер на 1 байт меньше
; загрузим GDT
	lgdt	fword ptr gdtr

; Аналогично вычислим линейный адрес IDT
	pop	eax
	add	eax,offset idt
	mov	dword ptr idtr+2,eax
	mov	word ptr idtr,idt_size-1

; Заполним смещение в дескрипторах прерываний
	load_offset trap1,dummy_exc
	load_offset trap13,exc13
	load_offset trap2,dummy_exc
	load_offset int08,int08_handler
	load_offset int09,int09_handler

; Cохраним маски прерываний контроллеров
	in	al,21h		; ведущего
	mov	master,al	
	in	al,0a1h		; ведомого
	mov	slave,al

; Перепрограммируем ведущий контроллер прерываний
	mov	al,11h
	out	20h,al
	mov	al,20h	; Базовый вектор (начальное смещение для обработчика) теперь 32 (20h) (вслед за векторами исключений)
	out	21h,al	; Указываем, что аппаратные прерывания будут обрабатываться начиная с 32го (20h)
	mov	al,4
	out	21h,al
	mov	al,1
	out	21h,al

; Запретим все прерывания в ведущем контроллере, кроме IRQ0 (таймер) и IRQ1(клавиатура)
	mov	al,0FCh	; Маска прерываний 11111100
	out	dx,al

; Запретим все прерывания в ведомом контроллере
; В противном случае возникнет исключение - может прийти прерывание
; для которого у нас не написан обработчик
	mov	dx,0A1h
	mov	al,0FFh
	out	dx,al
; Загрузим IDT
	lidt	fword ptr idtr

	open_a20
	disable_interrupts

; Перейти в непосредственно защищенный режим установкой соответствующего бита регистра cr0
	mov	eax,cr0
	or	al,1
	mov	cr0,eax

; Загрузить sel_code32 в регистр CS
; far jump в p_entry
	db	66h
	db	0eah
	dd	offset p_entry
	dw	sel_code32

; Начиная с этой строчки, будет выполняться код защищенного режима p_entry

r_return:
; Переход в реальный режим
	close_a20

	mov	eax,cr0
	and	al,0feh	; сбрасываем флаг защищенного режима
	mov	cr0,eax

; Сбросить очередь и загрузить CS реальным числом
	db	0eah
	dw	$+4
	dw	rseg

; Восстановить регистры для работы в реальном режиме
	mov	ax,pseg	; Загружаем в сегментные регистры реальные смещения
	mov	ds,ax
	mov	es,ax
	mov	ax,sseg
	mov	bx,sseg_size
	mov	ss,ax
	mov	sp,bx

; Реинициализация контроллера прерываний
	mov	al,11h
	out	20h,al
	mov	al,8
	out	21h,al
	mov	al,4
	out	21h,al
	mov	al,1
	out	21h,al

; Восстанавливаем маски контроллеров прерываний
	mov	al,master
	out	21h,al
	mov	al,slave
	out	0a1h,al

; Загружаем таблицу дескрипторов прерываний реального режима
	lidt	fword ptr idtr_r

	enable_interrupts

	setcursor 200h
	print_r msgr
	getchar
	cls

	mov	ah,4ch
	int	21h
rseg_size = $-r_start
rseg	ends

sseg	segment para stack 'stack'
	stack_start	db 100h dup(?)
sseg_size = $-stack_start
sseg	ends

end r_start
