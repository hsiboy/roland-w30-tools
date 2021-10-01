//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TListBox *Areas;
    TButton *LoadCDimg;
    TListBox *Groups;
    TButton *AddGroup;
    TButton *AddHDImg;
    TMemo *NewGroup;
    TMemo *NewArea;
    TButton *ChangeGroup;
    TButton *ChangeArea;
    TButton *SaveCDimg;
    TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
    TListBox *HDImages;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TGroupBox *GroupBox3;
    TLabel *igrpp;
    TLabel *igrpn;
    TLabel *sdirp;
    TLabel *sdirn;
    TLabel *mip;
    TLabel *min;
	TGroupBox *GroupBox4;
	TGroupBox *GroupBox5;
	TListBox *Patches;
	TListBox *Tones;
	TButton *Generate;
    void __fastcall LoadCDimgClick(TObject *Sender);

    void __fastcall GroupsClick(TObject *Sender);
    void __fastcall AreasClick(TObject *Sender);
    void __fastcall GroupsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
    void __fastcall AddGroupClick(TObject *Sender);
    void __fastcall AddareaClick(TObject *Sender);
    void __fastcall AddHDImgClick(TObject *Sender);
    void __fastcall AreasDblClick(TObject *Sender);
    void __fastcall GroupsDblClick(TObject *Sender);
    void __fastcall ChangeGroupClick(TObject *Sender);
    void __fastcall ChangeAreaClick(TObject *Sender);
    void __fastcall SaveCDimgClick(TObject *Sender);
	void __fastcall GenerateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
