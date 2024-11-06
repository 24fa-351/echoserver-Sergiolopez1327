main: main.c \
		redirect.c split.c abspath.c \
		redirect.h split.h abspath.h
	gcc -o main main.c redirect.c split.c abspath.c

results: main
	(echo "Test 1:"; echo; \
	echo "Command Run: ./main input.txt \"wc -w\" output.txt"; echo; ./main input.txt "wc -w" output.txt; \
	cat output.txt; echo; \
	echo "Test 2:"; echo; \
	echo "Command Run: ./main input.txt \"wc -l\" output.txt"; echo; ./main input.txt "wc -l" output.txt; \
	cat output.txt; echo; \
	echo "Test 3:"; echo; \
	echo "Command Run: ./main input.txt \"/bin/ls\" output.txt"; echo; ./main input.txt "/bin/ls" output.txt; \
	cat output.txt; echo) \
		> results.txt 2>&1

clean:
	rm -f main output.txt results.txt

