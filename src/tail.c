/*****************************************************************************

TAIL - gets the last "n" records from a file.

V2.4 - 09/10/91

Author:
D. Shepperd, Atari Games, Corp. shepperd@dms.UUCP

This software is supplied with no warranties expressed or implied. Use at
your own risk. Neither Atari Games, Corp. nor I can be held liable for any
loss of data or productivity as a result of using this program. Should any
of your users be caught or killed, the secretary will disavow any knowledge
of your actions. This software may be copied in whole or in part without
restriction.

To build this program:
	$ CC TAIL
	$ LINK TAIL

It was developed with Vax C 3.0 under VMS 5.1-1 although it ought to work
with earlier and later versions of VMS. (The latest version was developed
under VMS 5.4-2 and C 3.0). Since it uses VMS's RMS for file I/O, it is not
the least bit portable.

To run this program: Create a foreign command:

   $ TAIL :== $dev:[dir]TAIL

and type:

   $ TAIL [/number] [/?] [/S] [/F] [/T secs] input_file(s) [output_file]

where:
   [] denotes optional parameters.
   /? - prints a help screen.
   /number - indicates the number in decimal of records to output.
   /S - forces TAIL to use the "safe" way.
   /F - monitors the input file and outputs records as they show up.
   /T secs - sets the monitor update rate in seconds (forces a /F).

The /F option sets a display rate of 5 seconds. The /T option will force a
/F and sets the display rate to the number of seconds specified. Type
<return>  or some other terminator character on the terminal to advance to
the next  file if wildcards are used on input and the /F option is selected.
If  monitoring batch .LOG files, you will probably want to correlate the
monitor  rate with the batch log's SET OUTPUT_RATE parameter (normally 1
minute).

You can use "-" in place of "/" to delimit options.

The defaults applied to the input file spec are "SYS$DISK:[].LOG" and the
output file defaults to SYS$OUTPUT. The input file may contain wildcards,
however, if you use wildcards on the input filename AND supply an output
file name, only the first file in the wildcard list will be put into the
specified output file. The rest will be output to SYS$OUTPUT.

The /S can be used to force TAIL to read the file(s) from the beginning
rather than from the end. You would use /S if, for some reason, TAIL screws
up trying to figure out the record structure on variable  length files but
for some reason doesn't notice.

Design goal:

Make a TAIL utility that can very quickly pickup the last n records of a
batch log file (which tend to be tens of thousands of blocks long for some
of our jobs).

How it works (more or less): For fixed length records, TAIL just seeks to
the desired record and dumps the file from there. For stream record formats,
it reads from the end of the file and counts backwards the appropriate
number of terminators, then seeks to that point and dumps the file from
there. For variable and VFC record formats, it has two modes. The default
mode is for it to seek to the end of the file and read backwards trying to
determine record boundaries based upon some reasoning. This will usually
work if the records are relatively short and there are few (preferrably  no)
control characters in it. TAIL trys to figure out if it is getting screwed
up, and sometimes it may, and will fall back to the "safer" mode if so. It
may not notice, however, so you can force it to use the "safer" mode with
the /S option. The "safer" mode is where it reads the file from the
beginning, recording the record file address (rfa) of each record as it goes
(modulo the number of desired lines). When it reaches the end of file, it
backs up through the rfa table the desired number of lines, seeks to that
point and dumps the file from there. It always uses the "safer" mode on
input files less than 128 blocks in size. In the "safer" mode, it doesn't
use RMS $GET calls for each record. Instead it reads up to 127 blocks of the
file at a time then decodes the record structure within that 64K of data.
This results in a substantially faster access to the end of the file than
would normally be  available with successive $GET calls. Monitor mode starts
the same way, however, once the record structure has been determined it does
successive $GET calls  to obtain the records added to the end of the input
file. Due to a "feature" of RMS, the input file has to be closed and
reopened in order to gain access to data added to the end of an already
opened file. Although this undoubtly adds overhead to the system (closing
and opening the file every few seconds) TAIL maintains the rfa of the last
record read, so it can seek to that record to get quick access to the "new"
records at the new end of the file.

BUGS:

Nothing major has cropped up since the image was released in 4/90. Since
this  release has so many new features, it has at least an equal number of
areas of  potential bugdom. Beware. Besides these new features, there
remains the old tried an true (but yet to happen to me) small possibility
that it could seek to a point in a variable length file that RMS doesn't
like (only while it is NOT using the "safer" mode). This doesn't seem likely
because I believe that RMS should like whatever record structure TAIL
decodes even if it is not the correct one. Should TAIL seek to a point in
the file that RMS doesn't like, however, one of several things might occur.
Probably RMS will return an "invalid record format" error. There is a slim
chance that RMS might bugcheck which would not be a nice thing to have
happen to you and if your SYSGEN option BUGCHECKFATAL=1, your system would
crash. If BUGCHECKFATAL=0, your process would be deleted. I suggest you keep
BUGCHECKFATAL=0 and use some care if TAIL'ing binary files (or chicken out
and always use /S on binary files).

Does not treat: <dir>file, only: [dir]file.

This program is getting to damn big and I can't remember how the hell it
works.

Areas for improvment:

Currently the monitor mode does a double read of the tail end of the file
via multiple $GET's. This could be optimized by having the normal block mode
reader start where the last_rfa pointed and doing a normal "safer" way
search from there. This would result in a performance improvment if the
file grows by at least dozens of records between monitor mode samples. If
the file grows by less then a screen full of records between samples, then
changing this probably wouldn't make any difference.

Use async I/O and double buffering during the safe mode reads.
This might speed up the reading a little (might reduce spindle rotational
latency delays). Probably wouldn't make any difference on files accessed
via LAVC or DECnet.

Accept default values via an environment variable or logical name.

*****************************************************************************

Edit History:
	091091_DMS v2.4 Added a -f option (monitor) and set the default
   			number of lines to the terminal's page size. Added
   			some additional comments. This release has lots of
   			BIG changes so there's plenty of room for bugs.
   			And, as luck would have it, plenty were found.

	041790_DMS v2.3 Didn't notice the default input file spec had been
   			changed to *.* from .LOG; so I changed it back.

	041490_DMS v2.2 Made it only output a filename if there was more
   			than 1 input file. Only did highlighting if
   			outputting to tty, made "maxargs" adjust dynamically
			instead of being #defined. Put in a help screen.
			Posted this version to usenet 04/14/90.

	031290_KAR v2.1 Kjell Arne Rekaa - EB COMDATA, NORWAY: Corrected
   			minor error: One line less than specified with
   			/n was displayed. Accepts wildcards on filespec.
		        Filename displayed prior to each file.

	021790_DMS v2.0 added the "read backwards" algorithm for both
   			stream and variable rfm's.

	010590_DMS v1.0	image release

***************************************************************************/


