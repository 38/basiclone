CPPFLAGS=`sdl-config --cflags` -Wall
CFLAGS= -g 
LDFLAGS=`sdl-config --libs` -lSDL_ttf -lm
TARGET=basiclone
all:list $(TARGET)

list: preproc.o codeset.o memory.o mutex.o display.o io.o io_calls.o token.o builtin.o init.o expression.o statement.o list.o 
	$(CC) $^ $(LDFLAGS) -o $@
$(TARGET): preproc.o codeset.o memory.o mutex.o display.o io.o io_calls.o token.o builtin.o init.o expression.o statement.o main.o 
	$(CC) $^ $(LDFLAGS) -o $@
list.o: list.c codeset.h common.h preproc.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
preproc.o: preproc.c preproc.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
codeset.o: codeset.c codeset.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
memory.o: memory.c memory.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
mutex.o: mutex.c mutex.h common.h memory.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
display.o: display.c memory.h display.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
io.o: io.c memory.h io.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
io_calls.o: io_calls.c io_calls.h io.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
token.o: token.c token.h  common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
builtin.o: builtin.c builtin.h token.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
init.o: init.c init.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
expression.o: expression.c expression.h token.h common.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
statement.o: statement.c statement.h common.h expression.h token.h
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
main.o: main.c
	$(CC) -c  $(CPPFLAGS) $(CFLAGS) $< -o $@
clean:
	rm -f *.o
upload:
	svn commit
