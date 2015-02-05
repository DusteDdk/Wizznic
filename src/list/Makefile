test: main.o lib
	gcc main.o list.a -o test-list.static
	gcc main.o list.so -o test-list.shared

main.o: main.c
	gcc -c main.c

lib:
	gcc -Wall -c list.c -o list.a
	gcc -Wall -c -fPIC list.c
	gcc -shared -Wl,-soname,list.so -o list.so list.o

clean:
	rm -rf *.o test-list.* *.so *.a