#ifdef __alpha
#include <descrip.h>
#include <stdio.h>
#include <stdlib.h>
#include <rms.h>
#include <string.h>
#include <dvidef.h>
#include <dcdef.h>
#include <iodef.h>
#include <msgdef.h>
#include <ssdef.h>

int lib$find_file_end(),LIB$FIND_FILE(),sys$qio(),que_ttiread(),sys$wake(),
        sys$canwak(),sys$read(),sys$get(),sys$put(),sys$close(),
        sys$display(),sys$hiber(),sys$schdwk(),do_stream(),do_var(),
        do_fixed(),skip_through(),sys$create(),sys$connect(),sys$open(),
        que_ttiread(),sys$assign(),fgen(),toupper(),sys$getdviw();

#else
#include <descrip.h> 		 	/* descriptor details */
#include <stdio.h>			/* for printf and stuff */
#include <rms.h>			/* for the "real" I/O stuff */
#include <dvidef.h>			/* stuff for the GETDVI ss */
#include <dcdef.h>			/* device classes */
#include <iodef.h>			/* QIO stuff */
#include <msgdef.h>			/* mailbox stuff */
#include <ssdef.h>			/* completion codes */
char *malloc(),*strrchr();
int lib$find_file_end(),strncpy(),exit(),realloc(),
        LIB$FIND_FILE(),strlen(),sys$qio(),que_ttiread(),sys$wake(),
        sys$canwak(),sys$read(),calloc(),sys$get(),sys$put(),sys$close(),
        sys$display(),sys$hiber(),sys$schdwk(),do_stream(),do_var(),
        do_fixed(),skip_through(),sys$create(),sys$connect(),sys$open(),
        que_ttiread(),sys$assign(),fgen(),toupper(),sys$getdviw();
#endif
struct FAB inp_fab;		/* Input fab/rab/xab */
struct RAB inp_rab;
struct XABFHC inp_xab;

struct FAB out_fab;		/* Output fab/rab */
struct RAB out_rab;

typedef struct {		/* define an item list struct */
   short length;		/* length of buffer */
   short code;			/* item code */
   void *ptr;			/* ptr to buffer */
   void *retlen;		/* ptr to return length */
} Item;

typedef struct {		/* Genernic IOSB struct */
   short status;
   short length;
   long devdepend;
} Iosb;

typedef struct {
   unsigned long block;		/* block # */
   unsigned short offset;	/* offset within block */
   unsigned short length;	/* record length */
} Rfa;

typedef struct {		/* VMS 64 bit quadword time */
   long lsb;
   long msb;
} VMSTime;

#ifdef __alpha
char inp_buf[65536-512]; /* 127 blocks of buffer space */
#else
char inp_buf[65536-512]; /* 127 blocks of buffer space */
#endif
int the_safe_way=0;		/* If != then read var files front to back */
int monitor;			/* if != then loop on display of tail */
int sec=5;			/* delay time in seconds */
int tti_chan;			/* channel to use to look for stop code */
char tti_text[8];		/* room for terminal input */
Iosb tti_iosb;			/* IOSB for terminal input */
int tti_class;			/* SYS$INPUT device class */
int tto_class;			/* SYS$OUTPUT device class */
int tto_page;			/* SYS$OUTPUT page size (if tto_class == DC$_TERM) */
int were_done;			/* if !=, signals monitor complete */
int last_rfa_blk;		/* saved rfa of last record read */
int last_rfa_off;

char default_string[] = "SYS$DISK:[].LOG"; /* default input filename */

Item tt_dvi[] = {		/* An item list used to get SYS$xxx class */
   {4,DVI$_DEVCLASS,&tto_class,0},
   {4,DVI$_TT_PAGE,&tto_page,0},
   {0,0,0,0}
};

$DESCRIPTOR(sysin,"SYS$INPUT");
$DESCRIPTOR(sysout,"SYS$OUTPUT");

/* rfa stands for "record file address" */

