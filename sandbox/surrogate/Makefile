OBJ= surwidth.o wcwidth.o
PROGRAM = surwidth

all : $(PROGRAM) surrogate

%.o : %.c
	gcc -Wall -Wextra -c -std=gnu99 $< -o $@

wcwidth.o : ../../wcd/src/wcwidth.c
	gcc -Wall -Wextra -c -std=gnu99 $< -o $@


$(PROGRAM) : $(OBJ)
	gcc $(OBJ) -o $@

surrogate : surrogate.c
	gcc $< -o $@

clean :
	rm -rf *.o *.bak *.exe *~ $(PROGRAM) surrogate
