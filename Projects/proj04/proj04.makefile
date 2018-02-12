#CSE410 proj05 Makefile
#mcgreg45
#Control translation using the make utility program

proj04: proj04.student.o
					g++ proj04.student.o -o proj04

proj04.student.o: proj04.student.c
					g++ -Wall -c proj04.student.c
