#CSE410 proj06 Makefile
#mcgreg45
#Control translation using the make utility program

proj06: proj06.student.o
					g++ proj06.student.o -o proj06

proj06.student.o: proj06.student.c
					g++ -Wall -c proj06.student.c
