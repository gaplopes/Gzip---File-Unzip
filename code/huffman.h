/*Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2007/2008*/

/*
AUTORES DESTA PL2 2015/2016
Trabalho realizado por:
- Gon�alo Lopes (2014205060)
- Jo�o Reis (2014216372)
- Paulo Cruz (2014205161)
*/

#include <stdio.h>
#include <string.h>

typedef struct hfnode {
  short index;                  // se folha, guarda posi��o no alfabeto; sen�o, -1;
  short level;                  // n�vel do n� na �rvore
  struct hfnode *left, *right;  // refer�ncias para os filhos direito e esquerdo: � folha se ambos forem NULL
} HFNode;

typedef struct huffmantree {
  HFNode *root, *curNode;  // ra�z da �rvore e n� actual na travessia
} HuffmanTree;

HuffmanTree *createHFTree();
HFNode *createHFNode(short index, short level, HFNode *left, HFNode *right);
int addNode(HuffmanTree *hft, char *s, int ind, short verbose);
int nextNode(HuffmanTree *hft, char c);                               // utilizada para pesquisa na �rvore bit a bit (e n�o c�digo inteiro de uma vez)
int findNode(HuffmanTree *hft, char *s, short verbose);               // procura c�digo s a partir da ra�z (c�digo inteiro de uma vez)
int findNode(HuffmanTree *hft, char *s, HFNode *cur, short verbose);  // procura c�digo a partir do n� cur
short isLeaf(HFNode *n);
void resetCurNode(HuffmanTree *hft);
