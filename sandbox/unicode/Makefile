OBJ= warg.o

all : warg.exe

%.o : %.c
ifdef MSYSTEM
	# MinGW
	gcc -c -DWIN32 -DWCD_UNICODE -std=gnu99 $< -o $@
else
	# Linux
	gcc -c -DUNIX -DWCD_UNICODE -std=gnu99 $< -o $@
endif

warg.exe : $(OBJ)
	gcc $(OBJ) -o $@

clean :
	rm -rf *.bak *.exe *~
