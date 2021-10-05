#include <stdio.h>
#include <stdlib.h>

#define	comma	putchar(',')

int	amode, btype, opc, tmp;
unsigned long pc, pcend, offs;
FILE	*fp;

//unsigned getw(FILE *);

main(argc, argv)
int	argc;
char	*argv[];
{
	sscanf(argv[2], "%lx", &offs);
	sscanf(argv[3], "%lx", &pc);
	pc += offs;
	sscanf(argv[4], "%lx", &pcend);
	fp = fopen(argv[1], "rb");
	fseek(fp, offs, SEEK_SET);
	while(! feof(fp)){
		printf("%.8lx\t", pc);
		opc = getc(fp);
		++pc;
		if(opc == 0xfe){
			printf("signed\n");
			continue;
		}
		arit1();
		arit2();
		sinreg();
		onebyt();
		conjp();
		shift();
		jbitres();
		jbitset();
		djnz();
		pupo();
		stores();
		others();
		putchar('\n');
		if(pc >= pcend)
			break;
	}
	fclose(fp);
}

char	*arits1[] = {
	"and", "add", "sub", "mulu"
};

arit1()
{
	if((opc & 0xc0) != 0x40)
		return;
	amode = opc & 3;
	btype = opc & 0x10;
	printf(arits1[(opc & 0xc) >> 2]);
	if(btype)
		putchar('b');
	putchar('\t');
	putop();
	comma;
	putreg();
	if(! (opc & 0x20)){
		comma;
		putreg();
	}
}

char	*arits2[] = {
	"or", "xor", "cmp", "div",
	"ld", "addc", "subb", "ldbe"
};

arit2()
{
	if((opc & 0xc0) != 0x80)
		return;
	amode = opc & 3;
	btype = opc & 0x10;
	printf(arits2[((opc & 0xc) >> 2) | ((opc & 0x20) >> 3)]);
	if(btype)
		putchar('b');
	putchar('\t');
	putop();
	comma;
	putreg();
}

char	*sinregs[] = {
	"error", "clr", "not", "neg",
	"error", "dec", "ext", "inc"
};

sinreg()
{
	if(opc & 0xe8)
		return;
	printf(sinregs[opc & 7]);
	if(opc & 0x10)
		putchar('b');
	putchar('\t');
	putreg();
}

char	*onebyts[] = {
	"ret", "error", "pushf", "popf",
	"error", "error", "error", "trap",
	"clrc", "setc", "di", "ei",
	"clrvt", "nop", "error", "rst"
};

onebyt()
{
	if((opc & 0xf0) != 0xf0)
		return;
	printf(onebyts[opc & 15]);
}

char	*conjps[] = {
	"nst", "nh", "gt", "nc",
	"nvt", "nv", "ge", "ne",
	"st", "h", "le", "c",
	"vt", "v", "lt", "e"
};

conjp()
{
	if((opc & 0xf0) != 0xd0)
		return;
	printf("j%s\t%.4X", conjps[opc & 15], ++pc + signex8(getc(fp)));
}

char	*shifts[] = {
	"shr", "shl", "shra", "error",
	"shrl", "shll", "shral", "norml",
	"shrb", "shlb", "shrab", "error",
	"error", "error", "error", "error"
};

shift()
{
	if((opc & 0xe8) != 8)
		return;
	printf("%s\t%.2X, %.2X", shifts[opc & 7], getc(fp), getc(fp));
	pc += 2;
}

jbitres()
{
	if((opc & 0xf8) != 0x30)
		return;
	printf("jbc%d\t", opc & 7);
	putreg();
	printf(",%.4X", ++pc + signex8(getc(fp)));
}

jbitset()
{
	if((opc & 0xf8) != 0x38)
		return;
	printf("jbs%d\t", opc & 7);
	putreg();
	printf(",%.4X", ++pc + signex8(getc(fp)));
}

