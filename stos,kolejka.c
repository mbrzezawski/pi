#include <stdio.h>

#define STACK_SIZE 10

enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };

int stack[STACK_SIZE];
int top = 0;

int stack_push(int x) {
	if(top>=STACK_SIZE){
		return OVERFLOW;
	}
	else{
		stack[top] = x;
		top++;
		return OK;
	}
}

int stack_pop(void) {
	if(top == 0){
		return UNDERFLOW;
	}
	else{
		top--;
		return stack[top];
	}
}

int stack_state(void) {
	return top;
	}


// FIFO queue with shifts

#define QUEUE_SIZE 10

int queue[QUEUE_SIZE];
int in = 0, curr_nr = 0;

int queue_push(int in_nr) {
	while(in < QUEUE_SIZE && in_nr > 0){
		curr_nr++;
		queue[in] = curr_nr;
		in++;
		in_nr--;
	}
	if(in_nr == 0 && in <= QUEUE_SIZE){
		return OK;
	}
	else{
		curr_nr = curr_nr + in_nr;
		return OVERFLOW;
	}
}
	
int queue_pop(int out_nr) {
	if(out_nr >= in){
		for(int i = 0; i < QUEUE_SIZE; i++){
			queue[i] = 0;
		}
		if(out_nr == in){
			in = 0;
			return in;
		}
		else{
			in = 0;
			return UNDERFLOW;
		}
	}
	else{
		for(int i = 0; i < out_nr; i++){
			in--;
			for(int j = 0; j < QUEUE_SIZE - 1; j++){
				queue[j] = queue[j + 1];
			}
		}
		return in;
	}
}
int queue_state(void) {
	return in;
}

void queue_print(void) {
	for(int i = 0; i < in; i++){
		printf("%d ", queue[i]);
}
}
// Queue with cyclic buffer

#define CBUFF_SIZE 10

int cbuff[CBUFF_SIZE];
int out = 0, len = 0;


int cbuff_push(int cli_nr) {
	if(len < CBUFF_SIZE){
		cbuff[(out + len) % (CBUFF_SIZE)] = cli_nr;
		len++;
		return OK;
	}
	if( len == CBUFF_SIZE){
		return OVERFLOW;
	} 
}

int cbuff_pop(void) {
	if (len == 0){
		return UNDERFLOW;
	}
    int out_nr = cbuff[out];
    out = (out + 1) % (CBUFF_SIZE);
    len--;
    	return out_nr;
}

int cbuff_state(void) {
	return len;

}

void cbuff_print(void) {
	int i = len;
    int curr_index = out;
    while (i) {
        printf("%d ", cbuff[curr_index]);
        curr_index = (curr_index + 1) % (CBUFF_SIZE);
        i--;
	}	
}

int main(void) {
	int to_do, n, client_no, answer;
	scanf("%d", &to_do);
	switch(to_do) {
		case 1: // stack
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = stack_push(n)) < 0) printf("%d ", answer);
				} else if (n < 0) {
					printf("%d ", stack_pop());
				} else printf("\n%d\n", stack_state());
			} while(n != 0);
			break;
		case 2: // FIFO queue with shifts
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = queue_push(n)) < 0) printf("%d ", answer);
				} else if (n < 0) {
					if ((answer = queue_pop(-n)) < 0) printf("%d ", answer);
				} else {
					printf("\n%d\n", queue_state());
					queue_print();
				}
			} while(n != 0);
			break;
		case 3: // queue with cyclic buffer
			client_no = 0;
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = cbuff_push(++client_no)) < 0) printf("%d ", answer);
				} else if (n < 0) {
					printf("%d ", cbuff_pop());
				} else {
					printf("\n%d\n", cbuff_state());
					cbuff_print();
				}
			} while(n != 0);
			break;
		default: 
			printf("NOTHING TO DO!\n");
	}
	return 0;
}
