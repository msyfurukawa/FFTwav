#include <stdio.h>
#include "CmpNum.h"

void wavFT(char *filename, long sf, double start, long plot_xrange[2]);

int main(){
	long x_range[2] = {0, 2000};
	wavFT("clwav.txt", 44100, 1, x_range);

	return 0;
}
