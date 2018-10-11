$!------------------------------------------------------------------------
$! MAKEFILE.COM
$!------------------------------------------------------------------------
$  set noverify
$! Check your system
$!
$  arch_name=f$getsyi("ARCH_NAME")
$  WRITE SYS$OUTPUT "System is ''arch_name'."
$  WRITE SYS$OUTPUT "-------------------------------------"
$  WRITE SYS$OUTPUT "Compiling sources files ..."
$!
$  IF arch_name .EQS. "VAX"
$  THEN
$      WRITE SYS$OUTPUT "Compiling file TAIL.C on VAX ..."
$      cc/nodebug TAIL.C
$  ELSE
$      WRITE SYS$OUTPUT "Compiling file TAIL.C on ALPHA ..."
$      cc/nodebug/define=__alpha TAIL
$  ENDIF
$!
$  WRITE SYS$OUTPUT "Linking TAIL.EXE ..."
$  link/nodebug/notraceback TAIL
$!
$! - Clean directory -----------------------------------------------------
$  WRITE SYS$OUTPUT "Cleaning DIRECTORY ..."
$! - Check if exixst *.obj and *.exe -----
$  IF F$SEARCH("*.OBJ") .EQS. ""
$  THEN
$      WRITE SYS$OUTPUT "->>> There is nothing to DELETE ..."
$  ELSE
$      WRITE SYS$OUTPUT "->>> Deleting *.OBJ files ..."
$      DELETE *.OBJ;*
$  ENDIF
$  IF F$SEARCH("TAIL.EXE") .EQS. ""
$  THEN
$      WRITE SYS$OUTPUT "->>> There is nothing to PURGE ..."
$  ELSE
$      WRITE SYS$OUTPUT "->>> Rurging TAIL.EXE files ..."
$      PURGE TAIL.EXE
$  ENDIF
$!------------------------------------------------------------------------
$ EXIT
$!------------------------------------------------------------------------