Rfa *rfas;			/* ptr to array of rfa structs */
int record_count=23;		/* number of records to output */
int	maxargs;		/* records size of next_file array */
char **next_file;		/* pts to array of char ptrs to filenames */
VMSTime delay = {-5*10*1000*1000, /* 64 bit VMS delta time format for monitor timer... */
   		 -1};		/* ...initialized to 5 seconds */

char *mini_help_msg[] = {
   "TAIL version 2.4, 09/10/91. D. Shepperd, shepperd@dms.UUCP\n",
   "Usage: TAIL [/record_count] [/S] [/F] [/T secs] input_file [output_file]\n",
   0
};

char *help_msg[] = {
   "where: \"[]\" indicates optional data\n",
   " \"/record_count\" is decimal number of records desired\n",
   " \"/S\" indicates to use the \"safer\" mode\n",
   " \"/F\" monitor tail end of file (5 second sample rate)\n",
   " \"/T secs\" monitor tail end of file with sample rate of \"secs\"\n",
   " \"input_file\" is the input filename (can have wildcards)\n",
   " \"output_file\" is output filename (bogus if wildcards on input)\n",
   "Note that a \"-\" can be used in place of the \"/\" to delimit options.\n",
   "White space is required between all arguments (including the /T and secs).\n",
   "Options may appear in any order, but all must preceed filename(s).\n",
   0
};


void mini_help()
{
   char **s;
   for (s=mini_help_msg;*s;++s) fputs(*s,stderr); /* show mini help */
   return;
}

void show_help()
{
   char **s;
   mini_help();
   for (s = help_msg;*s;++s) fputs(*s,stderr);	/* display help message */
   return;
}

