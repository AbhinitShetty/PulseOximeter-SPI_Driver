/*
 ============================================================================
 Name        : ecgfilt.c
 Author      : Kai Mueller
 Version     :
 Copyright   : Univ. Bremerhaven
 Description : generic discrete IIR filter
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


#define  IIR_NMAX 10
typedef struct {
	int n;
	double  num[IIR_NMAX];
	double  den[IIR_NMAX];
	double  x[IIR_NMAX];
} IIR_object;


static IIR_object IIR_Highp, IIR_Lowp;


static void InitIIR() {
	int  k;

	// high pass filter fc_h = 0.4 Hz
	IIR_Highp.n = 4;

	IIR_Highp.num[0] =  0.996721633714597;  //b0
	IIR_Highp.num[1] = -3.98688653485839;   //b1
	IIR_Highp.num[2] =  5.98032980228758;   //b2
	IIR_Highp.num[3] = -3.98688653485839;   //b3
	IIR_Highp.num[4] =  0.996721633714597;  //b4

	IIR_Highp.den[0] =  1.0;                //a0
	IIR_Highp.den[1] = -3.99343249905975;
	IIR_Highp.den[2] =  5.98031905462197;
	IIR_Highp.den[3] = -3.98034057063714;
	IIR_Highp.den[4] =  0.993454015114695;

	for (k = 0; k < IIR_NMAX; k++) {
		IIR_Highp.x[k] = 0.0;
	}

	// low pass filter fc_l = 40 Hz
	IIR_Lowp.n = 8;

	IIR_Lowp.num[0] =  3.39885405636203e-08;
	IIR_Lowp.num[1] =  2.71908324508963e-07;
	IIR_Lowp.num[2] =  9.51679135781369e-07;
	IIR_Lowp.num[3] =  1.90335827156274e-06;
	IIR_Lowp.num[4] =  2.37919783945342e-06;
	IIR_Lowp.num[5] =  1.90335827156274e-06;
	IIR_Lowp.num[6] =  9.51679135781369e-07;
	IIR_Lowp.num[7] =  2.71908324508963e-07;
	IIR_Lowp.num[8] =  3.39885405636203e-08;

	IIR_Lowp.den[0] =   1.0;
	IIR_Lowp.den[1] =  -6.71209536004312;
	IIR_Lowp.den[2] =  19.7998045536059;
	IIR_Lowp.den[3] = -33.5161659992072;
	IIR_Lowp.den[4] =  35.5989372956733;
	IIR_Lowp.den[5] = -24.288697758792;
	IIR_Lowp.den[6] =  10.3935331061982;
	IIR_Lowp.den[7] =  -2.54984059629758;
	IIR_Lowp.den[8] =   0.274533459928962;

	for (k = 0; k < IIR_NMAX; k++) {
		IIR_Lowp.x[k] = 0.0;
	}

}


static double IIRdfII(IIR_object *filtobj, double u)  // "filtobj" is used as a pointer
{
	int k, n;
	double  y;

	n = filtobj->n;
	// correct output equation here...
	y = filtobj -> num[0] * u + filtobj -> x[0] ;     // "filtobj ->" is to dereference a pointer
	// state equations here...
	for (k=1; k<n; k++){
		filtobj -> x[k-1] = filtobj->num[k] *u + filtobj->x[k] - filtobj->den[k] * y;
	}
	filtobj -> x[n-1] = filtobj->num[n]*u  - filtobj->den[n]*y ;
	return y;
}


int main(void) {
	FILE *fi, *fo;
	int  k;
	double  u, y_hp, y;

	printf("--- IIR Filter for ECG signals ---\n");
	if ((fi = fopen("u.dat", "r")) == NULL) {
		printf(" *** cannot open u.dat for reading.\n");
		exit(EXIT_FAILURE);
	}
	if ((fo = fopen("y.dat", "w")) == NULL) {
		printf(" *** cannot open y.dat for writing.\n");
		fclose(fi);
		exit(EXIT_FAILURE);
	}

	InitIIR();
	k = 0;
	printf("         k           u            y_hp               y\n");
	printf("------------------------------------------------------\n");
	while (!feof(fi)) {
		if ((fscanf(fi, "%lf", &u)) == 1) {
			k++;
			y_hp = IIRdfII(&IIR_Highp, u);
			y = IIRdfII(&IIR_Lowp, y_hp);
			printf("%10d  %10.5g  %14.6g  %14.6g\n", k, u, y_hp, y);
			fprintf(fo, "%17.7e\n", y);
		}
	}
	printf("------------------------------------------------------\n");
	printf(" %d lines processed.\n", k);
	fclose(fo);
	fclose(fi);
	printf("That`s all, folks.\n");
	return EXIT_SUCCESS;
}
