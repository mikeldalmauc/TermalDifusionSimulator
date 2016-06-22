/************************************************************************
P11-distribute0.c

>>> TO DO: MODIFY AND COMPLETE <<<
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){
	int    N, Nloc, npr, remainder, i;

	printf("\n Data to distribute:  N and npr\n");
	scanf("%d %d" , &N, &npr);

	//Initialization of size and shift 
	int size[npr],shift[npr],size2[npr],shift2[npr];
	for (i=0; i<npr; i++){
		size[i] = 0; 	shift[i] = 0;
		size2[i] = 0;	shift2[i] = 0;
	}
	//Compute Nloc and remainder
	Nloc = floor((double)N/(double)npr);
	remainder = N - Nloc*npr;

// FIRST DISTRIBUTION:  the remainder for the last process 
//	>>> TO DO: CALCULATE SIZE AND SHIFTS FOR THE FIRST DISTRIBUTION <<<

	for(i=0; i<npr-1; i++){
		size[i] = Nloc;
		shift[i] = i*Nloc;
	}
	size[npr-1] = Nloc + remainder;
	shift[npr-1] = (npr-1)*Nloc;

	printf("\n FIRST DISTRIBUTION: the remainder for the last process \n");
	for (i=0; i<npr; i++) printf("\n  %d   %d", size[i], shift[i]);
  
// SECOND DISTRIBUTION: the remainder distributed (+1) among the first processes
//	>>> TO DO: CALCULATE SIZE AND SHIFTS FOR THE SECOND DISTRIBUTION <<<
    
	
	  

	//Value asignment to first process  
	size2[0] = Nloc;
	shift2[0] = 0; 
	
	//Distribution of the remainder among the first processes 
	i = 0;
	while(remainder){
		size2[i] += 1;
		remainder -= 1;
		i++;
	}
	
	//Distribute the rest of the vector among the processes
	for(i=1; i<npr; i++){
	  size2[i] += Nloc;
	  shift2[i] = shift2[(i-1)] + size2[(i-1)];
	}
	
	printf("\n\n SECOND DISTRIBUTION: the remainder distributed (+1) among the first processes \n");
	for (i=0; i<npr; i++) printf("\n  %d   %d", size2[i], shift2[i]);
	printf("\n");

	return 0;
}



