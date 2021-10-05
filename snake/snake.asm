zero	equ	0

ax	equ	80
al	equ	80
ah	equ	81
bx	equ	82
bl	equ	82
bh	equ	83
cx	equ	84
cl	equ	84
ch	equ	85
dx	equ	86
dl	equ	86
dh	equ	87
hpoi	equ	88
epoi	equ	8a
ppoi	equ	8c
scor	equ	8e
rompoi	equ	90
sp	equ	18

padd	equ	46
pchar	equ	49
keycod	equ	51

mapper	equ	0c600
picinit	equ	0190
consi	equ	06f2
keyin	equ	155a
conso	equ	28a8
linout	equ	26d5
boot	equ	0d51

kup	equ	'8'
kdown	equ	'5'
kleft	equ	'4'
kright	equ	'6'
kexit	equ	1e

left	equ	0
right	equ	1
up	equ	2
down	equ	3

food	equ	90
snak	equ	91

column	equ	28
row	equ	8

leds	equ	0d800

	org	4000

	skip	0
	skip	0

start:
	ld	#stacktop,sp		;make some stack
	stb	mapper[0],0		;make ROM available
	call	intro
;	ld	0,bx
;	ldb	1,al
;s1:
;	stb	leds[0],al
;s2:
;	dec	bx
;	jne	s2
;	inc	ax
;	jne	s1
	call	wkey
	cmpb	#kexit,keycod
	jne	playit
	jmp	boot

playit:
	ld	#snake,ax		;tabla init
	ld	#row*column,cx
	ldb	#20,bl
p1:
	stb	[ax]+,bl
	dec	cx
	jne	p1

	ld	#snake+1,ax		;felso vonal
	ldb	#column-2,cl
	ldb	#2,bl
p2:
	stb	[ax]+,bl
	djnz	cl,p2

	ld	#snake+(row-1)*column+1,ax ;also vonal
	ldb	#column-2,cl
	ldb	#8,bl
p3:
	stb	[ax]+,bl
	djnz	cl,p3

	ld	#snake+column,ax	;bal fuggoleges vonal
	ldb	#row-2,cl
	ldb	#4,bl
p4:
	stb	[ax],bl
	add	#column,ax
	djnz	cl,p4

	ld	#snake+2*column-1,ax	;jobb fuggoleges vonal
	ldb	#row-2,cl
	ldb	#7,bl
p5:
	stb	[ax],bl
	add	#column,ax
	djnz	cl,p5

	ld	#snake,ax		;bal folso csucsok
	ldb	#1,bl
	stb	[ax],bl

	ld	#snake+column-1,ax	;jobb folso csucsok
	ldb	#3,bl
	stb	[ax],bl

	ld	#snake+(row-1)*column,ax ;bal also csucsok
	ldb	#5,bl
	stb	[ax],bl

	ld	#snake+row*column-1,ax	;jobb also csucsok
	ldb	#6,bl
	stb	[ax],bl

	ld	#row*column,cx
	ld	6000[0],ppoi
	ld	ppoi,padd
	ld	#snake,dx
p6:
	ldb	[dx]+,pchar
	call	conso
	inc	padd
	dec	cx
	jne	p6

	ld	#snake+3*column+5,ax	;ax <- kigyo kezdeti helye
	ldb	#right,bl		;bl <- kigyo mozgasa
	ld	ax,epoi			;le a kigyo farkat
	stb	[ax]+,bl		;letesszuk a kezdo kigyot
	stb	[ax]+,bl
	stb	[ax]+,bl
	stb	[ax],bl
	ld	ax,hpoi			;le a kigyo fejet

	ldb	#snak,pchar
	add	#3*column+5,ppoi,padd
	ldb	#4,cl