int main(int argc,char *argv[])
{
   int param;			/* Parameter counter */
   int err;			/* place to hold rms errors */
   int rfm;			/* loaded with record format code */
   int i, files;

   err = sys$getdviw(0,0,&sysin,&tt_dvi,0,0,0,0);
   if ((err&1) == 0) return err;
   tti_class = tto_class;

   err = sys$getdviw(0,0,&sysout,&tt_dvi,0,0,0,0);
   if ((err&1) == 0) return err;
   if (tto_class == DC$_TERM) record_count = tto_page > 2 ? tto_page-1 : 2;

   inp_fab = cc$rms_fab;	/* init the input fab/rab/xab */
   inp_fab.fab$b_shr = FAB$M_GET|FAB$M_PUT|FAB$M_UPI;
   inp_fab.fab$b_fac = FAB$M_GET|FAB$M_BRO;
   inp_rab = cc$rms_rab;
   inp_rab.rab$l_ubf = inp_buf;
   inp_rab.rab$w_usz = sizeof(inp_buf);	/* assume to read the max */
   inp_xab = cc$rms_xabfhc;
   inp_rab.rab$l_fab = &inp_fab;	/* tell rab where fab is */
   inp_fab.fab$l_xab = (char *)&inp_xab;	/* tell fab where xab is */
   inp_fab.fab$l_dna = default_string;	/* say input file defaults */
   inp_fab.fab$b_dns = sizeof(default_string)-1;

   param = 1;			/* start looking at argv[1] */
   --argc;			/* skip the image name */
   while(1) {			/* get all the command options */
      char c,*s;
      if (argc < 1) break;
      s = argv[param];
      c = *s++;
      if (c == '/' || c == '-') {
	 c = *s++;
	 c = toupper(c);
	 switch (c) {
	    int secs;
	    case '?':
	    case 'H':
	       show_help();
	       return 0x10000003;
	    case 'S':		/* safer mode */
	       the_safe_way = 1;
	       break;
	    case 'T':		/* set monitor rate in seconds */
	       if (*s == 0) {
		  if (--argc < 1) {
		     fputs("Missing delay time parameter\n",stderr);
		     mini_help();
		     return 0x10000002;
		  }
		  s = argv[++param];
	       }
	       if (sscanf(s,"%d",&secs) != 1 || secs <= 0 || secs > 2000) {
		  fprintf(stderr,"Invalid delay time parameter: %s\n",s);
		  fputs("Time value must be between 1 and 2000 secs\n",stderr);
		  mini_help();
		  return 0x10000002;
	       }
	       delay.lsb = -secs*10*1000*1000;
	    /* fall through to /F to default to monitor mode if /T specified */
	    case 'F':		/* set monitor mode */
	       monitor = 1;
	       break;
	    default:		/* assume the param is a record count */
	       --s;		/* backup to the first char of the record count */
	       if (sscanf(s,"%d",&record_count) != 1 || record_count <= 0) {
		  fprintf(stderr,"Invalid record count parameter: %s\n",s);
		  mini_help();
		  return 0x10000002;
	       }
 	 }
	 ++param;
	 --argc;
      } else {
	 break;
      }
   }
   record_count++; 	/* Fix of wrong record_count. KAR - 6-mar-1990 */
   if (argc < 1) {
      show_help();
      exit(0x10000003);
   }
   files = fgen(argv[param], &next_file, &maxargs);	/* deal with wildcards */
   if (files < 1) {
      fputs("No input file(s) found\n",stderr);
      return 0x10000002;
   }
   if (monitor) {
      if (tti_class != DC$_TERM) {
	 fputs("Cannot monitor files if SYS$INPUT is not a terminal\n",stderr);
	 monitor = 0;
      } else {
	 err = sys$assign(&sysin,&tti_chan,0,0);
	 if ((err&1) == 0) {
	    fputs("Error assigning channel to SYS$INPUT, monitor mode disabled\n",stderr);
	    monitor = 0;
	 }
	 que_ttiread();		/* que up a read to the terminal */
      }
   }
   for (i = 0; i < files; i++) {
      if (files > 1) {
	 if (tto_class == DC$_TERM) {
	    printf("\r\n         \033[7m**************** %s ****************\033[0m",
		     next_file[i]);
	 } else {
	    printf("\r\n	**************** %s ****************",
		     next_file[i]);
	 }
      }
      inp_fab.fab$l_fna = next_file[i];	/* input filename is next param */
#ifdef __alpha
      inp_fab.fab$b_fns = (int) strlen(next_file[i]);
#else
      inp_fab.fab$b_fns = strlen(next_file[i]);
#endif
      if (((err=sys$open(&inp_fab))&1) == 0) {
	 fputs("Error opening input file\n",stderr);
	 exit(err);
      }
      if (((err=sys$connect(&inp_rab))&1) == 0) {
	 fputs("Error connecting input rab\n",stderr);
	 exit(err);
      }
      if (inp_fab.fab$b_org != FAB$C_SEQ) {
	 char *oldtype="UNKNOWN";
	 if (inp_fab.fab$b_org == FAB$C_REL) oldtype = "RELATIVE";
	 if (inp_fab.fab$b_org == FAB$C_IDX) oldtype = "INDEXED";
	 if (inp_fab.fab$b_org == FAB$C_HSH) oldtype = "HASHED";
	 fprintf(stderr,"Input file organization is %s. This program only supports SEQUENTIAL.\n",
		  oldtype);
	 exit(0x10000002);
      }
      if (inp_fab.fab$b_rfm == FAB$C_UDF || inp_fab.fab$b_rfm > FAB$C_STMCR) {
	 fputs("Input file has undefined or unsupported record format.\n",stderr);
	 exit(0x10000002);
      }
      out_fab = cc$rms_fab;			/* init the output fab */
      out_fab.fab$b_bks = inp_fab.fab$b_bks;	/* make rest same as input */
      out_fab.fab$w_bls = inp_fab.fab$w_bls;
      out_fab.fab$w_deq = inp_fab.fab$w_deq;
      out_fab.fab$b_fac = FAB$M_PUT;
      out_fab.fab$b_fsz = inp_fab.fab$b_fsz;
      out_fab.fab$w_mrs = inp_fab.fab$w_mrs;
      out_fab.fab$b_rat = inp_fab.fab$b_rat;
      out_fab.fab$b_rfm = inp_fab.fab$b_rfm;
      out_rab = cc$rms_rab;
      out_rab.rab$l_fab = &out_fab;		/* tell rab where fab is */
      if (--argc >= 1) {
	 out_fab.fab$l_fna = argv[++param];	/* output filename is next param */
      } else {
	 out_fab.fab$l_fna = "SYS$OUTPUT:";	/* else default */
      }
      out_fab.fab$b_fns = strlen(out_fab.fab$l_fna);
      if (((err=sys$create(&out_fab))&1) == 0) {
	 fputs("Error opening output file\n",stderr);
	 exit(err);
      }
      if (((err=sys$connect(&out_rab))&1) == 0) {
	 fputs("Error connecting output rab\n",stderr);
	 exit(err);
      }
      rfm = inp_fab.fab$b_rfm;	/* pickup rfm code */
      last_rfa_blk = last_rfa_off = 0;	/* start at top of file */
      were_done = 0;
      while (1) {
	 long oldebk;
	 int oldffb;
	 oldebk = inp_xab.xab$l_ebk;	/* remember the old end of file mark */
	 oldffb = inp_xab.xab$w_ffb;
	 if (last_rfa_blk != 0) {		/* if we've already been through the file */
	    err = skip_through();	/* then just start where we left off */
	 } else {
	    switch (inp_fab.fab$b_rfm) {
	       case FAB$C_FIX:
		  err = do_fixed();
		  break;
	       case FAB$C_VAR:
		  err = do_var();
		  break;
	       case FAB$C_VFC:
		  err = do_var();
		  break;
	       case FAB$C_STM:
		  err = do_stream(2);
		  break;
	       case FAB$C_STMCR:
		  err = do_stream(1);
		  break;
	       case FAB$C_STMLF:
		  err = do_stream(0);
		  break;
	       default:
		  err = 0x10000004;
		  fputs("Unknown record format\n",stderr);
		  exit(err);
	    }
	 }
	 if ((err&1) == 0) {
	    if (err != RMS$_EOF) {
	       fputs("Error reading input\n",stderr);
	       exit(err);
	    }
	 }
	 if (!monitor || were_done == 1) break;
	 while (!were_done) {
	    sys$schdwk(0,0,&delay,0);
	    sys$hiber();
	    err = sys$display(&inp_fab);	/* see if stuff has been added to file */
	    if ((err&1) == 0) {
	       fputs("Error doing $DISPLAY on input, monitor mode cancelled\n",stderr);
	       were_done = 1;
	       break;
	    }
	    if (inp_xab.xab$l_ebk == oldebk && inp_xab.xab$w_ffb == oldffb) {
	       continue;			/* eof hasn't moved, continue waiting */
	    }
   /* This part is goofy. One would think that a simple sys$display would be all that */
   /* should be required, but nooooooo.... DEC has set some internal flags that will  */
   /* not let me read past the old end of file regardless of the fact the the eof has */
   /* moved. What's really goofy is that sys$display notices that the eof has moved   */
   /* but it won't change those internal flags. Closing/reopening the file every few  */
   /* seconds seems like an expensive proposition to me. Grrrr.			      */
	    err = sys$close(&inp_fab);	/* close the file */
	    if ((err&1) == 0) {
	       fputs("Error closing the input file\n",stderr);
	       exit(err);
	    }
	    err = sys$open(&inp_fab);	/* reopen the file */
	    if ((err&1) == 0) {
	       fputs("Error reopening the input file\n",stderr);
	       exit(err);
	    }
	    err = sys$connect(&inp_rab);	/* reconnect the rab */
	    if ((err&1) == 0) {
	       fputs("Error reconnecting the input rab\n",stderr);
	       exit(err);
	    }
	    break;
	 }
      }
      sys$close(&inp_fab);
      sys$close(&out_fab);
   }
   if ((err&1) != 0) return err;
   if (err != RMS$_EOF) return err;
   return SS$_NORMAL;
}

