/* Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2007/2008*/

/*
AUTORES DESTA PL2 2015/2016
Trabalho realizado por:
- Gon�alo Lopes (2014205060)
- Jo�o Reis (2014216372)
- Paulo Cruz (2014205161)
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

// Gzip header variables
typedef struct header {
  // elementos fixos
  unsigned char ID1, ID2, CM, XFL, OS;
  unsigned long MTIME;
  unsigned char FLG_FTEXT, FLG_FHCRC, FLG_FEXTRA, FLG_FNAME, FLG_FCOMMENT;  // bits 0, 1, 2, 3 e 4, respectivamente (restantes 3: reservados)

  // FLG_FTEXT --> ignorado deliberadamente (tipicamente igual a 0)
  // se FLG_FEXTRA == 1
  unsigned char xlen;
  unsigned char *extraField;

  // se FLG_FNAME == 1
  char *fName;  // terminada por um byte a 0

  // se FLG_FCOMMENT == 1
  char *fComment;  // terminada por um byte a 0

  // se FLG_HCRC == 1
  unsigned char *HCRC;
} gzipHeader;

long getOrigFileSize(FILE *gzFile);
int getHeader(FILE *gzFile, gzipHeader *gzh);
int isDynamicHuffman(unsigned char rb);
void bits2String(char *strBits, int byte, int n);
