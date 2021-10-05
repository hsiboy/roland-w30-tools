scr	equ	0e400
kbd_prt	equ	0d806
kbd_tab	equ	073b
keywin	equ	155a
keyin	equ	06f2		;result in 51
sys_chk	equ	1565
goxy	equ	26b1		;4c-x, 4d-y
moniter	equ	21ba
flpy	equ	0c800
w_frdy	equ	2900
w_fit	equ	2912		;??
rd_sec	equ	2a37		;62-trk, 61-hd, 63-sec, 34-addr
wr_sec	equ	2a5a		;same as rd_sec
int_sta	equ	0c200		;??
seek	equ	2973		;62-trk
strouti	equ	2723		;data after the call
strout	equ	26d5		;54-struct
windowi	equ	2729
window	equ	26c8
ad	equ	0c009
memmap	equ	0c600
tx_dat	equ	2306		;1a-char
tx_hex	equ	230d		;1a-hex
rx_wrd	equ	22da		;1c-word
rx_hex	equ	22e5		;1a-hex
sys_res	equ	2102
ovl_add	equ	0200
hex_wri	equ	2737		;50-byt, 4c-x, 4d-y
chr_wri	equ	26a1		;50-chr, 4c-x, 4d-y
conso	equ	28a8		;49-chr, 46-add
picadd	equ	6000


;	   20 21 22					37 38 39
;							34 35 36
;	   25 29 2a					31 32 33
;	24 26 27 28	23 2b 2c 2d 2e 2f 1e 30		30 1d 1f