/**************************************************************************
 * do_seqout - sequentially output the data from the input file
 *
 * At entry:
 *	rfa_blk - starting block number
 *	rfa_off - offset within block to start of record
 * At exit:
 *	last_rfa_blk and last_rfa_off are set to the rfa of the last record read
 *	input file has been dumped to output
 **************************************************************************/

do_seqout(long rfa_blk,int rfa_off)	/* seek to desired record and output */
{
   int err,skip=0;
   if (rfa_blk == 0) rfa_blk = 1;	/* start at the beginning */
   if (rfa_blk < last_rfa_blk || (rfa_blk == last_rfa_blk && rfa_off <= last_rfa_off)) {
      rfa_blk = last_rfa_blk;			/* seek to record last displayed */
      rfa_off = last_rfa_off;
      skip = 1;				/* and skip it */
   }
   inp_rab.rab$l_rfa0 = rfa_blk;	/* starting block # */
   inp_rab.rab$w_rfa4 = rfa_off;	/* byte offset within block */
   inp_rab.rab$b_rac = RAB$C_RFA;	/* change to RFA access mode */
   inp_rab.rab$l_bkt = 0;		/* make sure bkt field is clear */
   inp_rab.rab$l_rhb = inp_buf;		/* init the ptrs */
   inp_rab.rab$l_ubf = inp_buf + inp_fab.fab$b_fsz; /* in case making VFC file */
   out_rab.rab$l_rhb = inp_buf;
   out_rab.rab$l_rbf = inp_buf + inp_fab.fab$b_fsz;
   while(1) {
      last_rfa_blk = inp_rab.rab$l_rfa0;	/* save rfa of last record read */
      last_rfa_off = inp_rab.rab$w_rfa4;
      err=sys$get(&inp_rab);		/* read the record */
      if ((err&1) == 0) {
	 if (err != RMS$_EOF) {
	    fputs("Error reading input\n",stderr);
	    exit(err);
	 }
	 break;
      }
      inp_rab.rab$b_rac = RAB$C_SEQ;	/* switch back to sequential reads */
      out_rab.rab$w_rsz = inp_rab.rab$w_rsz;
      if (skip == 0) {			/* if not to skip the record */
	 if (((err=sys$put(&out_rab))&1) == 0) { /* write it */
	    fputs("Error writing output\n",stderr);
	    exit(err);
	 }
      }
      skip = 0;				/* at most, we skip 1 record */
   }
   return err;
}

char end_mark[] = "\n\r\n";

/**************************************************************************
 * do_stream - figure out the record structure for one of the 3 types of
 *		stream files there are.
 * At entry:
 *	type - record type. 0=stmlf, 1=stmcr, 2=stmcrlf
 * At exit:
 *	has called do_seqout with the computed block and offset of the
 *	desired starting record.
 **************************************************************************/

do_stream(type)
int type;	/* 0=stmlf, 1=stmcr, 2=stmcrlf */
{
   unsigned long block,rfa_blk=0;
   int rcd_num= 0,err,rfa_off,part1=0,end_char;
#ifdef __alpha
   char *s;
#else
   char *s;
#endif
   inp_rab.rab$l_bkt = inp_xab.xab$l_ebk+1;
   end_char = end_mark[type];

   while(1) {				/* as long as there's data in the file */
      if (inp_rab.rab$l_bkt <= 1) {	/* quit if we already read blk 1 */
	 rfa_blk = 1;			/* give 'em the whole file */
	 rfa_off = 0;
	 break;
      }
      block = inp_rab.rab$l_bkt-(inp_rab.rab$w_usz>>9);
      if (block == 0 || block > inp_rab.rab$l_bkt) block = 1;	/* but can't start before 1 */
      inp_rab.rab$l_bkt = block;	/* rememebr starting block number */
      err = sys$read(&inp_rab);
      if ((err&1) == 0) {
	 if (err == RMS$_EOF) break;
	 fprintf(stderr,"Error (%08X) trying to read %d bytes at block %d\n",
	 	err,inp_rab.rab$w_usz,block);
	 continue;
      }
      s = inp_rab.rab$l_ubf+inp_rab.rab$w_rsz;

      if (part1) {
	 if (*--s == '\r') {
	    rfa_blk = block+((s+2)-inp_rab.rab$l_ubf>>9);
	    rfa_off = ((s+2)-inp_rab.rab$l_ubf)&511;
	    if (++rcd_num >= record_count) break;
	 }
	 ++s;
      }
      part1 = 0;
      while (s >= inp_rab.rab$l_ubf) {
	 if (*--s == end_char) {
	    char *nrp;
	    nrp = s+1;			/* next record starts here */
	    if (type == 2) {		/* if strmcrlf */
	       if (s <= inp_rab.rab$l_ubf) {	/* if on the cusp */
		  part1 = 1;		/* defer */
		  break;		/* get somemore data */
	       }
	       if (*--s != '\r') {	/* else if next char is not cr */
		  ++s;			/* then this is not a record */
		  continue;
	       }
	    }
	    rfa_blk = block+(nrp-inp_rab.rab$l_ubf>>9);
	    rfa_off = (nrp-inp_rab.rab$l_ubf)&511;
	    if (++rcd_num >= record_count) break;
	 }	
      }
      if (rcd_num >= record_count) break;	/* we got everything */
   }
   if (rfa_blk == 0) return RMS$_EOF;
   return do_seqout(rfa_blk,rfa_off);	/* write out records */
}

