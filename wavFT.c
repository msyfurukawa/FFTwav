#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CmpNum.h"

#define LINE_LEN 256

void FFT(complex *data, complex *X, int k);

/*
 * calculate Fourier transform from [start] second to [start]+1 second
 * sf means sampling frequency
 * [filename] should be .txt file (not .wav file) converted by "dumpwave.c"
 * use only 1 channel
 */
void wavFT(char *filename, long sf, double start, long plot_xrange[2]){
	FILE *f;
	if((f = fopen(filename, "rb")) == NULL) return;

	char readline[LINE_LEN];

	long start_row = sf * start + 1;
	long r;

	//seek file to start_row
	for(r = 1; r < start_row; r++){
		//end if the number of row of [filename] is less than [start_row]
		if(fgets(readline, LINE_LEN, f) == NULL) return;
	}

	//K=2^k is a maximum number s.t. K < sf
	int k = 0, K = 1;
	for(; K < sf/2; K *= 2) k++;

	//read K rows (the number of samples in one second) and apply window function
	complex *data = (complex*)malloc(sizeof(complex)*K);
	if(data == NULL) return;
	r = 0;
	while(fgets(readline, LINE_LEN, f) != NULL){
		data[r].Re = strtod(readline, NULL) * (0.54 - 0.46 * cos(2*M_PI*(r+1)/K));
		data[r].Im = 0;
		//printf("%lf\n", data[r].Re);
		if(++r >= K) break;
	}
	//end if the number of read row is less than [K]
	if(r < K) return;

	fclose(f);

	complex *ft = (complex*)malloc(sizeof(complex)*K);
	if(ft == NULL) return;

	//Fourier transform
	FFT(data, ft, k);

	//plot result of FT
	double fr;
	FILE *gp;
	gp = popen("gnuplot -persist","w");

	fprintf(gp, "set xrange[%ld:%ld]\n", plot_xrange[0], plot_xrange[1]);

	fprintf(gp, "plot '-' with impulses\n");
	for(r = 0; r < K; r++){
		//fr is frequency of [r]th datum in ft
		fr = (r+1) * sf / K;
		fprintf(gp,"%lf\t%lf\n", fr, sqrt(pow(ft[r].Re, 2) + pow(ft[r].Im, 2)));
	}
	fprintf(gp, "e\n");

	pclose(gp);

	free(data);
	free(ft);
}
