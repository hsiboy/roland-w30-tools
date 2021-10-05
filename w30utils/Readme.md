This set of files contains the source code, VC++ 1.5, makefiles and built executable of for the following 5 utilities:

1. tonedump : extract and dumps some parameters of a given tone on a W30 disk
2. wdir : lists all the tone on a W30 disk
3. wget : extract a tone's wave from a W30 disk and convert it into .wav format. The compiled version will save 16bits sounds only. To save 8bits sounds, recompilation is needed and the keyword USE8BIT should be #defined.
4. winit : writes the information needed on a blank disk to make it usable for the W30. For more quick than the W30's format if you have pre-formatted disks
5. wput : convert a .wav soundfile into a W30 tone. Supports stereo file transfer.

Hope you'll enjoy them. There are already some troubles I'm already aware of. Leave me a mail if you have any suggestion/comments.

Marc Resibois (mr@nit.be) 100126,1007

---

Hi Folks,

During the holidays, I did some work in order to ehance the W30 to PC transfert utilities I had. Their current status is the following :

wdir : lists all the tones on a W30 disk.
tonedump : lists (some of) the parameters of a given tone ( merely for debugging/example)
winit : inits a formatted disk so that it is recognized by the W-30.
wget : extracts a tone from a W30 disk and write it into .wav format.
( 8/16 bits output now supported ).
wput !!! : converts a .wav files and write it as a W30 tone. Supports stereo files.

This means that you can now use your PC to edit/transfer W-30 waves. It is something I'm wanting to have since 8 years (?).

There is of course improvement that has to be done but I'll post this first draft. My intention is to put everything ( source code / executables ) on the lotus server but unfortunately I'm off the list and misses the administrator's address. So if anyone could send it to me, i'll be glad to send him the whole bunch of things.

Greetings,

-+-> Marc. <mr@nit.be> <100126,1007>