/************************************************************************
 * do_varfast. This procedure trys to figure out the record structure
 * of a variable length file while reading from the end of the file.
 *
 * At entry:
 *	(called by do_var)
 * At exit:
 *	returns a 0 if it couldn't determine a valid record structure.
 *	called do_seqout with a computed block and offset if it did
 *		successfuly find an appropriate record boundary.
 *	returns with RMS status in all cases.
 *************************************************************************/

do_varfast()
{
   unsigned long block,rfa_blk=0;
   int rcd_num= 0,err,rec_len,rfa_off,min_recsiz;
   union {
#ifdef __alpha
      char *s;
      short *len;
      int align;
#else
      char *s;
      short *len;
      int align;
#endif
   } rp;

   inp_rab.rab$l_bkt = inp_xab.xab$l_ebk+1;
   rec_len = 0;
   min_recsiz = inp_fab.fab$b_fsz;
   inp_rab.rab$w_usz = ((inp_xab.xab$w_lrl+3)*record_count+511)&~511;

   while(1) {
      if (inp_rab.rab$l_bkt <= 1) break; /* quit if we already read blk 1 */
      block = inp_rab.rab$l_bkt-(inp_rab.rab$w_usz>>9);
      if (block == 0 || block > inp_rab.rab$l_bkt) block = 1;	/* but can't start before 1 */
      inp_rab.rab$l_bkt = block;	/* remember starting block number */
      err = sys$read(&inp_rab);
      if ((err&1) == 0) {
	 if (err == RMS$_EOF) break;
	 fprintf(stderr,"Error (%08X) trying to read %d bytes at block %d, record %d\n",
	 	err,inp_rab.rab$w_usz,block,rcd_num);
	 continue;
      }
      rp.s = inp_rab.rab$l_ubf+inp_rab.rab$w_rsz;
      if ((rp.align&1) == 1) ++rp.align;	/* ffb must be even */

/*************************************************************************
 * The following procedure loops through the buffer picking up pairs of bytes (a
 * short) on even byte boundaries from the end. If this pair of bytes forms a
 * integer whose value points within 1 of the the next record or the current eof,
 * then the pair is assumed to be the count field of a valid record and is so
 * recorded. It can easily get screwed up if there is binary data in the records,
 * but for ascii files such as log files, it ought to work well enough most of
 * the time. The integer value represents the length of the record. If this value
 * is greater than the maxium record length recorded for the file (in the
 * XAB$W_LRL field of the XABFHC), then this routine assumes to have gotten lost,
 * so it rolls over and dies.
 *************************************************************************/

      while (1) {
	 int len,t;
	 char *tp;
	 rp.s -= 2;			/* backup 2 bytes */
	 if (rp.s < inp_rab.rab$l_ubf) break;	/* too far, get more data */
	 len = *rp.len;
	 if (rec_len >= min_recsiz && 	/* if currently at or greater than min */
	     (len == rec_len ||		/* and lengths match exactly */
   	      (rec_len > 0 &&		/* or less by 1 */
   	       len == rec_len-1))) {
	    rfa_blk = block+(rp.s-inp_rab.rab$l_ubf>>9); /* remember this point */
	    rfa_off = (rp.s-inp_rab.rab$l_ubf)&511;
	    rec_len = 0;
	    if (++rcd_num >= record_count) break;
	    continue;
	 }
	 rec_len += 2;			/* increase size of record */
	 if (rec_len > inp_xab.xab$w_lrl) return 0; /* we're lost, give up */
      }
      if (rcd_num >= record_count) break;	/* we got everything */
   }
   if (rfa_blk == 0) return RMS$_EOF;
   return do_seqout(rfa_blk,rfa_off);	/* write out records */
}

/************************************************************************
 * skip_through - this routine positions to the desired record begining
 * with a known starting position. It is used in monitor mode to skip
 * records that may have been added to the input file since the last
 * time it was looked at it.
 *
 * At entry:
 *	last_rfa_blk and last_rfa_off point to the last record read.
 * At exit:
 *	called do_seqout with a computed block and offset
 *	last_rfa_blk and last_rfa_off point to a new last record read.
 *	
 ************************************************************************/

