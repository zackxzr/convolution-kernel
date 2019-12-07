CFLAG=-Wall -O2 -ansi
filter:filter.o
	gcc -o filter $?

clean:
	@rm -rf filter *.o