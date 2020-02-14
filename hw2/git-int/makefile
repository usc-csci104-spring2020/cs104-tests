all: gitint

gitint: gitint.cpp gitint-shell.cpp
	g++ -g -Wall -std=c++11 $^ -o gitint

dbg:gitint.cpp gitint-shell.cpp
	g++ -g -Wfatal-errors -std=c++11 $^ -o gitint

test:
	./gitint > output.exp
	diff -s -y -W 80 output.out output.exp

clean:
	rm -f *.o gitint