int skip_through()
{
   int err;
   int first_rfa=0;		/* index to first rfa */
   int next_rfa=0;		/* index to next available rfa */

   if (rfas == (Rfa *)0) rfas = (Rfa *)calloc(record_count,sizeof(Rfa));
   inp_rab.rab$l_rfa0 = last_rfa_blk;	/* where we left off */
   inp_rab.rab$w_rfa4 = last_rfa_off;
   inp_rab.rab$b_rac = RAB$C_RFA;	/* change to RFA access mode */
   inp_rab.rab$l_bkt = 0;		/* make sure bkt field is off */
   inp_rab.rab$l_rhb = inp_buf;		/* init the ptrs */
   inp_rab.rab$l_ubf = inp_buf + inp_fab.fab$b_fsz; /* in case making VFC file */
   while (1) {
      (rfas+next_rfa)->block = inp_rab.rab$l_rfa0;
      (rfas+next_rfa)->offset = inp_rab.rab$w_rfa4;
      next_rfa += 1;
      next_rfa %= record_count;
      if (next_rfa == first_rfa) {
	 first_rfa += 1;
	 first_rfa %= record_count;
      }
      err=sys$get(&inp_rab);
      if ((err&1) == 0) {
	 if (err != RMS$_EOF) {
	    fputs("Error reading input\n",stderr);
	    exit(err);
	 }
	 break;
      }
      inp_rab.rab$b_rac = RAB$C_SEQ;	/* switch back to sequential */
   }
   return do_seqout((rfas+first_rfa)->block,(rfas+first_rfa)->offset);
}

/************************************************************************
 * do_var. This procedure positions to the nth record from the end of
 * a file by reading the whole file into memory 127 blocks at a time
 * and skipping through the records recording the block and offset of
 * each as it goes.
 *
 * At entry:
 *
 * At exit:
 *	called do_seqout with the computed block and offset
 *	last_rfa_blk and last_rfa_off updated to point to the last record read.
 *	returns with RMS status.
 *************************************************************************/

do_var()
{
   long block,rcd_num= 0;
   int err;
   int first_rfa=0;		/* index to first rfa */
   int next_rfa=0;		/* index to next available rfa */
#ifdef __alpha
   char *ebp;
#else
   char *ebp;
#endif
   union {
      char *s;
      unsigned short *len;
      unsigned int align;
   } rp;

/* Unless otherwise indicated, trys to do the fast way first. If that fails,
 * it'll do the hard way. */

   if (the_safe_way == 0 && inp_xab.xab$l_ebk > (inp_rab.rab$w_usz>>9)) {
      if ((err=do_varfast()) != 0) return err;
      fputs("Unable to determine record structure from backend of file.\n",stderr);
      fputs("Doing it the 'safer' way instead.\n",stderr);
      inp_rab.rab$w_usz = sizeof(inp_buf);	/* assume to read the max */
   }	
   rp.s = inp_rab.rab$l_ubf;
   inp_rab.rab$l_bkt = 1;
   if (rfas == (Rfa *)0) rfas = (Rfa *)calloc(record_count,sizeof(Rfa));
   while(1) {
      block = inp_rab.rab$l_bkt;
      err = sys$read(&inp_rab);
      inp_rab.rab$l_bkt += (inp_rab.rab$w_rsz+511) >> 9;
      if ((err&1) == 0) {
	 if (err == RMS$_EOF) break;
	 fprintf(stderr,"Error (%08X) trying to read %d bytes at block %d, record %d\n",
	 	err,inp_rab.rab$w_usz,block,rcd_num);
	 continue;
      }
      ebp = inp_rab.rab$l_ubf+inp_rab.rab$w_rsz;
      while (1) {
	 int len,t;
	 char *tp;
	 if ((rp.align&1) == 1) ++rp.align;
	 if (rp.s >= ebp) {
	    t = rp.s - ebp;
	    inp_rab.rab$l_bkt += t>>9;
	    rp.s = inp_rab.rab$l_ubf+(t&511);
	    break;
	 }
	 ++rcd_num;
	 len = *rp.len;
	 if (len > 32767) {
	    if (len == 0xFFFF) {
	       rp.s = inp_rab.rab$l_ubf;
	       break;			/* eof */
	    }
	    fprintf(stderr,"Warning: Record %d (blk=0x%08X,off=0x%04X): cnt %04X greater than 0x7FFF\n",
	 		rcd_num,(rp.s-inp_rab.rab$l_ubf>>9)+block,(rp.s-inp_rab.rab$l_ubf)&511,len);
	 }
	 t = rp.s-inp_rab.rab$l_ubf;
	 (rfas+next_rfa)->block = block + (t >> 9);
	 (rfas+next_rfa)->offset = t & 511;
	 (rfas+next_rfa)->length = len;
	 next_rfa += 1;
	 next_rfa %= record_count;
	 if (next_rfa == first_rfa) {
	    first_rfa += 1;
	    first_rfa %= record_count;
	 }
	 rp.s += len+2;
      }
   }
   if ((rfas+first_rfa)->block == 0) return RMS$_EOF;
   return do_seqout((rfas+first_rfa)->block,(rfas+first_rfa)->offset);
}

/************************************************************************
 * do_fixed. This procedure positions to the nth record from the end of
 * a fixed length record file by computing the desired rfa from the
 * required record number, the size of the file and the size of each
 * record. This is by far the simplest of the three decoding routines.
 *
 * At entry:
 *
 * At exit:
 *	called do_seqout with the computed block and offset
 *	last_rfa_blk and last_rfa_off updated to point to the last record read.
 *	returns with RMS status.
 *************************************************************************/

