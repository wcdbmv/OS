include debug.mac		; (1) Объявление файла с макросом
.386P				; (2)
; Структура для описания дескрипторов сегментов
descr	struc			; (3)
limit	dw	0		; (3) Граница (биты 0...15)
base_l	dw	0		; (3) База, биты 0...15
base_m	db	0		; (3) База, биты 16...23
attr_1	db	0		; (3) Байт атрибутов 1
attr_2	db	0		; (3) Граница (биты 16...19) и атрибуты 2
base_h	db	0		; (3) База, биты 24...31
descr	ends			; (4)
; Структура для описания дескрипторов прерываний (шлюзов ловушек)
trap	struc			; (5)
offs_l	dw	0		; (6) Смещение обработчика, биты 0...15
sel	dw	0		; (7) Селектор сегмента команд
rsrv	db	0		; (8) Зарезервировано
attr	db	8fh		; (9) Атрибуты
offs_h	dw	0		; (10) Смещение обработчика, биты 16...31
trap	ends			; (11)

data	segment	use16			; (12)
; Таблица глобальных дескрипторов GDT
gdt_null descr<0,0,0,0,0,0>		; (13) Нулевой дескриптор
gdt_data descr<data_size-1,0,0,92h>	; (14) Сел-р 8, сегмент данных
gdt_code descr<code_size-1,,,98h>	; (15) Селектор 16, сегмент команд
gdt_stack descr<255,0,0,92h>		; (16) Селектор 24, сегмент стека
gdt_screen descr<4095,8000h,0bh,92h>	; (17) Селектор 32, видеобуфер
gdt_size=$-gdt_null			; (18) Размер GDT
; Таблица дескрипторов прерываний (исключений) IDT
idt	label	word		; (19) Начало таблицы IDT
exc0	trap	<exc0>		; (20) Дескриптор исключения 0
exc1	trap	<exc1>		; (21) Дескриптор исключения 1
exc2	trap	<exc2>		; (22) Дескриптор исключения 2
exc3	trap	<exc3>		; (23) Дескриптор исключения 3
exc4	trap	<exc4>		; (24) Дескриптор исключения 4
exc5	trap	<exc5>		; (25) Дескриптор исключения 5
exc6	trap	<exc6>		; (26) Дескриптор исключения 6
exc7	trap	<exc7>		; (27) Дескриптор исключения 7
exc8	trap	<exc8>		; (28) Дескриптор исключения 8
exc9	trap	<exc9>		; (29) Дескриптор исключения 9
exc0ah	trap	<exc0ah>	; (30) Дескриптор исключения 10
exc0bh	trap	<exc0bh>	; (31) Дескриптор исключения 11
exc0ch	trap	<exc0ch>	; (32) Дескриптор исключения 12
exc0dh	trap	<exc0dh>	; (33) Дескриптор исключения 13
exc0eh	trap	<exc0eh>	; (34) Дескриптор исключения 14
exc0fh	trap	<exc0fh>	; (35) Дескриптор исключения 15
exc10fh	trap	<exc10fh>	; (36) Дескриптор исключения 16
exc11h	trap	<exc11h>	; (37) Дескриптор исключения 17
idt_size=$-idt			; (38) Размер таблицы IDT
; Поля данных программы
pdescr	df	0		; (39) Псевдодескриптор для команд lgdt и lidt
mes	db	27,'[31;42m Вернулись в реальный режим! ',27,'[0m$'; (40)
tblhex	db	'0123456789abcef'	; (41) Таблица преобразования bin-hex
string	db	'**** ****-**** ****-**** ****'	; (42) Шаблон для вывода
;		 0    5    10   15   20   25	Позиции в шаблоне
len=$-string			; (43)
home_sel dw	home		; (44) Адрес возврата из исключения
	dw	10h		; (45) Сегмент команд
data_size=$-gdt_null		; (46) Размер сегмента данных
data	ends			; (47)

text	segment	'code' use16	; (48)
	assume	cs:text,ds:data	; (49)
begin	label	word		; (50) Начало сегмента команд
exc_0	proc			; (51) Обработчик исключения 0
	mov	ax,0		; (52) Номер исключения для вывода на экран
	jmp	dword ptr home_sel ; (53) На выход
exc_0	endp			; (54)
exc_1	proc			; (55) Обработчик исключения 1
	mov	ax,1		; (56) Номер исключения для вывода на экран
	jmp	dword ptr home_sel ; (57) На выход
exc_1	endp			; (58)
exc_2	proc
	mov	ax,2
	jmp	dword ptr home_sel
exc_2	endp
exc_3	proc
	mov	ax,3
	jmp	dword ptr home_sel
