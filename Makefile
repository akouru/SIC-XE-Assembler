# Makefile for class file_parser

TORM = macroPP macroPP.o file_parser.o
CC = g++
CCFLAGS = -g -O3 -Wall

macroPP:	macroPP.o file_parser.o
	$(CC) $(CCFLAGS) -o macroPP macroPP.o file_parser.o

macroPP.o:	macroPP.cpp macroPP.h
		${CC} ${CCFLAG} -c macroPP.cpp

file_parser.o:	file_parser.cc	file_parser.h
		${CC} ${CCFLAGS} -c file_parser.cc
		
clean:
	rm -f ${TORM}
