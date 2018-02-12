#CSE410 proj05 Makefile
#mcgreg45
#Control translation using the make utility program

proj03: proj03.student.o
					g++ proj03.student.o -o proj03

proj03.student.o: proj03.student.c
					g++ -Wall -c proj03.student.c
