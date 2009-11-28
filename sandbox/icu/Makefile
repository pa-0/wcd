OBJ= icu.o

all : icu.exe

%.o : %.c
	gcc -c -std=gnu99 $< -o $@

icu.exe : $(OBJ)
	gcc $(OBJ) -o $@


clean :
	rm -rf *.bak *.exe *~
