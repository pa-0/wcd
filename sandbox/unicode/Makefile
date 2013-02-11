OBJ= warg.o
PROGRAM = warg

all : $(PROGRAM) wcwidth

%.o : %.c
	gcc -c -std=gnu99 $< -o $@

$(PROGRAM) : $(OBJ)
	gcc $(OBJ) -o $@

wcwidth : wcwidth.o
	gcc $< -o $@

clean :
	rm -rf *.bak *.exe *~ *.o $(PROGRAM)
