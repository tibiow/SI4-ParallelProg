#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

int m ;//log(size_source)/log(2);

struct tablo {
  int * tab;
  int size;
};

void printArray(struct tablo * tmp) {
  printf("---- Array of size %i ---- \n", tmp->size);
  int size = tmp->size;
  int i;
  for (i = 0; i < size; ++i) {
    printf("%i ", tmp->tab[i]);
  }
  printf("\n");
}

struct tablo * allocateTablo(int size) {
  struct tablo * tmp = malloc(sizeof(struct tablo));
  tmp->size = size;
  tmp->tab = malloc(size*sizeof(int));
  tmp->tab[0]=0;
  return tmp;
}

void montee(struct tablo * source, struct tablo * destination) {
  //TODO : remplissage du tableau destination de taille 2*n en
  // copiant les données du tableau source dans destination, 
  // à la bonne position
  // on suppose que le malloc de destination a été fait avant
  int size_destination = destination->size;
  int size_source = source->size;
	for (int i = size_destination-1; i > (size_destination/2)-1 ; i--)
	{
		destination->tab[i] = source->tab[i-size_source];
		//printf("%d\n",destination->tab[i] );
	}
  // TODO: Boucle de calcul pour la montée dans l'arbre/tableau


	for (int l = m-1; l >= 0; l--)
	{
		for (int j = pow(2,l); j <= pow(2,l+1)-1; j++)
		{
			destination->tab[j] = destination->tab[2*j] + destination->tab[2*j+1];
		}
	}


}

void descente(struct tablo * a, struct tablo * b) {
  // TODO : implementation de la phase de descente
  b->tab[1]=0;
  for (int l = 1; l <= m; l++)
  {
  	for (int j = pow(2,l); j <= pow(2,l+1)-1; j++)
  	{
  		if (j%2==0)
  		{
  			b->tab[j] = b->tab[j/2]; 
  		}
  		else{
  			b->tab[j] = b->tab[j/2] + a->tab[j-1];
  		}
  	}
  }

}

void final(struct tablo * a, struct tablo *b) {
  for (int j = pow(2,m); j <= pow(2,m+1)-1; j++)
  {
  	b->tab[j] = b->tab[j] + a->tab[j];
  }
}

void generateArray(struct tablo * s) {
  //construction d'un tableau pour tester
  s->size=8;
  s->tab=malloc(s->size*sizeof(int));
  s->tab[0]=3;
  s->tab[1]=1;
  s->tab[2]=7;
  s->tab[3]=0;
  s->tab[4]=4;
  s->tab[5]=1;
  s->tab[6]=6;
  s->tab[7]=3;

}

int main(int argc, char **argv) {

	m = 3;
  struct tablo source;

  generateArray(&source);

  struct tablo * a = allocateTablo(source.size*2);
  montee(&source, a);
  printArray(a);

  struct tablo * b = allocateTablo(source.size*2);
  descente(a, b);
  printArray(b);
   
  final(a,b);
  printArray(b);
}