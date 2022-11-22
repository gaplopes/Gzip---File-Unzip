
# AUTORES DESTA PL2 2015/2016
# Trabalho realizado por:
# - Gonçalo Lopes (2014205060)
# - João Reis (2014216372)
# - Paulo Cruz (2014205161)

FLAGS  = -Wall -g
CC     = g++
PROG   = gzip
OBJS   = gzip.o huffman.o

all:	${PROG}

clean:
	rm ${OBJS} ${PROG} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

gzip.o:  gzip.h gzip.cpp

huffman.o: huffman.h huffman.cpp

gzip: gzip.o huffman.o
