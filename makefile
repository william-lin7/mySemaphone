all: control.o write.o
	gcc -o control control.o
	gcc -o write write.o

control.o: control.c
	gcc -c control.c

write.o: write.c
	gcc -c write.c

clean:
	rm *.o
	rm *~
