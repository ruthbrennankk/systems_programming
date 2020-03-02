#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/* READING IN A FILE */

/* Open a file. Abort program if file cannot be opened */
FILE * open_file_r(char filename[])
{
  FILE * file;
  file = fopen(filename, "r");
  if ( file == NULL ) {
    printf("FATAL: Error opening file %s. Aborting program.\n", filename);
    exit(1);
  }

  return file;
}

/* If this function encounters a newline character '\n' 
or the end of the file EOF before they have read the maximum number of characters, 
then it returns only the characters read up to that point including the new line character */
void read_in(char buff[], int size, FILE *file)
{
  fgets(buff, size, file);
  //printf("1: %s\n", buff);
  
  fclose(file);
}

/*	WRITING OUT TO FILE	*/

FILE * open_file_w(char filename[])
{
  FILE * file;
  file = fopen(filename, "w+");
  if ( file == NULL ) {
    printf("FATAL: Error opening file %s. Aborting program.\n", filename);
    exit(1);
  }

  return file;
}

void write_out(char buff[], char filename[]) {

  FILE * file = open_file_w(filename);
  
  fputs(buff, file);
  
  fclose(file);

}



/* LISTS, PUSH AND POP	*/

struct listnode {

    double data;
    struct listnode *next;

};


struct list {

    struct listnode *head;

};


struct list *new_empty_list() {

    struct list *result;
    result = malloc(sizeof(struct list));
    result -> head = NULL;
    return result;

}

//void add_to_front_of_list(struct list *this, int data) 
void push(struct list *this, double data) {

	struct listnode *node = malloc(sizeof(struct listnode));
	node->data=data;
	node->next = this->head;
	this->head=node;

}

//int remove_from_front_of_list(struct *this) 
double pop(struct list *this) {

	//set head to node->next
	struct listnode *node = this->head;
	this->head=node->next;
	//retrieve data to return
	double data = node->data;
	//release space in memory
	free(node);
	return data;

}

/* CALC	*/
double postfixcal (char a[]) {

    struct list *stack = new_empty_list();
    bool space = true;	//boolean
    //int size_of_array = strlen(a)-1;
    char token;
    for (int i=0; i<strlen(a); i++) {
	token = a[i];
	if (token >= 48 && token <=57) {	//is a number
		if (!space) {
			double num =(double)(token-48);
			push(stack, ((pop(stack)*10.0) + num));
		} else {
			double num = (double)(token-48);
			push(stack, num );
		}
		space=false;
	} else if (token == ' ') {
		space=true;
	} else 
	{
		double b, a;
        	switch (token) {
			case 94: //^
				b = pop(stack);
				a = pop(stack);
                		push(stack, pow( a, b ) );
                		break;
			case 42: //*
			case 88: //X
              	 		push(stack, pop(stack)*pop(stack));
             	  		break;
           		case 47: // /
				b = pop(stack);
				a = pop(stack);
                   		push(stack, a/b);
                    		break;
        		case 43: //+
        	        	push(stack, pop(stack)+pop(stack));
        	        	break;
        		case 45: //-
        	        	b = pop(stack);
				a = pop(stack);
                   		push(stack, a-b);
        	        	break;
            		
        	} 
		space=false;
	}
    }
	struct listnode *node = stack->head;
	return node->data;
	
}

/*	Infix	*/
int precedence(char token) {
	int p=0; 
	int ascii = (int) token;
	switch (ascii) {
		case 94:
			p=5;
			break;
		case 42:
		case 88:
			p=4;
			break;
		case 47:
			p=3;
			break;
		case 43:
			p=2;
			break;
		case 45:
			p=1;
			break;
	}
	return p;
}

void convert(char buff[], char converted[]) {	
	struct list *stack = new_empty_list();
	char token;
	int j=0;
	for (int i=0; i<strlen(buff)-1; i++) {
		token = buff[i];	
		if (token >= 48 && token <=57) {
			converted[j++]=token;
		} else if (token == '(' ) { 
			push(stack, token);
		} else if (token=='^' || token=='*' || token=='X' || token=='/' || token=='+' || token=='-' ) {
				struct listnode *node = stack->head;
				while ( node!=NULL && precedence(token) <= precedence(node->data)) {
					converted[j++]=pop(stack);
					node=stack->head;
				} 	
			push(stack, token);
		} else if (token == ')' ) {
			struct listnode *node = stack->head;
			while( node->data != '(' ) {
				converted[j++] = pop(stack);
				node = stack->head;
			}
			pop(stack);
		} else if (token == ' ') {
			converted[j++] = ' ';
		}
	}

	struct listnode *node = stack->head;
	while(node){
        	converted[j++] = pop(stack);
       		node=stack->head;
	}
	converted[strlen(buff)-2]='\0';
}


int main(int argc, char ** argv) {
	char * filename;

	if ( argc == 1 ) {
		printf("Error: No input filename provided\n");
		printf("Usage: %s <input filename>\n", argv[0]);
		exit(1);
	}
	else if ( argc > 2 ) {
		printf("Error: Too many command line parameters\n");
		printf("Usage: %s <input filename>\n", argv[0]);
		exit(1);
	}
	else {
		filename = argv[1];
	}

	const int size = 100;
	char buff[size];
	char * filename_out;

	FILE * file = open_file_r(filename);

	char ext[] = ".results";
	filename[strlen(filename)] = '\0';
	strcpy(filename_out, filename);
	strcat(filename_out, ext);
	FILE * fileOut = open_file_w(filename_out); 

	while (fgets(buff, size, file)!=0) {
		fprintf(fileOut, "%s", buff);
		double result=9;	
		//char equation[size];

		if (buff[0]=='i' || buff[0]=='I') {

			// Infix Read In 
			char converted[strlen(buff)-1];
			memset(converted, 0, sizeof(converted) );
			convert(buff, converted);
			//fprintf(fileOut, "The converted is :%s \n", converted);
			result = postfixcal(converted);
		} else {

			//Postfix Read in 
			result =  postfixcal(buff);

		}
		fprintf(fileOut, "%g\n", result);
		
		
	}

	fclose(file); fclose(fileOut);
	return 0;
}






