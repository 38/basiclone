10 INPUT "]";STATEMENT$:GOSUB 100
20 __RESET_ERR:__EXECUTE STATEMENT$
25 IF STATEMENT$="SYSTEM" THEN END
30 IF __LASTERRNO<>0 THEN PRINT "?";__LASTERR$;" ERROR"
40 GOTO 10
100 TEMP$=""
110 FOR __I=1 TO LEN(STATEMENT$)
120 C$=MID$(STATEMENT$,__I,1)
130 IF C$>"z" OR C$<"a" THEN TEMP$=TEMP$+C$ ELSE TEMP$=TEMP$+CHR$(ASC(C$)-32)
140 NEXT __I
150 STATEMENT$=TEMP$
160 RETURN
