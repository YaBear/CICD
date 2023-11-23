all: s21_cat

s21_cat:
	gcc -Wall -Werror -Wextra -c s21_cat.c
	gcc s21_cat.o -o s21_cat
	rm -rf *.o

rebuild: clean all

style:
	clang-format -style=Google -n -Werror *.c *.h

style_format:
	clang-format -style=Google -i *.c *.h

clean:
	rm -rf *.o s21_cat

test: rebuild
	bash test_func_cat.sh

test_valgrind: rebuild
	bash test_valgrind_cat.sh

test_leaks: rebuild
	sh test_leak_cat.sh