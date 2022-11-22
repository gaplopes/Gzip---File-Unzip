/*Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2007/2008*/

/*
AUTORES DESTA PL2 2015/2016
Trabalho realizado por:
- Gon�alo Lopes (2014205060)
- Jo�o Reis (2014216372)
- Paulo Cruz (2014205161)
*/

#include "gzip.h"

#include <math.h>

#include <cstdlib>
#include <fstream>

#include "huffman.h"

char *Int2Binary(int num, int lencode);
char *return0(int lencode);
void printFilesChar(char *arrayH[], int sizeofvector, char *fname);
void printFilesInt(int arrayH[], int sizeofvector, char *fname);
void convert_Huffman(HuffmanTree *tree, int *array_len, int k, int maximoLen, char *fname);
void lenLiteraisandComp(FILE *gzFile, HuffmanTree *tree, unsigned int *rb, char *availBits, int array[], int n, char *fname);
int leBitsArvore(FILE *gzFile, HuffmanTree *tree, unsigned int *rb, char *availBits);
double leBits(FILE *gzFile, unsigned int *rb, int needBits, char *availBits);
int maximoArray(int *array_len, int n);
void decompress(gzipHeader gzh, FILE *gzFile, char *availBits, unsigned int *rb, HuffmanTree *HLIT, HuffmanTree *HDIST, long origFileSize, FILE *ficheiro, char array_dados[]);

int conta = 0;

