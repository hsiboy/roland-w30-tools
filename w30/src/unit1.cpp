//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>
#include <stdlib.h>

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

/*
 *
 * THE PROGRAM ASSUMES 80MB HARD DISK FORMAT!!!!!!!!!!!!!
 *
 */

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

#pragma pack(push, 1)

class m_dword{
	dword	x;
public:
	dword rev(dword n) {
		return ((n & 0x000000ff) << 24) +
			((n & 0x0000ff00) << 8) +
			((n & 0x00ff0000) >> 8) +
			((n & 0xff000000) >> 24);
	}
	m_dword() { x = 0; }
	m_dword(dword n) { x = rev(n); }
	m_dword & operator=(dword n) { x = rev(n); return *this; }
	operator dword() { return rev(x); }
};

class m_word{
	word	x;
public:
	word rev(word n) {
		return ((n & 0x00ff) << 8) +
			((n & 0xff00) >> 8);
	}
	m_word() { x = 0; }
	m_word(word n) { x = rev(n); }
	m_word & operator=(word n) { x = rev(n); return *this; }
	operator word() { return rev(x); }
};

class map{
public:
	char	desc[16];
	m_dword	pos;
	m_dword	x;
	byte	type;
	m_word	n;
	byte	d1;
	dword	d2;
};

class area_desc{
public:
	char	name[48];
	m_dword	pos;
	m_dword	len;
	byte	d1;
	byte	instr_group;
	word	d2;
	dword	d3;
};

class instrument_desc{
public:
	char	name[32];
};

#pragma pack(pop)

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

map 	dir[3];
area_desc sound_dir[512];
instrument_desc	instr_dir[512];
char	spaces[] = "                                                            ";

class hdinfo{
public:
	int		areas;
	char	path[256];
};

hdinfo	hdimages[64];
int		hdimage_ndx = 0;

