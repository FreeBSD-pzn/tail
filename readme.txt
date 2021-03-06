TAIL, Utilities, Performs a Variation of TYPE/TAIL on a File

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

--[.SRC]-------------------------------------------
To compile simple run
$ @MAKEFILE

-- Files ------------------------------------------
 TAIL.C1 - original source file which has been
           included into the FREEWAREV80
 TAIL.C  - source file to compile,
           solved some issues.
 *.HLP, *.DOC, *.HLB   - help and manual files
---------------------------------------------------
p.s.
I have kept TAIL, which I have found in the DECus archives:
https://www.digiater.nl/openvms/decus/