# roland-w30-tools
Various utilities for the Roland W30 from Gabor Pikacs and Andor Kocsi

---


hi!

These are the programs I wrote for playing with my W30.
I am a hardware and software developer, the music is only at hobby level.
Some of you might be interested in the snakegame. I wrote it for
demonstrating to the Roland guys back then that I was really interested
in software development for the W30. They were simply ignoring me :(
I had to earn some money so I gave it up.

I think I should say something about copying and distributing:

 --- Feel free to use, modify, copy the pack ---.

If you want to try the game:
- format a SYSTEM disk in the W30
- run "coddisk snake.cod" from the command prompt
  (I assume you have both coddisk.exe and snake.cod)
  drive A: MUST be the 1.44 floppy!!!
- put the disk in the W30 and have fun.
This program is e-mail -ware, so I would be glad to have
feedback from everyone who tried the game to
	pikcsi@c3.hu

The rest of the programs are (for those who want to program the W30) :

|name | function|
------|----------
coddisk|;create "special" disks
ass|;i8096 assembler
feltesz|;program to put file onto a W30 disk
ibmw30|;ditto
leszed|;program to get a W30 disk into a file
mivan|;program to list wave data on a W30 disk
snake|;THE GAME :) [use coddisk]
dis|;i8096 disassembler


you can compile the .C sources with Borland/MSC/Watcom.
The comments in the sources are in Hungarian.
