a.out: *.c
	gcc -o shell *.c
shell: builtins.c config.c ls.c main.c prompt.c utilities.c
	gcc -o shell builtins.c config.c ls.c main.c prompt.c utilities.c 

clean: 
	rm -rf .*
