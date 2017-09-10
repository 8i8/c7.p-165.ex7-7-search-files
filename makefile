a.out: src/init.c src/main.c src/io.c src/sort.c src/hash.c
	gcc -g -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer src/init.c src/main.c src/io.c src/sort.c src/hash.c