do_fixed()
{
   int reccnt;
   unsigned long size,sbn;
   long start,rfa_blk,rfa_off;

   size = inp_xab.xab$l_ebk*512+inp_xab.xab$w_ffb; /* file size in bytes */
   reccnt = size/inp_xab.xab$w_mrz;	/* total record count */
   start = reccnt - record_count;	/* starting record # */
   if (start < 0) start = 0;		/* can't go past beginning */
   sbn = start*inp_xab.xab$w_mrz;	/* starting byte # */
   rfa_blk = (sbn >> 9) + 1;		/* starting block number */
   rfa_off = sbn & 511;			/* offset in block */
   if (rfa_blk < last_rfa_blk || (rfa_blk == last_rfa_blk && rfa_off < last_rfa_off)) {
      rfa_blk = last_rfa_blk;		/* don't display records already displayed */
      rfa_off = last_rfa_off+inp_xab.xab$w_mrz; /* advance 1 record */
      if (rfa_off >= 512) {		/* and adjust pointers if appropriate */
	 rfa_off -= 512;
	 rfa_blk += 1;
      }
   }
   return do_seqout(rfa_blk,rfa_off);	/* write the end of the file */
}

int tti_ast()
{
   int sts;
   if (tti_iosb.status == SS$_ABORT) {
      return SS$_NORMAL;		/* aborts are ok, since they'll happen at exit */
   }
   if (tti_iosb.status == SS$_NORMAL || tti_iosb.status == SS$_ENDOFFILE) {
      were_done = 1;			/* signal that we're done */
      sys$canwak(0,0);			/* cancel our schwk */
      sys$wake(0,0);			/* wake up from our hiber */
   } else {
      if ((tti_iosb.status&1) == 0) {
	 fputs("Error reading from maillbox\n",stderr);
	 exit(tti_iosb.status);
      }
   }
   return que_ttiread();
}

int que_ttiread()
{
   int sts;
   sts = sys$qio(	0,		/* efn */
   			tti_chan,	/* channel */
   			IO$_READVBLK,	/* function */
   			&tti_iosb,	/* iosb addr */
   			tti_ast,		/* astadr */
   			0,		/* astparam */
   			tti_text,	/* p1 (buffer ptr) */
   			sizeof(tti_text), /* p2 (buffer size) */
   			0,0,0,0);	/* p3-p6 not used */

   if ((sts&1) == 0) {
      fputs("unable to do QIO to SYS$INPUT, monitor mode disabled\n",stderr);
      monitor = 0;
   }
   return sts;
}

/* ==============================fgen.c============================== */

/* #include <rms.h>  ...defined in the program top... */

/*
 * fgen(pattern, result_array, array_length)
 *   fgen generates filenames of files, matching a VMS pattern,
 *   the results are stored in an array, space for the strings is allocated
 *   by using malloc.
 * Return values:
 *   -1    : error in pattern
 *    0    : no files found
 *   n(>0) : number of matching filenames. (Stored in result_array [0] - [n-1])
 *
 * Wildcard expansion for VMS is easy;  we just use a run-time library call.
 */

fgen(pat,resarry,len)
char *pat,**resarry[];
int *len;
{
    struct dsc$descriptor_s file_spec, result, deflt;
    long context;
    int count, slen, status, plen;
    char *pp, *rp, result_string[256], *strchr();
    char *fnp,**fnpp;

    file_spec.dsc$w_length  = strlen(pat);
    file_spec.dsc$b_dtype   = DSC$K_DTYPE_T;
    file_spec.dsc$b_class   = DSC$K_CLASS_S;
    file_spec.dsc$a_pointer = pat;

    result.dsc$w_length  = sizeof result_string;
    result.dsc$b_dtype   = DSC$K_DTYPE_T;
    result.dsc$b_class   = DSC$K_CLASS_S;
    result.dsc$a_pointer = result_string;

    deflt.dsc$w_length  = sizeof(default_string)-1;
    deflt.dsc$b_dtype   = DSC$K_DTYPE_T;
    deflt.dsc$b_class   = DSC$K_CLASS_S;
    deflt.dsc$a_pointer = default_string;

    count = 0;
    context = 0;
    pp = strrchr(pat, ']');
    if ( !pp ) pp = strrchr(pat, ':');
    if ( !pp ) plen = 0;
    else plen = pp - pat + 1;
    fnpp = *resarry;
    while ((status = LIB$FIND_FILE(&file_spec, &result, &context, &deflt))
                                                              == RMS$_NORMAL) {
	if (count >= *len) {
	    if (*len == 0) *len = 256;
	    if (*resarry == (char **)0) {
		*resarry = (char **)malloc(*len*sizeof(char *));
	    } else {
		*len += *len/2;		/* increase length by 1/2 again */
		*resarry = (char **)realloc(*resarry,*len*sizeof(char *));
	    }
	    if (*resarry == (char **)0) {
		perror("Unable to malloc/realloc memory");
		exit(0x10000004);
	    }
	    fnpp = *resarry + count;
	}
        rp = strrchr(result_string, ']') + 1;
        if( !rp )
        	rp = result_string;
        slen = strchr(rp, ' ') - rp;
            fnp = *fnpp++ = malloc(slen + plen + 1);
        if (plen != 0)
            strncpy(fnp, pat, plen);
        strncpy(fnp + plen, rp, slen);
        fnp[slen + plen] = '\0';
        ++count;
    }
#ifdef DVI$_ALT_HOST_TYPE
    lib$find_file_end(&context);    /* Only on V4 and later */
#endif
    if (status == RMS$_FNF) return(0);
    if (status == RMS$_NMF) return(count);
    return(-1);
}
