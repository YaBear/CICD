all: s21_grep
s21_grep:
	gcc -Wall -Werror -Wextra s21_grep.c -o s21_grep
style:
	clang-format -style=Google -n -Werror *.c *.h
clean:
	rm -f *.o s21_grep
test: s21_grep
	bash test_func_grep.sh 
rebuild: clean all
