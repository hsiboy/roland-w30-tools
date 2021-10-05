#include <stdio.h>
#include "8097iset.h"

#define	MODE_REG	0
#define	MODE_IMM	1
#define	MODE_INDIR	2
#define	MODE_INDEX	3
typedef unsigned int	uword;

extern	int	getar();

extern outbyte(uword);
extern outaddr(uword);

extern uword	pc,		/* A pc				*/
	pass,		/* 0 vagy 1			*/
	labvalue,	/* Annak az erteke		*/
	val;		/* Aritmetikai kif. erteke	*/

uword	cons,		/* immediate erteke	*/
	reg,		/* Register cim		*/
	index,		/* Indexed mode -nal az index	*/
	amode;		/* Cimzesi mod		*/

extern
char	err,		/* Az error() allitja 1 be 	*/
	*labname,	/* Egy cimke neve		*/
	*ptr,
	*comm,		/* A prancsmezo pointere	*/
	*pars[128];	/* A parameterek pointerei	*/

extern	int	symfnd;		/* Megtalata -e a cimket	*/
uword	mode;

miez()
{
uword	os, i;
static	up,low,mid;

	if(*comm == 0)
		return;
	for(os = 0; pars[os][0]; ++os);
	if(os > 3)
		pars[3] = "";
	for(i = 0; i < MAX_INS; ++i)
		if((!strcmp(mnemos[i].name, comm)) && (os == mnemos[i].ops))
			break;
	if(i == MAX_INS){
		error("Unknown instruction");
		return;
	}
	if(mnemos[i].opcode & 0xff00)
		outbyte(0xfe);
	if(os){
		ptr = pars[0];
		getmode();
	}
	mode = mnemos[i].amodes;
	switch(os){
		case 0:	outbyte(mnemos[i].opcode);
			return;
		case 1:	egyoper(i);
			return;
		case 2:	ketoper(i);
			return;
		case 3:	haroper(i);
			return;
	}
}


egyoper(i)
uword	i;
{
uword	off;

	if(mode & 16){				/* 11 bit offset */
		if(amode != MODE_REG){
			error("Must be an address");
			return;
		}
		off = (reg - pc) - 2;
		if((pass) && (off & 0xfc00) && ((off & 0xfc00) != 0xfc00)){
			error("SJUMP / SCALL offset is out of range");
			off = 2;
		}
		off &= 0x7ff;
		outbyte(mnemos[i].opcode | (off >> 8));
		outbyte(off & 0xff);
		return;
	}
	if(mode & 8){				/* 16 bit offset */
		if(amode != MODE_REG){
			error("Must be an address");
			return;
		}
		off = (reg - pc) - 3;
		outbyte(mnemos[i].opcode);
		outword(off);
		return;
	}
	if(mode & 2){				/* 8 bit offset */
		outbyte(mnemos[i].opcode);
		reljump(1);
		return;
	}
	if(mode == 0x80){			/* Csak a BR  [reg] -nel */
		if(amode != MODE_INDIR){
			error("Illegal addressing mode");
			return;
		}
		outbyte(mnemos[i].opcode);
		outbyte(reg);
		return;
	}
	if(!(mode & 1)){
		if(amode != MODE_REG){
			error("Illegal addressing mode");
			return;
		}
	}
	outbyte(mnemos[i].opcode | (amode & 3));
	outaddr(i);
}

ketoper(i)
uword	i;
{
	if((!(mode & 1)) && (amode != MODE_REG)){
		error("Illeagal addressing mode");
		return;
	}
	outbyte((mnemos[i].opcode & 0xff) | amode);
	outaddr(i);
	ptr = pars[1];
	getmode();
	if(amode != MODE_REG){
		error("Illegal addressing mode");
		outbyte(0);
		return;
	}
	if(mode & 2){
		reljump(1);
		return;
	}
	outbyte(reg);
}


haroper(i)
uword	i;
{
uword	savrg;

	if(mode & 4){
		if(amode != MODE_REG){
			error("Must be a register");
			return;
		}
		savrg = reg;
		ptr = pars[1];
		getmode();
		if((amode != MODE_REG) || (reg > 7)){
			error("Bad bit number");
			return;
		}
		outbyte(mnemos[i].opcode | reg);
		outbyte(savrg);
		ptr = pars[2];
		getmode();
		reljump(1);
		return;
	}
	if((!(mode & 1)) && (amode != MODE_REG)){
		error("Illegal addressing mode");
		return;
	}
	outbyte((mnemos[i].opcode & 0xff) | amode);
	outaddr(i);
	ptr = pars[1];
	getmode();
	if(amode != MODE_REG){
		error("Illegal addressing mode");
		outword(0);
		return;
	}
	outbyte(reg);
	ptr = pars[2];
	getmode();
	if(amode != MODE_REG){
		error("Illegal addressing mode");
		outbyte(0);
		return;
	}
	outbyte(reg);
}

reljump(n)
int	n;
{
uword	off;

	if(amode != MODE_REG){
		error("Must be an address");
		outbyte(0);
		return;
	}
	off = (reg - pc) - n;
	if((pass) && (off & 0xff80) && ((off & 0xff80) != 0xff80)){
		error("Offset balhe");
		off = 0;
	}
	outbyte(off & 0xff);
}
	
outaddr(i)
uword	i;
{
	switch(amode & 3){
		case MODE_REG:
		case MODE_INDIR:
			outbyte(reg);
			return;

		case MODE_IMM:
			if(mnemos[i].opcode & 0x10)
				outbyte(cons);
			else
				outword(cons);
			return;
			
		case MODE_INDEX:
			outbyte(reg);
			outword(index);
			return;
	}
}

getmode()
{
	switch(*ptr){
		case '#':
			amode = MODE_IMM;
			++ptr;
			getar();
			cons = val;
			return;
		
		case '[':
			amode = MODE_INDIR;
			++ptr;
			getar();
			reg = (val & 0xfe);
			if((val & 1) || (val > 255))
				error("Incorrect register address");
			if(*ptr != ']')
				error("Missing ']'");
			else
				++ptr;
			if(*ptr == '+'){
				amode = MODE_INDIR | 0x80;
				reg |= 1;
				++ptr;
			}
			return;
	}
	getar();
	if(*ptr == '['){
		amode = MODE_INDEX;
		index = val;
		++ptr;
		getar();
		reg = val & 0xfe;
		if((val & 1) || (val > 255))
			error("Incorrect register address");
		if(*ptr != ']')
			error("Missing ']'");
		else
			++ptr;
		if((*ptr) && (*ptr != ' '))
			error("Syntax error");
	/*	if(index > 255)	!!!	*/
			reg |= 1;
		return;
	}
	amode = MODE_REG;
	reg = val;
}
