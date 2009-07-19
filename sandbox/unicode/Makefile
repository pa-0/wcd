OBJ= wcddir.o warg.o

all : warg.exe

%.o : %.c
	gcc -c -DWIN32 -DWCD_UNICODE -std=gnu99 $< -o $@

warg.exe : $(OBJ)
	gcc $(OBJ) -o $@
