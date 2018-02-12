#CSE410 proj05 Makefile
#mcgreg45
#Control translation using the make utility program

proj05: proj05.student.o
					g++ -lpthread proj05.student.o -o proj05

proj05.student.o: proj05.student.c
					g++ -Wall -c proj05.student.c