djnz()
{
	if(opc != 0xe0)
		return;
	printf("djnz\t");
	putreg();
	printf(",%.4X", ++pc + signex8(getc(fp)));
}

pupo()
{
	if((opc & 0xf8) != 0xc8)
		return;
	if(opc & 4)
		printf("pop");
	else
		printf("push");
	putchar('\t'); 
	amode = opc & 3;
	btype = opc & 0x10;
	putop();
}

stores()
{
	if((opc & 0xf8) != 0xc0)
		return;
	amode = opc & 3;
	btype = opc & 4;
	printf("st");
	if(btype)
		putchar('b');
	putchar('\t');
	putop();
	comma;
	putreg();
}

others()
{
	if(opc == 0xe7)
		printf("ljmp\t%.4X", (pc += 2) + getw(fp));
	if(opc == 0xef)
		printf("lcall\t%.4X", (pc += 2) + getw(fp));
	if((opc & 0xf8) == 0x20)
		printf("sjmp\t%.4X", ++pc + signex11(((opc & 7) << 8) + getc(fp)));
	if((opc & 0xf8) == 0x28)
		printf("scall\t%.4X", ++pc + signex11(((opc & 7) << 8) + getc(fp)));
	if(opc == 0xe3){
		printf("br[%.2X]", getc(fp));
		++pc;
	}
}

putreg()
{
static	reg;

	reg = getc(fp);
	switch(reg){
		case 0 :
		case 1 :
			printf("zero");
			break;
		case 2 :
			printf("ad_res/ad_cmd");
			break;
		case 3 :
			printf("ad_res/hsi_mode");
			break;
		case 4 :
		case 5 :
			printf("hsi_tim/hso_tim");
			break;
		case 6 :
			printf("hsi_sta/hso_cmd");
			break;
		case 7 :
			printf("serial");
			break;
		case 8 :
			printf("int_mask");
			break;
		case 9 :
			printf("int_pend");
			break;
		case 10 :
			printf("timer1/watch");
			break;
		case 11 :
			printf("timer1");
			break;
		case 12 :
		case 13 :
			printf("timer2");
			break;
		case 14 :
			printf("io0/baud");
			break;
		case 15 :
			printf("io1");
			break;
		case 16 :
			printf("io2");
			break;
		case 17 :
			printf("sp_sta/sp_con");
			break;
		case 21 :
			printf("ios0/ioc0");
			break;
		case 22 :
			printf("ios1/ioc1");
			break;
		case 23 :
			printf("pwm_ctrl");
			break;
		case 24 :
			printf("sp");
			break;
		default :
			printf("%.2X", reg);
			break;
	}
	++pc;
}

putop()
{
static	tmp2;

	switch(amode){
		case 0 :
			putreg();
			return;
		case 1 :
			if(btype){
				printf("#%.2X", getc(fp));
				++pc;
				return;
			}
			else{
				printf("#%.4X", getw(fp));
				pc += 2;
				return;
			}
		case 2 :
			printf("[%.2X]", (tmp = getc(fp)) & 0xfe);
			++pc;
			if(tmp & 1)
				putchar('+');
			return;
		case 3 :
			tmp = getc(fp);
			++pc;
			if(! (tmp & 1)){
				tmp2 = getc(fp);
				++pc;
				printf("%x[%.2X]", tmp2, tmp);
				return;
			}
			else{
				tmp2 = getw(fp);
				pc += 2;
				printf("%x[%.2X]", tmp2, tmp & 0xfe);
				return;
			}
	}
}

signex8(x)
int	x;
{
	if(! (x & 0x80))
		return x;
	return 0xff00 | x;
}

signex11(x)
int	x;
{
	if(! (x & 0x400))
		return x;
	return 0xf800 | x;
}

#ifdef __ZTC__
unsigned getw(fp)
FILE	*fp;
{
	return getc(fp) + (getc(fp) << 8);
}
#endif
