#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int tab_length ;

void carre(int* tableau){
	#pragma omp parallel for
	for (int i = 0; i < tab_length; i++)
	{
		tableau[i] = tableau[i]*tableau[i];
	}
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	tab_length = atoi(argv[1]);
	int* tab = malloc(tab_length*sizeof(int));

	omp_set_num_threads(atoi(argv[2]));
	#pragma omp parallel for
	for (int i = 0; i < tab_length; i++)
	{
		//printf("%d\n",omp_get_num_threads() );
		tab[i]= rand()%255;
		//printf("%d\n",tab[i] );
	}


	//printf("%d\n", tab[0]);
	carre(tab);
	//printf("%d\n", tab[0]);
	
	return 0;
}