void __fastcall TForm1::LoadCDimgClick(TObject *Sender)
{
	OpenDialog->Title = "Select CD image file to load";
	if(! OpenDialog->Execute())
		return;

	int h = open(OpenDialog->FileName.c_str(), O_RDONLY | O_BINARY);
	if(h == -1){
		Application->MessageBox("Can't open", "No such file", MB_OK);
		return;
	}

	lseek(h, 0x100, SEEK_SET);
	read(h, dir, 3 * sizeof(dir[0]));
    char buff[64];
    sprintf(buff, "%ld", (dword) dir[0].pos);
    igrpp->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[0].n);
    igrpn->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[1].pos);
    sdirp->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[1].n);
    sdirn->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[2].pos);
    mip->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[2].n);
    min->Caption = buff;

	lseek(h, dir[1].pos * 0x200, SEEK_SET);
	read(h, sound_dir, sizeof(area_desc) * dir[1].n);
	int i;
	Areas->Clear();
	for(i = 0; i < dir[1].n; i++){
        strncpy(buff, sound_dir[i].name, sizeof(sound_dir[0].name));
		buff[sizeof(sound_dir[0].name)] = '\0';
		Areas->Items->Add(buff);
	}
	lseek(h, dir[0].pos * 0x200, SEEK_SET);
	instrument_desc *instr_dir = new instrument_desc[dir[0].n];
	read(h, instr_dir, sizeof(instrument_desc) * dir[0].n);
	Groups->Clear();
	for(i = 0; i < dir[0].n; i++){
        strncpy(buff, instr_dir[i].name, sizeof(instr_dir[0].name));
		Groups->Items->Add(buff);
	}
	close(h);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::GroupsClick(TObject *Sender)
{
	int i;
	int n = Groups->ItemIndex;
	for(i = 0; i < Areas->Items->Count; i++){
		if(sound_dir[i].instr_group == n)
			Areas->Selected[i] = true;
		else
			Areas->Selected[i] = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AreasClick(TObject *Sender)
{
	int n = Areas->ItemIndex;
	Groups->ItemIndex = sound_dir[n].instr_group;

	Patches->Clear();
	Tones->Clear();

	int hdimg = open(hdimages[n / 64].path, O_RDONLY | O_BINARY);
	if(hdimg == -1){
		Application->MessageBox("Can't open", "No such file", MB_OK);
		return;
	}
	lseek(hdimg, 0x200 * sound_dir[n].pos, SEEK_SET);
	char descr[13];
	descr[12] = '\0';
	for(int i = 0; i < 16; i++){
		read(hdimg, descr, 12);
		lseek(hdimg, 256 - 12, SEEK_CUR);
		Patches->Items->Add(descr);
	}

	lseek(hdimg, 512, SEEK_CUR);
	descr[8] = '\0';
	for(int i = 0; i < 32; i++){
		read(hdimg, descr, 8);
		lseek(hdimg, 128 - 8, SEEK_CUR);
		Tones->Items->Add(descr);
	}
	close(hdimg);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::GroupsDragOver(TObject *Sender, TObject *Source,
	  int X, int Y, TDragState State, bool &Accept)
{
	int group;
	if((group = Groups->ItemAtPos(TPoint(X, Y), true)) == -1){
		Accept = false;
		Groups->DragCursor = crNoDrop;
		return;
	}
	for(int i = 0; i < Areas->Items->Count; i++){
		if(Areas->Selected[i]){
			sound_dir[i].instr_group = group;
		}
	}
	Groups->ItemIndex = group;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AddGroupClick(TObject *Sender)
{
    Groups->Items->Add(NewGroup->Text);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AddareaClick(TObject *Sender)
{
    Areas->Items->Add(NewArea->Text);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AddHDImgClick(TObject *Sender)
{
	OpenDialog->Title = "Select HD image file to add";
	if(! OpenDialog->Execute())
		return;

	strcpy(hdimages[hdimage_ndx].path, OpenDialog->FileName.c_str());
	HDImages->Items->Add(hdimages[hdimage_ndx].path);

	int h = open(hdimages[hdimage_ndx].path, O_RDONLY | O_BINARY);
	if(h == -1){
		Application->MessageBox("Can't open", "No such file", MB_OK);
		return;
	}
	lseek(h, 0x020, SEEK_SET);
	read(h, dir, 2 * sizeof(dir[0]));
    char buff[64];
    sprintf(buff, "%ld", (dword) dir[0].pos);
    igrpp->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[0].n);
    igrpn->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[1].pos);
    sdirp->Caption = buff;
    sprintf(buff, "%ld", (dword) dir[1].n);
    sdirn->Caption = buff;

	int areas = dir[1].x * 0x200 / sizeof(area_desc);
	hdimages[hdimage_ndx].areas = areas;

	int i;
	int nextarea = 0;
	for(i = 0; i < hdimage_ndx; i++)
		nextarea += hdimages[i].areas;

	lseek(h, dir[1].pos * 0x200, SEEK_SET);
	read(h, &sound_dir[nextarea], sizeof(area_desc) * areas);

	for(i = 0; i < areas; i++){
		strncpy(buff, sound_dir[nextarea + i].name, sizeof(sound_dir[0].name));
		buff[sizeof(sound_dir[0].name)] = '\0';
		Areas->Items->Add(buff);
	}
	close(h);

	hdimage_ndx++;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AreasDblClick(TObject *Sender)
{
	NewArea->Text = Areas->Items->Strings[Areas->ItemIndex];
}
//---------------------------------------------------------------------------
void __fastcall TForm1::GroupsDblClick(TObject *Sender)
{
	NewGroup->Text = Groups->Items->Strings[Groups->ItemIndex];
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ChangeGroupClick(TObject *Sender)
{
	Groups->Items->Strings[Groups->ItemIndex] = NewGroup->Text;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ChangeAreaClick(TObject *Sender)
{
	Areas->Items->Strings[Areas->ItemIndex] = NewArea->Text;
}
//---------------------------------------------------------------------------
char cdheader[512] = {
	"* ROLAND S-550 *"
};

void __fastcall TForm1::SaveCDimgClick(TObject *Sender)
{
	SaveDialog->Title = "Select new image file to save";
	if(! SaveDialog->Execute())
		return;

	int h = open(SaveDialog->FileName.c_str(), O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
	if(h == -1){
		Application->MessageBox("Can't create", "Error", MB_OK);
		return;
	}

	write(h, cdheader, sizeof(cdheader));

	lseek(h, 0x100, SEEK_SET);

	strncpy(dir[0].desc, "InstrumentGroup", 16);
	dir[0].pos = 0xf0;
	dir[0].n = Groups->Items->Count;
	dir[0].x = 15;
	dir[0].type = 0x48;
	dir[0].d1 = 0xff;
	dir[0].d2 = 0xffffffff;

	strncpy(dir[1].desc, "SoundDirectory ", 16);
	dir[1].pos = 0x01;
	dir[1].n = Areas->Items->Count;
	dir[1].x = 1 + (Areas->Items->Count * sizeof(area_desc)) / 512;
	dir[1].type = 0x40;
	dir[1].d1 = 0xff;
	dir[1].d2 = 0xffffffff;

	strncpy(dir[2].desc, "InstrumentGroup", 16);
	dir[2].pos = 0xff;
	dir[2].n = Groups->Items->Count;
	dir[2].x = 1;
	dir[2].type = 0x48;
	dir[2].d1 = 0xff;
	dir[2].d2 = 0xffffffff;

	write(h, dir, 3 * sizeof(dir[0]));

	dword hdpos = 0x100;		        // block #
	int i;
	int ndx = 0;
	word table[256];

	table[ndx] = 0;
	for(i = 0; i < Areas->Items->Count; i++){
		strncpy(sound_dir[i].name, spaces, 48);
		strncpy(sound_dir[i].name, Areas->Items->Strings[i].c_str(), 48);
		sound_dir[i].pos = hdpos;
		sound_dir[i].len = 0x6d2;
		hdpos += 0x6d4;
		sound_dir[i].d1 = 0x41;
		sound_dir[i].d2 = 0xffff;
		sound_dir[i].d3 = 0xffffffff;
		if(sound_dir[i].instr_group != sound_dir[table[ndx]].instr_group)
			table[++ndx] = i;
	}
	table[++ndx] = i;

	lseek(h, dir[1].pos * 0x200, SEEK_SET);
	write(h, sound_dir, sizeof(area_desc) * dir[1].n);

	for(i = 0; i < Groups->Items->Count; i++){
		strncpy(instr_dir[i].name, spaces, 32);
		strncpy(instr_dir[i].name, Groups->Items->Strings[i].c_str(), 32);
	}

	lseek(h, dir[0].pos * 0x200, SEEK_SET);
	write(h, instr_dir, sizeof(instrument_desc) * dir[0].n);

	for(; ndx < 256; ndx++)
		table[ndx] = -1;
	lseek(h, dir[2].pos * 0x200, SEEK_SET);
	write(h, table, sizeof(table));

	byte *ptr = (byte *) malloc(1024 * 1024);
	if(ptr == NULL){
		Application->MessageBox("Not enough memory", "Error", MB_OK);
		close(h);
		return;
	}

	for(int j = 0; j < hdimage_ndx; j++){
		int hdimg = open(hdimages[j].path, O_RDONLY | O_BINARY);
		if(hdimg == -1){
			Application->MessageBox("Can't open", "No such file", MB_OK);
			return;
		}
		lseek(hdimg, 0x14f8 * 0x200, SEEK_SET);
		for(i = 0; i < 64; i++){
			read(hdimg, ptr, 0x6d2 * 0x200);
			write(h, ptr, 0x6d2 * 0x200);
			write(h, &sound_dir[i], 0x400);
		}
		close(hdimg);
	}

	free(ptr);
	close(h);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GenerateClick(TObject *Sender)
{
	int i, j, k;

	for(j = 0; j < hdimage_ndx; j++){
		int hdimg = open(hdimages[j].path, O_RDONLY | O_BINARY);
		if(hdimg == -1){
			Application->MessageBox("Can't open", "No such file", MB_OK);
			return;
		}
		for(i = 0; i < 64; i++){
			lseek(hdimg, 0x200 * sound_dir[j * 64 + i].pos, SEEK_SET);
			char descr[49];
			descr[48] = '\0';
			lseek(hdimg, 0x1200, SEEK_CUR);
			for(int i = 0; i < 6; i++){
				read(hdimg, descr + i * 8, 8);
				lseek(hdimg, 128 - 8, SEEK_CUR);
			}
			Areas->Items->Strings[j * 64 + i] = descr;
		}
		close(hdimg);
	}
}
//---------------------------------------------------------------------------