exc_3	endp
exc_4	proc
	mov	ax,4
	jmp	dword ptr home_sel
exc_4	endp
exc_5	proc
	mov	ax,5
	jmp	dword ptr home_sel
exc_5	endp
exc_6	proc
	mov	ax,6
	jmp	dword ptr home_sel
exc_6	endp
exc_7	proc
	mov	ax,7
	jmp	dword ptr home_sel
exc_7	endp
exc_8	proc
	mov	ax,8
	jmp	dword ptr home_sel
exc_8	endp
exc_9	proc
	mov	ax,9
	jmp	dword ptr home_sel
exc_9	endp
exc_0ah	proc
	mov	ax,0ah
	jmp	dword ptr home_sel
exc_0ah	endp
exc_0bh	proc
	mov	ax,0bh
	jmp	dword ptr home_sel
exc_0bh	endp
exc_0ch	proc
	mov	ax,0ch
	jmp	dword ptr home_sel
exc_0ch	endp
exc_0dh	proc
	mov	ax,0dh
	jmp	dword ptr home_sel
exc_0dh	endp
exc_0eh	proc
	mov	ax,0eh
	jmp	dword ptr home_sel
exc_0eh	endp
exc_0fh	proc
	mov	ax,0fh
	jmp	dword ptr home_sel
exc_0fh	endp
exc_10h	proc
	mov	ax,10h
	jmp	dword ptr home_sel
exc_10h	endp
exc_11h	proc
	mov	ax,11h
	jmp	dword ptr home_sel
exc_11h	endp
main	proc			; (59)
	xor	eax, eax	; (60) Очистим EAX
	mov	ax,data		; (61) Загрузим в DS сегментный
	mov	ds,ax		; (62) адрес сегмента данных
; Вычислим 32-битовый линейный адрес сегмента данных и загрузим его
; в дескриптор сегмента данных в GDT.
	shl	eax,4		; (62) В EAX линейный базовый адрес
	mov	ebp,eax		; (62) Сохраним его в EBP
	mov	bx,offset gdt_data; (62) В BX адрес дескриптора
	mov	[bx].base_l,ax	; (62) Загрузим младшую часть базы
	rol	eax,16		; (62) Обмен старшей и младшей половин EAX
	mov	[bx].base_m,al	; (62) Загрузим среднюю часть базы
; Аналогично для линейного адреса сегмента команд
	xor	eax,eax		; (62) Очистим EAX
	mov	ax,cs		; (62) Адрес сегмента команд
	shl	eax,4		; (62) Умножим на 16
	mov	bx,offset gdt_code; (62) Адрес дескриптора
	mov	[bx].base_l,ax	; (62) Загрузим младшую часть базы
	rol	eax,16		; (62) Обмен половин EAX
	mov	[bx].base_m,al	; (62) загрузим среднюю часть базы
; Аналогично для линейного адреса сегмента стека
	xor	eax,eax		; (62)
	mov	ax,ss		; (62)
	shl	eax,4		; (62)
	mov	bx,offset gdt_stack; (62)
	mov	[bx].base_l,ax	; (62)
	rol	eax,16		; (62)
	mov	[bx].base_m,al	; (62)
; Подготовим псевдодескриптор pdescr и загрузим регистр GDTR
	mov	dword ptr pdescr+2,ebp		; (63) База GDT, биты 0...31
	mov	word ptr pdescr,gdt_size-1	; (64) Граница GDT
	lgdt	pdescr				; (65) Загрузим регистр GDTR
; Подготовимся к переходу в защищённый режим
	cli			; (66) Запрет аппаратных прерываний
	mov	al,80h		; (67) Запрет NMI
	out	70h,al		; (68) Порт КМОП-микросхемы
; Таблица прерываний уже заполнена на этапе трансляции
; Загрузим регистр IDTR
	mov	word ptr pdescr,idt_size-1	; (69) Граница IDT
	xor	eax,eax		; (70) eax=0
	mov	ax,offset idt	; (71) Смещение IDT в сегменте данных
	add	eax,ebp		; (72) Прибавим линейный адрес сегмента
				; данных и получим
	mov	dword ptr pdescr+2,eax	; (73) линейный адрес IDT
	lidt	pdescr		; (74) Загрузим IDTR
; Переходим в защищённый режим
	mov	eax,cr0		; (75) Получим содержимое CR0
	or	eax,1		; (76) Установим бит PE
	mov	cr0,eax		; (77) Запишем назад в CR0
