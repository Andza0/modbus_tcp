mbread: mbread.c
	gcc -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -o mbread mbread.c

clean:
	rm -f mbread
