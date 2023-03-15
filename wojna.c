#include <stdio.h>

#define size 52
int talia[size];
int karty_na_wojnie = 0;

int cbuffA[size];
int lenA = 0;
int outA = 0;
int kartyA[size];


int cbuffB[size];
int lenB = 0;
int outB = 0;
int kartyB[size];




enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };


int cbuff_pushA(int card_nr) {
	if(lenA < size){
		cbuffA[(outA + lenA) % (size)] = card_nr;
		lenA++;
		return OK;
	}
	if( lenA == size){
		return OVERFLOW;
	} 
}

int cbuff_pushB(int card_nr) {
	if(lenB < size){
		cbuffB[(outB + lenB) % (size)] = card_nr;
		lenB++;
		return OK;
	}
	if( lenB == size){
		return OVERFLOW;
	} 
}

int cbuff_popA(void) {
	if (lenA == 0){
		return UNDERFLOW;
	}
    int out_nr = cbuffA[outA];
    outA = (outA + 1) % (size);
    lenA--;
    	return out_nr;
}

int cbuff_popB(void) {
	if (lenB == 0){
		return UNDERFLOW;
	}
    int out_nr = cbuffB[outB];
    outB = (outB + 1) % (size);
    lenA--;
    	return out_nr;
}



void swap (int array[], int i, int k) {
	int temp = array[i];
	array[i] = array[k];
	array[k] = temp;

}

void rand_permutation(int n, int array[]) {
	if(n>=0){
		for(int i = 0; i < n; i++){
			array[i] = i;
		}
		for(int j = 0; j < n-1; j++){
			int k = rand_from_interval(j, n-1);
			swap(array, j, k);
		}
	}
}

void tasowanie(int talia[]){
    rand_perumtation(size, talia);       
}

void rozdawanie(int cbuffA, int cbuffB, int talia[]){
    tasowanie(talia[size]);
    for(int i = 0; i < (size/2); i++){
        cbuff_pushA(talia[i]);
    }
    for(int j = (size/2); j < size; j++){
        cbuff_pushB(talia[j]);
    }
}


void wstaw_po_konfliktA(int kartyA[], int kartyB[],int cbuffA_B){


}