; Теперь процессор работает в защищённом режиме
; Загружаем в CS:IP селектор:смещение точки continue
	db	0EAh		; (78) Код команды far jmp
	dw	offset continue	; (79) Смещение
	dw	16		; (80) Селектор сегмента команд
continue:
; Делаем адресуемыми данные
	mov	ax,8		; (81) Селектор сегмента данных
	mov	ds,ax		; (82)
; Делаем адресуемым стек
	mov	ax,24		; (83) Селектор сегмента стека
	mov	ss,ax		; (84)
; Инициализируем ES и выводим символы
	mov	ax,32		; (85) Селектор сегмента видеобуфера
	mov	es,ax		; (86)
; Организуем периодический вывод на экран символов
	mov	cx,300		; (87) Число символов
	mov	bx,2720		; (88) Начальная позиция на экране
	mov	dx,3001h	; (89) Начальный символ
xxxx:	push	cx		; (90) Сохраним счетчик внешнего цикла
	mov	cx,0		; (91) Повторение команды
zzzz:	loop	zzzz		; (92) loop 65536 раз
	mov	es:[bx],dx	; (93) Вывод в видеобуфер
	inc	dl		; (94) Инкремент символа
	add	bx,2		; (95) Инкремент позиции на экране
	pop	cx		; (96) Извлечём счётчик внешнего цикла
	loop	xxxx		; (97) Цикл
	mov	ax,0FFFFh	; (98) Диагностическое значение
home:	mov	si,offset string; (99)
	debug			; (100)
; Выведем на экран диагностическую строку
	mov	si,offset string; (101)
	mov	cx,len		; (102)
	mov	ah,74h		; (103)
	mov	di,1600		; (104)
scr:	lodsb			; (105)
	stosw			; (106)
	loop	scr		; (107)
; Подготовим переход в реальный режим
; Сформируем и загрузим дескрипторы для реального режима
	mov	gdt_data.limit,0FFFFh	; (108) Граница сегмента данных
	mov	gdt_code.limit,0FFFFh	; (109) Граница сегмента команд
	mov	gdt_stack.limit,0FFFFh	; (110) Граница сегмента стека
	mov	gdt_screen.limit,0FFFFh	; (111) Граница дополнительного сегмента
	mov	ax,8		; (112) Загрузим теневой регистр
	mov	ds,ax		; (113) сегмента данных
	mov	ax,24		; (114) Загрузим теневой регистр
	mov	ss,ax		; (115) стека
	mov	ax,32		; (116) Загрузим теневой регистр
	mov	es,ax		; (117) дополнительного сегмента
; Выполним дальний переход для того, чтобы заново загрузить
; селектор в регистр CS и модифицировать его теневой регистр
	db	0eah		; (118) Командой дальнего перехода
	dw	offset go	; (119) загрузим теневой регистр
	dw	16		; (120) сегмента команд
; Переключим режим процессора
go:	mov	eax,cr0		; (121) Получим содержимое CR0
	and	eax,0fffffffeh	; (122) Сбросим бит PE
	mov	cr0,eax		; (123) Запишем назад в CR0
	db	0eah		; (124) Код команды far jmp
	dw	offset return	; (125) Смещение
	dw	text		; (126) Сегмент
; Теперь процессор снова работает в реальном режиме
; Восстановим операционную среду реального режима
return:	mov	ax,data		; (127) Восстановим
	mov	ds,ax		; (128) адресуемость данных
	mov	ax,stk		; (129) Восстановим
	mov	ss,ax		; (130) адресуемость стека
; Восстановим состояние регистра IDTR реального режима
	mov	ax,3ffh		; (131) Граница таблицы векторов (1Кбайт-1)
	mov	word ptr pdescr,ax ; (132)
	mov	eax,0		; (133) Смещение таблицы векторов
	mov	dword ptr pdescr+2,eax ; (134)
	lidt	pdescr		; (135) Загрузим псевдодескриптор в
				; регистр процессора IDTR
; Разрешим аппаратные и немаскируемые прерывания
	sti			; (136) Разрешение прерываний
	mov	al,0		; (137) Засылка константы с битом
	out	70h,al		; (138) 7=0 в порт CMOS-разрешение NMI
; Проверим выполнение функций DOS после возврата в реальный режим
	mov	ah,09h		; (139)
	mov	dx,offset mes	; (140)
	int	21h		; (141)
	mov	ax,4c00h	; (142) Завершим программу
	int	21h		; (143)
main	endp			; (144)
code_size=$-begin		; (145) Размер сегмента команд
text	ends			; (146)

stk	segment	stack 'stack'	; (147)
	db	256 dup('^')	; (48)
stk	ends			; (149)
	end	main		; (150)
