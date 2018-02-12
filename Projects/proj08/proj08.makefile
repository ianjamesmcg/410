#CSE410 proj08 Makefile
#mcgreg45
#Control translation using the make utility program

proj08: proj08.student.o
					g++ proj08.student.o -o proj08

proj08.student.o: proj08.student.c
					g++ -Wall -c proj08.student.c