// fun��o principal, a qual gere todo o processo de descompacta��o
int main(int argc, char **argv) {
  //--- Gzip file management variables
  FILE *ficheiro;  // ponteiro para o novo ficheiro
  FILE *gzFile;    // ponteiro para o ficheiro a abrir
  long fileSize;
  long origFileSize;
  int numBlocks = 0;
  gzipHeader gzh;
  unsigned char byte;   // vari�vel tempor�ria para armazenar um byte lido directamente do ficheiro
  unsigned int rb = 0;  //�ltimo byte lido (poder� ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
  char needBits = 0, availBits = 0;

  char fname[30];
  int HLIT, HDIST, HCLEN;
  HuffmanTree *tree, *tree_HLIT, *tree_HDIST;
  tree = createHFTree();
  tree_HLIT = createHFTree();
  tree_HDIST = createHFTree();

  //--- obter ficheiro a descompactar
  if (argc != 2) {
    printf("Linha de comando invalida!!!");
    return -1;
  }
  char *fileName = argv[1];

  //--- processar ficheiro
  gzFile = fopen(fileName, "rb");
  fseek(gzFile, 0L, SEEK_END);
  fileSize = ftell(gzFile);
  fseek(gzFile, 0L, SEEK_SET);

  // ler tamanho do ficheiro original
  origFileSize = getOrigFileSize(gzFile);
  char array_dados[origFileSize];

  //--- ler cabe�alho
  int error = getHeader(gzFile, &gzh);
  if (error != 0) {
    printf("Formato invalido!!!");
    return -1;
  }

  //--- Para todos os blocos encontrados
  char BFINAL;

  if ((ficheiro = fopen(gzh.fName, "w")) == NULL) {
    printf("Erro na abertura do ficheiro!\n");
    exit(EXIT_FAILURE);
  }

  do {
    //--- ler o block header: primeiro byte depois do cabe�alho do ficheiro
    needBits = 3;
    if (availBits < needBits) {
      fread(&byte, 1, 1, gzFile);
      rb = (byte << availBits) | rb;
      availBits += 8;
    }

    // obter BFINAL
    // ver se � o �ltimo bloco
    BFINAL = rb & 0x01;  // primeiro bit � o menos significativo
    printf("%d\n", rb);
    printf("BFINAL = %d\n", BFINAL);
    rb = rb >> 1;  // descartar o bit correspondente ao BFINAL
    availBits -= 1;

    // analisar block header e ver se � huffman din�mico
    if (!isDynamicHuffman(rb))  // ignorar bloco se n�o for Huffman din�mico
      continue;
    rb = rb >> 2;  // descartar os 2 bits correspondentes ao BTYPE
    availBits -= 2;

    //--- Se chegou aqui --> compactado com Huffman din�mico --> descompactar
    //**************************************************
    //****** ADICIONAR PROGRAMA... *********************
    //**************************************************

    // PERGUNTA 1:
    FILE *output;
    strcpy(fname, "exercicio1.txt");
    HLIT = leBits(gzFile, &rb, 5, &availBits) + 257;
    HDIST = leBits(gzFile, &rb, 5, &availBits) + 1;
    HCLEN = leBits(gzFile, &rb, 4, &availBits) + 4;

    // escrita de resultados em ficheiro
    if ((output = fopen(fname, "w")) == NULL) {
      printf("nao foi possivel criar o ficheiro %s!\n", fname);
      exit(1);
    }

    fprintf(output, "HLIT = %d\n", HLIT - 257);
    fprintf(output, "HDIST = %d\n", HDIST - 1);
    fprintf(output, "HCLEN = %d\n", HCLEN - 4);
    fclose(output);

    // DECLARACAO DE VETORES (tem que ser depois porque ja sabemos os valores)
    int array_len[HLIT - 257];
    int array_HLIT[HLIT];
    int array_HDIST[HDIST];

    // PERGUNTA 2
    int arrayOrdem[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    memset(array_len, 0, sizeof(array_len));
    for (int i = 0; i < (HCLEN); i++) {
      array_len[arrayOrdem[i]] = leBits(gzFile, &rb, 3, &availBits);
      printf("ARRAY_LEN: %d - %d \n", arrayOrdem[i], array_len[arrayOrdem[i]]);
    }

    strcpy(fname, "exercicio2.txt");
    printFilesInt(array_len, 19, fname);

    // Funcao que devolve o comprimento maior no array de comprimentos(alfabeto de comprimentos)
    int maximo;
    maximo = maximoArray(array_len, HLIT - 257);
    printf("MAXIMO DO ARRAY_LEN = %d !\n", maximo);

    // PERGUNTA 3
    strcpy(fname, "exercicio3.txt");
    convert_Huffman(tree, array_len, HLIT - 257, maximo, fname);

    // PERGUNTA 4
    strcpy(fname, "exercicio4.txt");
    lenLiteraisandComp(gzFile, tree, &rb, &availBits, array_HLIT, HLIT, fname);

    // PERGUNTA 5
    strcpy(fname, "exercicio5.txt");
    lenLiteraisandComp(gzFile, tree, &rb, &availBits, array_HDIST, HDIST, fname);

    // PERGUNTA 6
    maximo = maximoArray(array_HLIT, HLIT);
    strcpy(fname, "exercicio6HLIT.txt");
    convert_Huffman(tree_HLIT, array_HLIT, HLIT, maximo, fname);
    maximo = maximoArray(array_HDIST, HDIST);
    strcpy(fname, "exercicio6HDIST.txt");
    convert_Huffman(tree_HDIST, array_HDIST, HDIST, maximo, fname);

    // PERGUNTA 7
    decompress(gzh, gzFile, &availBits, &rb, tree_HLIT, tree_HDIST, origFileSize, ficheiro, array_dados);

    // actualizar n�mero de blocos analisados
    numBlocks++;

    // APAGA AS ARVORES (UMA SIMPLES LIMPEZA DO "SISTEMA")
    delete (tree_HLIT->root);
    delete (tree_HDIST->root);
    delete (tree->root);

  } while (BFINAL == 0);

  // Vai escrever todos os dados para o novo ficheiro (o ficheiro descomprimido)
  if (fwrite(array_dados, 1, origFileSize, ficheiro) <= 0) {
    fclose(ficheiro);
    printf("Erro na escrita em ficheiro!\n");
    exit(EXIT_FAILURE);
  }
  printf("TAMANHO DO FAQ.txt: %d \n", (int)origFileSize);
  // termina��es
  fclose(gzFile);
  fclose(ficheiro);
  printf("End: %d bloco(s) analisado(s).\n", numBlocks);

  return EXIT_SUCCESS;
}

void printFilesInt(int arrayH[], int sizeofvector, char *fname) {
  int i;
  FILE *output;

  // escrita de resultados em ficheiro
  if ((output = fopen(fname, "w")) == NULL) {
    printf("nao foi possivel criar o ficheiro !\n");
    exit(1);
  }

  fprintf(output, "codigos de Huffman\n\nsimbolo = comprimento\n");
  for (i = 0; i < sizeofvector; i++) {
    fprintf(output, "%d\t=\t%d\n", i, arrayH[i]);
  }
  fclose(output);
}

void printFilesChar(char *arrayH[], int sizeofvector, char *fname) {
  int i;
  FILE *output;

  // escrita de resultados em ficheiro
  if ((output = fopen(fname, "w")) == NULL) {
    printf("nao foi possivel criar o ficheiro !\n");
    exit(1);
  }

  fprintf(output, "codigos de Huffman\n\nsimbolo = comprimento\n");
  for (i = 0; i < sizeofvector; i++) {
    fprintf(output, "%d\t=\t%s\n", i, arrayH[i]);
  }
  fclose(output);
}

void decompress(gzipHeader gzh, FILE *gzFile, char *availBits, unsigned int *rb, HuffmanTree *HLIT, HuffmanTree *HDIST, long origFileSize, FILE *ficheiro, char array_dados[]) {
  // Algoritmo LZ77, comprimento de bits a recuar
  int bits_len[2][30] = {{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0}, {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258}};
  // Algoritmo LZ77, distancia a recuar em bits
  int bits_distance[2][30] = {{0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13}, {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577}};

  int bit = 0, bits = 0, len = 0, dist = 0, i = 0, j = 0, aux = 0;

  while (bit != 256) {
    // le 1 bit da arvore e retorna o valor quando deixa de ser menos -2, ou seja ja nao � folha e � indice
    bit = leBitsArvore(gzFile, HLIT, rb, availBits);

    // se o bit lido for menor do que 256 ele vai adicionar o bit ao array_dados (que � o array com as letras do FAQ)
    if (bit < 256) {
      array_dados[j] = bit;
      j++;
    }

    // RFC deflate - section 3.2.5
    // se o bit lido for entre 256 e 286 faz:
    else if ((bit > 256) && (bit < 286)) {
      // vai ler bits_len[0][bit-257] bits do gzFile e adicionar a bits
      bits = leBits(gzFile, rb, bits_len[0][bit - 257], availBits);
      // de seguida vai ler o comprimento desses bits
      len = bits_len[1][bit - 257] + bits;
      // depois vai ler novamento os bits da arvore at� retornar um indice
      aux = leBitsArvore(gzFile, HDIST, rb, availBits);
      // de seguida novamente vamos ler bits_distance[0][aux] bits que nos vai dar a distancia
      bits = leBits(gzFile, rb, bits_distance[0][aux], availBits);
      // concluimos depois que a distancia sera calculada da seguinte forma
      dist = bits_distance[1][aux] + bits;
      dist = j - dist;
      // fazendo depois um ciclo for da distancia at� � distancia mais o comprimento, guardando entao o bit lido que encripta uma letra no array
      for (i = dist; i < dist + len; i++) {
        array_dados[j] = array_dados[i];
        j++;
      }
    }
  }
}

// Devolve o maior comprimento do array
int maximoArray(int *array_len, int n) {
  int j;
  int max = 0;
  for (j = 0; j < n; j++) {
    if (array_len[j] > max) {
      max = array_len[j];
    }
  }
  return max;
}

void lenLiteraisandComp(FILE *gzFile, HuffmanTree *tree, unsigned int *rb, char *availBits, int array[], int n, char *fname) {
  int i, j, g;
  int valor, temp;
  unsigned char byte;
  // Primeiro vamos zerar todo o array
  for (g = 0; g < n; g++) {
    array[g] = 0;
  }

  for (i = 0; i < n;) {
    valor = leBitsArvore(gzFile, tree, rb, availBits);

    if (valor > -1 && valor <= 15) {
      // comp � o indice
      array[i] = valor;
      i++;
    }
    if (valor == 16) {
      // o bit actual nao se mexe, apenas se avan�a para os proximos dois que depois nos vao dar o numero de vezes
      //    para repetir o bit n vezes + 3 sendo no maximo 6
      if (*availBits < 2) {
        fread(&byte, 1, 1, gzFile);
        *rb = (byte << *availBits) | *rb;
        *availBits += 8;
      }
      temp = *rb & 0x03;
      *rb = *rb >> 2;
      *availBits -= 2;

      for (j = 0; j < temp + 3; j++) {
        array[i] = array[i - 1];
        i++;
      }
      // incrementar o i para quando chegar ao inicio do for voltar a incrementar
    }
    if (valor == 17) {
      // o bit actual nao se mexe, apenas se avan�a para os proximos tres que depois nos vao dar o numero de vezes
      //    para repetir o bit n vezes + 3 sendo no maximo 10
      if (*availBits < 3) {
        fread(&byte, 1, 1, gzFile);
        *rb = (byte << *availBits) | *rb;
        *availBits += 8;
      }
      // os proximos 3 bits indicam o numero de vezes a repetir o codigo 0 (3 a 10)
      temp = *rb & 0x07;
      *rb = *rb >> 3;
      *availBits -= 3;

      for (j = 0; j < temp + 3; j++) {
        array[i] = 0;
        i++;
      }
      // incrementar o i para quando chegar ao inicio do for voltar a incrementar
    }
    if (valor == 18) {
      // o bit actual nao se mexe, apenas se avan�a para os proximos sete que depois nos vao dar o numero de vezes
      //    para repetir o bit n vezes + 11 sendo no maximo 138

      if (*availBits < 7) {
        fread(&byte, 1, 1, gzFile);
        *rb = (byte << *availBits) | *rb;
        *availBits += 8;
      }

      // os proximos 3 bits indicam o numero de vezes a repetir o codigo 0 (11 a 138)
      temp = *rb & 0x07F;  // 01111111 (127)
      *rb = *rb >> 7;
      *availBits -= 7;

      for (j = 0; j < temp + 11; j++) {
        array[i] = 0;
        i++;
      }
    }
  }
  printFilesInt(array, n, fname);
}

// L� bit do ficheiro e prossegue na �rvore at� encontrar uma folha
int leBitsArvore(FILE *gzFile, HuffmanTree *tree, unsigned int *rb, char *availBits) {
  int valor, aux;
  char a;
  unsigned char byte;

  valor = -2;

  while ((valor == -2)) {
    if (*availBits < 1) {
      fread(&byte, 1, 1, gzFile);
      *rb = (byte << *availBits) | *rb;
      *availBits += 8;
    }
    aux = *rb & 0x1;  // aux fica apenas com o ultimo bit do bloco
    *rb = *rb >> 1;   // r retira o bit guardado em aux
    *availBits -= 1;  // diminiu o numero de bits disponiveis
    a = aux + '0';

    valor = nextNode(tree, a);
  }
  resetCurNode(tree);
  return valor;
}

void convert_Huffman(HuffmanTree *tree, int *arrayComp, int k, int maximoLen, char *fname) {
  // Count the number of codes for each code length. Let bl_count[N] be the number of codes of length N, N >= 1
  int bl_count[maximoLen];
  int next_code[maximoLen];
  char *vetor[19 + 257];
  int conta, code, aux, z;

  // Primeiro vamos zerar todo o array
  for (int g = 0; g < maximoLen; g++) {
    bl_count[g] = 0;
  }

  // Depois vamos percorrer o array para guardar o numero de codes length
  for (int i = 0; i <= maximoLen; i++) {  // percorre o bl_count
    conta = 0;
    for (int j = 0; j < k; j++) {  // percorre o array
      if (arrayComp[j] == i) {
        conta++;
      }
    }
    bl_count[i] = conta;
  }

  // Find the numerical value of the smallest code for each code length
  code = 0;
  bl_count[0] = 0;
  // o codigo seguinte soma o numero de ocorrencias actual com o anterior e multipla por 2
  for (int bits = 1; bits <= maximoLen; bits++) {
    code = (code + bl_count[bits - 1]) << 1;  // multiplica por 2 o <<
    next_code[bits] = code;
  }

  // Assign numerical values to all codes, using consecutive values for all codes of the same
  //    length with the base values determined at step 2. Codes that are never used (which have a bit length of zero)
  //    must not be assigned a value.

  for (int n = 0; n < k; n++) {
    if (bl_count[arrayComp[n]] == 0) {
      vetor[n] = return0(1);  // para meter o comprimento 0 chamar esta fun�ao
    }

    if (bl_count[arrayComp[n]] != 0) {
      aux = arrayComp[n];
      arrayComp[n] = next_code[arrayComp[n]];
      addNode(tree, Int2Binary(next_code[aux], aux), n, 1);
      if (Int2Binary(next_code[aux], aux) != NULL) {
        vetor[n] = Int2Binary(next_code[aux], aux);
      } else {
        vetor[n] = 0;
      }

      next_code[aux]++;
    }
  }

  printFilesChar(vetor, k, fname);
}

char *Int2Binary(int num, int lencode) {
  int *vet_bin = new int[lencode];
  char *bin = new char[lencode + 1];
  int i = 0, g, j, w, y, contador = 0, avanco;
  int numaux = num;

  for (g = 0; g < lencode; g++) {
    bin[g] = '\0';
  }

  if (num == 0) {
    for (w = 0; w < lencode; w++) {
      strcat(bin, "0");
    }
    return bin;
  }

  while (numaux > 0) {
    numaux = numaux / 2;
    contador++;
  }

  avanco = lencode - contador;

  for (y = 0; y < avanco; y++) {
    strcat(bin, "0");
  }

  while (num > 0) {
    vet_bin[i] = num % 2;
    i++;
    num = num / 2;
  }

  for (j = i - 1; j >= 0; j--) {
    if (vet_bin[j] == 1) {
      strcat(bin, "1");
    }
    if (vet_bin[j] == 0) {
      strcat(bin, "0");
    }
  }
  return bin;
}

char *return0(int lencode) {
  char *bin = new char[lencode + 1];
  int g;

  for (g = 0; g < lencode; g++) {
    bin[g] = '0';
    bin[g + 1] = '\0';
  }

  return bin;
}

// Al�nea 1 - Funcao que le um n�mero vari�vel bits do ficheiro compactado
double leBits(FILE *gzFile, unsigned int *rb, int needBits, char *availBits) {
  int j;
  int mask = 0;
  double final;
  unsigned char byte;

  while (*availBits < needBits) {
    fread(&byte, 1, 1, gzFile);
    *rb = (byte << *availBits) | *rb;
    *availBits += 8;
  }

  for (j = 0; j < needBits; j++) {  // forma de obter a mascara do bit que quero, tornando a fun�ao mais geral
    mask += pow(2, j);
  }

  final = (*rb) & mask;

  // descartar os needBits lidos;
  *rb = *rb >> needBits;
  *availBits -= needBits;
  return final;
}

// TERMINA AQUI O NOVO CODIGO
//---------------------------------------------------------------

// L� o cabe�alho do ficheiro gzip: devolve erro (-1) se o formato for inv�lido devolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh)  // obt�m cabe�alho
{
  unsigned char byte;

  // Identica��o 1 e 2: valores fixos
  fread(&byte, 1, 1, gzFile);
  (*gzh).ID1 = byte;
  if ((*gzh).ID1 != 0x1f) return -1;  // erro no cabe�alho

  fread(&byte, 1, 1, gzFile);
  (*gzh).ID2 = byte;
  if ((*gzh).ID2 != 0x8b) return -1;  // erro no cabe�alho

  // M�todo de compress�o (deve ser 8 para denotar o deflate)
  fread(&byte, 1, 1, gzFile);
  (*gzh).CM = byte;
  if ((*gzh).CM != 0x08) return -1;  // erro no cabe�alho

  // Flags
  fread(&byte, 1, 1, gzFile);
  unsigned char FLG = byte;

  // MTIME
  char lenMTIME = 4;
  fread(&byte, 1, 1, gzFile);
  (*gzh).MTIME = byte;
  for (int i = 1; i <= lenMTIME - 1; i++) {
    fread(&byte, 1, 1, gzFile);
    (*gzh).MTIME = (byte << 8) + (*gzh).MTIME;
  }

  // XFL (not processed...)
  fread(&byte, 1, 1, gzFile);
  (*gzh).XFL = byte;

  // OS (not processed...)
  fread(&byte, 1, 1, gzFile);
  (*gzh).OS = byte;

  //--- Check Flags
  (*gzh).FLG_FTEXT = (char)(FLG & 0x01);
  (*gzh).FLG_FHCRC = (char)((FLG & 0x02) >> 1);
  (*gzh).FLG_FEXTRA = (char)((FLG & 0x04) >> 2);
  (*gzh).FLG_FNAME = (char)((FLG & 0x08) >> 3);
  (*gzh).FLG_FCOMMENT = (char)((FLG & 0x10) >> 4);

  // FLG_EXTRA
  if ((*gzh).FLG_FEXTRA == 1) {
    // ler 2 bytes XLEN + XLEN bytes de extra field
    // 1� byte: LSB, 2�: MSB
    char lenXLEN = 2;

    fread(&byte, 1, 1, gzFile);
    (*gzh).xlen = byte;
    fread(&byte, 1, 1, gzFile);
    (*gzh).xlen = (byte << 8) + (*gzh).xlen;

    (*gzh).extraField = new unsigned char[(*gzh).xlen];

    // ler extra field (deixado como est�, i.e., n�o processado...)
    for (int i = 0; i <= (*gzh).xlen - 1; i++) {
      fread(&byte, 1, 1, gzFile);
      (*gzh).extraField[i] = byte;
    }
  } else {
    (*gzh).xlen = 0;
    (*gzh).extraField = 0;
  }

  // FLG_FNAME: ler nome original
  if ((*gzh).FLG_FNAME == 1) {
    (*gzh).fName = new char[1024];
    unsigned int i = 0;
    do {
      fread(&byte, 1, 1, gzFile);
      if (i <= 1023)  // guarda no m�ximo 1024 caracteres no array
        (*gzh).fName[i] = byte;
      i++;
    } while (byte != 0);
    if (i > 1023)
      (*gzh).fName[1023] = 0;  // apesar de nome incompleto, garantir que o array termina em 0
  } else
    (*gzh).fName = 0;

  // FLG_FCOMMENT: ler coment�rio
  if ((*gzh).FLG_FCOMMENT == 1) {
    (*gzh).fComment = new char[1024];
    unsigned int i = 0;
    do {
      fread(&byte, 1, 1, gzFile);
      if (i <= 1023)  // guarda no m�ximo 1024 caracteres no array
        (*gzh).fComment[i] = byte;
      i++;
    } while (byte != 0);
    if (i > 1023)
      (*gzh).fComment[1023] = 0;  // apesar de coment�rio incompleto, garantir que o array termina em 0
  } else
    (*gzh).fComment = 0;

  // FLG_FHCRC (not processed...)
  if ((*gzh).FLG_FHCRC == 1) {
    (*gzh).HCRC = new unsigned char[2];
    fread(&byte, 1, 1, gzFile);
    (*gzh).HCRC[0] = byte;
    fread(&byte, 1, 1, gzFile);
    (*gzh).HCRC[1] = byte;
  } else
    (*gzh).HCRC = 0;

  return 0;
}

//---------------------------------------------------------------
// Analisa block header e v� se � huffman din�mico
int isDynamicHuffman(unsigned char rb) {
  unsigned char BTYPE = rb & 0x03;

  if (BTYPE == 0)  //--> sem compress�o
  {
    printf("Ignorando bloco: sem compactacao!!!");
    return 0;
  } else if (BTYPE == 1) {
    printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
    return 0;
  } else if (BTYPE == 3) {
    printf("Ignorando bloco: BTYPE = reservado!!!\n");
    return 0;
  } else
    return 1;
}

//---------------------------------------------------------------
// Obt�m tamanho do ficheiro original
long getOrigFileSize(FILE *gzFile) {
  // salvaguarda posi��o actual do ficheiro
  long fp = ftell(gzFile);

  //�ltimos 4 bytes = ISIZE;
  fseek(gzFile, -4, SEEK_END);

  // determina ISIZE (s� correcto se cabe em 32 bits)
  unsigned long sz = 0;
  unsigned char byte;
  fread(&byte, 1, 1, gzFile);
  sz = byte;
  for (int i = 0; i <= 2; i++) {
    fread(&byte, 1, 1, gzFile);
    sz = (byte << 8 * (i + 1)) + sz;
  }

  // restaura file pointer
  fseek(gzFile, fp, SEEK_SET);

  return sz;
}

//---------------------------------------------------------------
void bits2String(char *strBits, int byte, int n) {
  char mask = 0x01;  // get LSbit

  strBits[n] = 0;
  for (char bit, i = n - 1; i >= 0; i--) {
    bit = byte & mask;
    strBits[i] = bit + 48;  // converter valor num�rico para o caracter alfanum�rico correspondente
    byte = byte >> 1;
  }
}