p7:
	call	conso
	inc	padd
	djnz	cl,p7

	ld	#snake+5*column+20,ax	;ax <- elso hami cime
	ldb	#food,bl		;bl <- flammo
	stb	[ax],bl

	add	#5*column+20,ppoi,padd	;elso hami -> kepernyore
	ldb	#food,pchar
	call	conso

	ld	#4,scor
	call	score
game:
	ldb	[hpoi],cl		;kigyo fej mozgasi iranyat cl es
	ldb	cl,bl			;bl-be eldugja
	ldb	0,ch			;ch <- 0
	add	cx,cx			;cx index
	add	table[cx],hpoi
	ldb	[hpoi],bh		;regi adat az uj fej helyen
	stb	[hpoi],bl		;leteszi az uj fej mozgasat
	ld	hpoi,ax
	sub	#snake,ax
	add	ppoi,ax,padd		;padd <- uj fej a kepernyon
	ldb	#snak,pchar
	call	conso
	cmpb	#food,bh		;ha hami volt
	je	grow			;akkor no a kigyo
	cmpb	#20,bh			;ha az uj helyen nem ures van
	je	ok			;akkor meghal
	jmp	died
ok:					;felveszi a kigyo farkanak
	ldb	[epoi],cl		;mozgasi iranyat
	ldb	#' ',bh
	stb	[epoi],bh
	ld	epoi,ax
	sub	#snake,ax
	add	ppoi,ax,padd		;padd <- regi farok a kepernyon
	ldb	#' ',pchar
	call	conso
	ldb	0,ch			;ch <- 0
	add	cx,cx			;cx index
	add	table[cx],epoi
lookkey:
	ld	#800,ax
ww:
	call	consi
	cmpb	#kexit,keycod
	jne	g1
	jmp	start
g1:
	ldb	0,bl
	cmpb	#kleft,keycod
	je	g2
	incb	bl
	cmpb	#kright,keycod
	je	g2
	incb	bl
	cmpb	#kup,keycod
	je	g2
	incb	bl
	cmpb	#kdown,keycod
	je	g2
	dec	ax
	jne	ww
	jmp	game
g2:
	stb	[hpoi],bl		;uj mozgasi irany
	dec	ax
	jne	ww
	jmp	game
grow:
	ld	[rompoi]+,ax		;uj hami generalas
	and	#3fff,rompoi
	mulu	#0b,ax
	cmp	#row*column-1,ax
	jh	grow
	add	#snake,ax
	ldb	[ax],bl
	cmp	#' ',bl
	jne	grow
	ldb	#food,bl
	stb	[ax],bl
	sub	#snake,ax
	add	ppoi,ax,padd
	ldb	#food,pchar
	call	conso
	inc	scor
	call	score
	jmp	lookkey
died:
	call	wkey
	jmp	start

score:
	push	scor
	ld	ppoi,padd
	call	decnum
	pop	scor
	ret

decnum:
	ld	2[sp],ax
	cmp	#0a,ax
	jlt	dn1
	ld	0,bx
	div	#0a,ax
	push	bx
	push	ax
	call	decnum
	pop	ax
	pop	bx
	ldb	bl,al
dn1:
	addb	#'0',al,pchar
	call	conso
	inc	padd
	ret
wkey:
	call	consi
	cmpb	#0ff,keycod
	je	wkey
	ret

intro:
	ldb	#8,al
	ld	#duma,54
inloop:
	call	linout
	djnz	al,inloop
	ret

snake:
	ds	column*row
duma:
	db	0,0,0
	text	'Snakegame Snakegame Snakegame Snakegame '
	db	0,1,0ff
	text	'             for the W-30               '
	db	0,2,0
	text	'     This game was written by Dome      '
	db	0,5,0ff
	text	'Your little snake moves u-8/d-5/l-4/r-6 '
	db	0,6,0
	text	'and you can exit with (what a surprise) '
	db	0,7,0
	text	'the EXIT key ... Press a key to start ! '
	even
table:	dw	-1
	dw	1
	dw	-column
	dw	column

	ds	256
	even
stacktop:
