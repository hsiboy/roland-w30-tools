#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define	TRUE	1
#define	FALSE	0

#define	MAX_STK	32	/* Aritmetikai stack merete	*/
#define	MAX_SYM	17	/* Egy cimke hossza		*/
#define	EVEN_B	0xfd
#define	DEF_LINS	4
#define	P_BEG		32
#define	issymch(c)	((isalnum(c)) || (find(c, "._@$")))
#define	isaritch(c)	((issymch(c)) || (find(c, " +-*/%()'")))

extern	void	*malloc();
extern	int	miez();

typedef unsigned int	uword;

typedef struct x {
	char	name[MAX_SYM];
	uword	value;
	struct x *left;
	struct x *right;
} LABEL;

LABEL	*root;
FILE	*fp[4], *texto, *fo, *lstfile;
int	actfile;

uword	pc,		/* A pc				*/
	line,		/* Forditas alatti sor	  	*/
	lins,
	maxlins,
	syms,		/* Definialt cimkek szama 	*/
	kiirva,		/* Cimketabla kiirasakor kell	*/
	errors,		/* Hibak szama			*/
	pass;		/* 0 vagy 1			*/
char	err;		/* Az error() allitja 1 be 	*/
char	inline[256];	/* Bemeneti sor			*/
char	outline[256];	/* Kimeneti sor (printer)	*/
int	pptr;		/* Az outline tombpointere	*/
char	*labname;	/* Egy cimke neve		*/
uword	labvalue,	/* Annak az erteke		*/
	val;		/* Aritmetikai kif. erteke	*/
char	*ptr;
char	*comm;		/* A prancsmezo pointere	*/
char	*pars[128];	/* A parameterek pointerei	*/
int	stack[MAX_STK],	/* Aritmetikai stack		*/
	esp;		/* Aritmetikai stack pointer	*/
int	symfnd;		/* Megtalata -e a cimket	*/
char	printer;	/* 				*/

char	*dirs[] = {
		"DB", "DW", "TEXT", "DS",
		"ORG", "EVEN", "INCLUDE", "PRINT"
};

#define	DIRES	(sizeof(dirs) / sizeof(char *))

extern	dir_db(),
	dir_dw(),
	dir_text(),
	dir_ds(),
	dir_org(),
	dir_even(),
	dir_incl(),
	dir_print();

int	(*drout[DIRES])() = {
		dir_db,   dir_dw,     dir_text,
		dir_ds,   dir_org,    dir_even,
		dir_incl, dir_print
};

char	*nullstring = "";
char	srcfile[100],
	outfile[100],
	prnfile[100];

extern push(uword);
extern outbyte(uword);

main(argc, argv)
int	argc;
char	*argv[];
{
char	*fsp, *sfile, *ofile, *pfile, ap;

	printf("Intel 8097 Cross assembler v1.2 by Arthur Computer Systems, 1990\n");
	if(argc == 1){
		printf("Usage:\n\tA97 <input> [output[.ext]] [-options]\n\n");
		printf("Options:\t-s     forditasi lista kepernyore\n");
		printf("\t\t-l     forditasi lista <input>.lst -be\n");
		printf("\t\t-p     forditasi lista printerre\n");
		printf("\t\t-m<x>  max <x> sort general egy sorbol\n");
		printf("\t\t       default = 4\n");
		exit(0);
	}
	strcpy(srcfile, argv[1]);
	strcpy(outfile, argv[1]);
	strcpy(prnfile, argv[1]);
	if(strchr(srcfile, '.') == NULL)
		sfile = strcat(srcfile, ".asm");
	else
		sfile = srcfile;
	if((fp[actfile = 0] = fopen(sfile, "r")) == NULL){
		printf("Can't open '%s' source file: '%s'\n", srcfile);
		exit(-1);
	}
	if((fsp = strchr(outfile, '.')) != NULL)
		*fsp = 0;
	ofile = strcat(outfile, ".cod");
	if((fsp = strchr(prnfile, '.')) != NULL)
		*fsp = 0;
	pfile = strcat(prnfile, ".lst");
	printer = FALSE;
	texto = NULL;
	maxlins = DEF_LINS;
	for(ap = 2; ap != argc; ++ap){
		if(argv[ap][0] == '-'){
			switch(toupper(argv[ap][1])){
				case 'M':	if(sscanf(&argv[ap][2], "%d", &maxlins) != 1){
							printf("Bad parameter for the 'm' switch");
							maxlins = DEF_LINS;
						}
						break;

				case 'P':	printer = TRUE;
						texto = stdprn;
						break;

				case 'S':	printer = TRUE;
						texto = stdout;
						break;

				case 'L':	if(!(texto = fopen(pfile, "w")))
							error("Can't open list file\n");
						else
							printer = TRUE;
						break;

				default:	printf("Unknown switch\n");
						break;
			}
		} else
			ofile = argv[ap];
	}
	if((fo = fopen(ofile, "wb")) == NULL){
		printf("Can't open output file\n");
		exit(-1);
	}
	if(maxlins)
		--maxlins;
	root = NULL;
	pc = 0;
	pass = 0;
	errors = 0;
	syms = 0;
	line = 0;
	printf("Pass 1\n");
	while(getline()){
		parser();
		err = FALSE;
		forditoka();
		++line;
	}
	pc = 0;
	pass = 1;
	line = 0;
	fp[actfile = 0] = fopen(sfile, "r");
	printf("Pass 2\n");
	while(getline()){
		parser();
		forditoka();
		if(printer)
			fprintf(texto, outline);
		++line;
	}
	printf("\n %d line(s) compiled, ", line);
	if(!errors)
		printf("no error\n");
	else
		printf("%d error(s)\n", errors);

	if((printer) && (texto != stdout)){
		fprintf(texto, "\n%d line(s) compiled, ", line);
		if(!errors)
			fprintf(texto, "no error\n");
		else
			fprintf(texto, "%d error(s)\n", errors);
	}
	if(printer)
		dumplabs();
	fclose(fo);
	if(texto)
		fclose(texto);
	exit(errors);
}

getline()
{
	do{
		if(fgets(inline, 255, fp[actfile]))
			return TRUE;
		fclose(fp[actfile]);
	} while(actfile--);
	return FALSE;
}

parser()
{
char	c, *src, *dst, ap, spc, *xx;
int	i;

	setout();
	lins = maxlins;
	strncpy(&outline[P_BEG], inline, 46);
	if(outline[i = (strlen(outline) - 1)] != '\n'){
		outline[i] = '\n';
		outline[i + 1] = 0;
	}
	ap = spc = FALSE;
	src = dst = inline;
	while((c = *src++) != '\n'){
		if(ap){
			if(c == 0x27){
				*dst++ = 0x27;
				ap = FALSE;
			} else
				*dst++ = c | 0x80;
			continue;
		}
		if(c == ';')
			break;
		if(c == ':')
			c = ' ';
		if((c == ' ') || (c == '\t')){
			if(!spc){
				*dst++ = ' ';
				spc = TRUE;
			}
			continue;
		}
		if(c == 0x27)
			ap = TRUE;
		spc = FALSE;
		*dst++ = toupper(c);
	}
	if(ap){
		error("Missing '");
		*dst++ = 0x27;
	}
	*dst = 0;
	for(i = 0; i < 128; ++i)
		pars[i] = nullstring;
	comm = inline;
	while((*comm) && (*comm != ' '))
		++comm;
	if(*comm == 0)
		return;
	*comm++ = 0;
	ptr = comm;
	while((*ptr) && (*ptr != ' '))
		++ptr;
	if(*ptr == 0)
		return;
	*ptr++ = 0;
	pars[0] = ptr;
	for(i = 1; i < 128; ++i){
		while((*ptr) && (*ptr != ','))
			++ptr;
		if(*ptr == 0)
			return;
		*ptr++ = 0;
		pars[i] = ptr;
	}
}

forditoka()
{
	if((*inline == 0) && (*comm == 0))
		return;
	if(direc())
		return;
	ptr = pars[0];
	miez();
}


direc()
{
char	c;
uword	cp, i;

	if(!strcmp(comm, "EQU")){
		if(*inline == 0){
			error("EQU directive without symbol");
			return TRUE;
		}
		if(!getarit(0))
			error("Missing parameter");
		sprintf(outline, "%.4X", val);
		outline[4] = ' ';
		if(pass == 0)
			insert(inline, val);
		return TRUE;
	}
	if((pass == 0) && (inline[0]))
		insert(inline, pc);

	for(i = 0; i < DIRES; ++i)
		if(!strcmp(comm, dirs[i])){
			drout[i]();
			return TRUE;
		}
	return FALSE;	/* Nem direktiva */
}

dir_dw()
{
int	x;

	if(pars[0][0] == 0){
		error("Missing parameter");
		return;
	}
	for(x = 0; pars[x][0]; ++x){
		getarit(x);
		outword(val);
	}
}

dir_org()
{
	if(!getarit(0)){
		error("Missing parameter");
		return;
	}
	if(!symfnd){
		error("No valid value in pass 1");
		return;
	}
	pc = val;
}

dir_ds()
{
int	i;

	if(!getarit(0)){
		error("Missing parameter");
		return;
	}
	if(!symfnd){
		error("U.A. mint org -nal");
		return;
	}
	for(i = 0; i < val; ++i)
		outbyte(0);
}

dir_text()
{
	dir_db();
	outbyte(0xfe);
}

dir_db()
{
int	i, j;

	if(pars[0][0] == 0){
		error("Missing parameter");
		return;
	}
	for(i = 0; pars[i][0]; ++i){
		if((pars[i][0] == 0x27) && (strlen(pars[i]) > 3)){
			if(pars[i][1] == 0x27){
				error("Missing character");
				return;
			}
			for(j = 1; pars[i][j] != 0x27; ++j)
				outbyte(pars[i][j] & 0x7f);
		} else{
			getarit(i);
			outbyte(val);
		}
	}
}

dir_incl()
{
	if(actfile == 3){
		error("Too many includes");
		return;
	}
	if((fp[++actfile] = fopen(pars[0], "r")) == NULL){
		/*error("Can't open '%s' include file", pars[0]);*/
		error("Can't open include file");
		--actfile;
	}
}

dir_even()
{
	if(pc & 1)
		outbyte(EVEN_B);
}

dir_print()
{
	printf("Message: %s\n", pars[0]);
}
/**********************************************************************
*
*
***********************************************************************/

getarit(n)
int	n;
{
	ptr = pars[n];
	return getar();
}

getar()
{
uword	p, c;
int	zek;

	symfnd = TRUE;
	val = 0;
	if(!*ptr)
		return FALSE;
	p = zek = 0;
	while(c = ptr[p++]){
		if(c == '(')
			++zek;
		if(c == ')')
			--zek;
	}
	if(zek){
		if(zek > 0)
			error("Missing ')'");
		else
			error("Missing '('");
		val = 0;
		return TRUE;
	}
	esp = 0;
	err = FALSE;
	val = calcul();
	if((pass) && (!symfnd))
		error("Symbol not defined");
	return TRUE;
}

calcul()
{
int	stk, nextn, lastp, c;

	if(err)
		return 0;
	stk = 0;
	nextn = TRUE;
	lastp = 0;
	while(isaritch(c = *ptr)){
		if(c == ')'){
			++ptr;
			break;
		}
		if(c == ' '){
			++ptr;
			continue;
		}
		if((c == '(') && (nextn)){
			++ptr;
			push(calcul());
			if(err)
				return 0;
			++stk;
			nextn = FALSE;
			lastp = 0;
			continue;
		}
		if(nextn){
			push(getnum());
			if(err)
				return 0;
			++stk;
			nextn = FALSE;
			continue;
		}
		if(prior(c) < lastp){
			cal();
			if(err)
				return 0;
			stk -= 2;
		}
		push(c);
		if(err)
			return 0;
		++stk;
		lastp = prior(c);
		++ptr;
		nextn = TRUE;
	}
	if((stk - 1) % 2){
		error("Syntax error");
		return 0;
	}
	while(stk > 1){
		cal();
		if(err)
			return 0;
		stk -= 2;
	}
	return pop();
}

push(c)
uword	c;
{
	stack[esp++] = c;
	if(esp == MAX_STK){
		--esp;
		error("Aritmetic stack overflow");
	}
}

pop()
{
	if(!esp){
		error("Aritmetic stack empty");
		return 0;
	}
	return stack[--esp];
}

prior(c)
char	c;
{
	switch(c){
		case '+':
		case '-':	return 5;
				break;
		case '*':
		case '/':
		case '%':	return 10;
				break;
		default:	return 0;
				break;
	}
}

cal()
{
uword	n1, n2, op;

	n2 = pop();
	op = pop();
	n1 = pop();
	switch(op){
		case '+':	push(n1 + n2);
				return;
		case '-':	push(n1 - n2);
				return;
		case '*':	push(n1 * n2);
				return;
		case '/':	push(n1 / n2);
				return;
		case '%':	push(n1 % n2);
				return;
		default:	error("Unknown operator");
				push(0);
				return;
	}
}

getnum()
{
int	n, sign, c, lonyal;
char	*savptr;

	if(*ptr == '-'){
		sign = -1;
		++ptr;
	} else
		sign = 1;
	if(!(isdigit(*ptr)))
		return(sign * fromtext());
	lonyal = 16;
	savptr = ptr;
	while(isxdigit(*savptr))
		++savptr;
	if(*savptr == 'D')
		lonyal = 10;
	n = 0;
	err = FALSE;
	while(isxdigit(c = *ptr)){
		++ptr;
		if(xxxx(c) >= lonyal){
			error("Bad number");
			break;
		}
		n = n * lonyal + xxxx(c);
	}
	while(isxdigit(*ptr))
		++ptr;
	if(*ptr == 'H')
		++ptr;
	if(err)
		return 0;
	return n * sign;
}

xxxx(c)
char	c;
{
	if(isdigit(c))
		return c - '0';
	else
		return (c - 'A') + 10;
}

