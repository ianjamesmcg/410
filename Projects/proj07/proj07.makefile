#CSE410 proj06 Makefile
#mcgreg45
#Control translation using the make utility program

proj07: proj07.student.o
					g++ proj07.student.o -o proj07

proj07.student.o: proj07.student.c
					g++ -Wall -c proj07.student.c
