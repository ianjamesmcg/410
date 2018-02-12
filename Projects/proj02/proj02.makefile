#CSE410 proj05 Makefile
#mcgreg45
#Control translation using the make utility program

proj02: proj02.student.o
					g++ proj02.student.o -o proj02

proj02.student.o: proj02.student.c
					g++ -Wall -c proj02.student.c
