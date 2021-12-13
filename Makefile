FLAGS=-Wall -Wextra -Wpedantic -Wconversion -Wformat-security -Werror
STD=-std=c17
CMD=gcc
PROG=JackAnalyzer
FILES=*.c

build: clean
	$(CMD) $(STD) $(FLAGS) $(FILES) -o $(PROG)
debug: clean
	$(CMD) $(STD) $(FLAGS) -g $(FILES) -o $(PROG)
run:
	./$(PROG) test.jack
clean:
	rm -f $(PROG)
	rm -f *.xml
