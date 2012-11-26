OBJ= warg.o
PROGRAM = warg

all : $(PROGRAM)

%.o : %.c
	gcc -c -std=gnu99 $< -o $@

$(PROGRAM) : $(OBJ)
	gcc $(OBJ) -o $@

clean :
	rm -rf *.bak *.exe *~ *.o $(PROGRAM)