fromtext()
{
char	text[32];
char	c, p, *tp;

	if(*ptr == 0x27){
		if((ptr[2] != 0x27) && (ptr[3] != 0x27)){
			error("Bad constant");
			++ptr;
			while(*ptr != 0x27)
				++ptr;
			return 0;
		}
		if(ptr[2] == 0x27){
			ptr += 3;
			return (*(ptr-2)) & 0x7f;
		} else {
			ptr += 4;
			return 256 * ((*(ptr-3)) & 0x7f)
				+ ((*(ptr-2)) & 0x7f);
		}
	}
	for(p = 0; issymch(*ptr);)
		text[p++] = *ptr++;
	text[p] = 0;
	if(!strcmp("$", text))
		return pc;
	symfnd = search(text);
	return labvalue;
}

/****************************************************************
*			Cimkek					*
*****************************************************************/
/*
 *	Betesz egy cimket a tablaba
 */
insert(s, v)
char	*s;
uword	v;
{
	if((isdigit(*inline)) || (!issymch(*inline))){
		error("Bad character in symbol");
		return;
	}
	if(strlen(s) >= MAX_SYM)
		s[MAX_SYM-1] = 0;
	labname = s;
	labvalue = v;
	in_label(&root);
}

in_label(poi)
LABEL	**poi;
{
LABEL	*tmp;
int	x;

	if(*poi == NULL){
		if((tmp = malloc(sizeof(LABEL))) == NULL){
			error("Out of memory !");
			exit(-1);
		}
		*poi = tmp;
		tmp->value = labvalue;
		strcpy(tmp->name, labname);
		tmp->left = NULL;
		tmp->right = NULL;
		++syms;
		return;
	}
	if((x = strcmp(labname, (*poi)->name)) == 0){
		error("Multidefined label");
		return;
	}
	if(x < 0)
		in_label(&((*poi)->left));
	else
		in_label(&((*poi)->right));
}

LABEL	*sch_label(poi)
LABEL	*poi;
{
int	x;

	if(poi == NULL)
		return NULL;
	if((x = strcmp(labname, poi->name)) == 0)
		return poi;
	if(x < 0)
		return sch_label(poi->left);
	else
		return sch_label(poi->right);
}

search(s)
char	*s;
{
LABEL	*poi;

	labname = s;
	labvalue = 0;
	if((poi = sch_label(root)) == NULL)
		return FALSE;
	labvalue = poi->value;
	return TRUE;
}



dumplabs()
{
	kiirva = 0;
	fprintf(texto, "\n%d symbols defined\n", syms);
	dumpl(root);
	putc('\n', texto);
}

dumpl(poi)
LABEL	*poi;
{
	if(!poi)
		return;
	if(poi->left)
		dumpl(poi->left);
	irdki(poi);
	if(poi->right)
		dumpl(poi->right);
}

irdki(poi)
LABEL	*poi;
{
char	x;

	if(kiirva++ == 3){
		putc('\n', texto);
		kiirva = 1;
	}
	fprintf(texto, "%-16s .. %.4X ", poi->name, poi->value);
	++syms;
}

/******************************************************************************
*
*
*
************************/
error(s)
char	*s;
{
	++errors;
	err = TRUE;
	printf("Line %d, error: %s\n", line + 1, s);
	if(texto != stdout)
		printf(outline);
	if((printer) && (texto != stdout))
		fprintf(texto, "Line %d, error: %s\n", line + 1, s);
}


find(c, s)
char	c, *s;
{
	while(*s)
		if(*s++ == c)
			return TRUE;
	return FALSE;
}

setout()
{
int	i;

	for(i = 0; i < P_BEG; ++i)
			outline[i] = ' ';
	outline[P_BEG] = '\n';
	outline[P_BEG + 1] = '\0';
	sprintf(outline, "%.4X  ", pc);
	outline[pptr = 6] = ' ';
	if(actfile)
		outline[P_BEG - 2] = 'I';
}

outbyte(c)
uword	c;
{
int	i;

	if(pptr > 24){
		if((printer) && (pass) && (lins)){
			--lins;
			fprintf(texto, outline);
			setout();
			sprintf(&outline[pptr], "%.2X ", c & 255);
			outline[pptr += 3] = ' ';
		}
	} else{
		sprintf(&outline[pptr], "%.2X ", c & 255);
		outline[pptr += 3] = ' ';
	}
	if(c > 255)
		error("Argument overflow");
	++pc;
	if(pass)
		putc(c & 255, fo);
}

outword(c)
uword	c;
{
	if(pptr > 22){
		if((printer) && (pass) && (lins)){
			--lins;
			fprintf(texto, outline);
			setout();
			sprintf(&outline[pptr], "%.4X ", c);
			outline[pptr += 5] = ' ';
		}
	} else{
		sprintf(&outline[pptr], "%.4X ", c);
		outline[pptr += 5] = ' ';
	}
	pc += 2;
	if(pass){
		putc(c & 255, fo);
		putc(c >> 8, fo);
	}
}
