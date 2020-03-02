// code for a huffman coder
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "huff.h"


// create a new huffcoder structure
struct huffcoder *  huffcoder_new()
{
	struct huffcoder *coder = malloc( sizeof(struct huffcoder) );
	coder->tree = malloc( sizeof(struct huffchar) );
	return coder;
}

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffcoder_count(struct huffcoder * this, char * filename)
{
	FILE * file = fopen(filename, "r");
	assert( file != NULL );
	unsigned char c;  // we need the character to be unsigned to use it as an index
  	c = fgetc(file);	// attempt to read a byte
  	while( !feof(file) ) {
    		printf("%c", c);
		int i = c;
		this->freqs[i]++;
    		c = fgetc(file);
  	}
	for(int i=0; i<NUM_CHARS; i++) {
		if (this->freqs[i]==0)
			this->freqs[i]=1;
	}
  	fclose(file);
}

void swap(struct huffchar *one, struct huffchar *two) 
{ 
    struct huffchar tmp = *one; 
    *one = *two; 
    *two = tmp; 
} 
  
void selectionSort(struct huffchar *arr[], int n) 
{ 
    int i, j, min_idex; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < 2; i++) 
    { 
        // Find the minimum element in unsorted array 
        min_idex = i; 
        for (j = i+1; j < n; j++) //should take sequence number into account here
          if (arr[j]->freq < arr[min_idex]->freq || (arr[j]->freq == arr[min_idex]->freq && arr[j]->seqno == arr[min_idex]->seqno ) ) 
            min_idex = j; 
  
        // Swap the found minimum element with the first element 
        swap(arr[min_idex], arr[i]); 
    } 
} 


// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffcoder_build_tree(struct huffcoder * this)
{
	int n=NUM_CHARS;
	//make a list of all huff characters
	struct huffchar *char_list[n]; //not sure if struct should be here
	for (int i=0; i<n; i++) {	//not entirely confident that NUM_CHARS does not need to be defined here 
		char_list[i]=malloc(sizeof(struct huffchar));
		char_list[i]->freq = this->freqs[i];
		char_list[i]->is_compound = 0;
		char_list[i]->u.c = (unsigned char) i;
		//printf("char_list @ %d = %u\n", i, char_list[i]->u.c);
		char_list[i]->seqno = i;
		char_list[i]->u.compound.right=NULL;
		char_list[i]->u.compound.left=NULL;
	}
	while (n > 0) {
		//get lowest two frequencies
		selectionSort(char_list, n);

		//make a compound character
		struct huffchar *ch = malloc( sizeof(struct huffchar) );
		ch->freq = char_list[0]->freq + char_list[1]->freq;
		//printf("The freq of a compund character of  (size %d) ([0]->freq %d) + ([1]->freq %d) = %d\n", n, char_list[0]->freq, char_list[1]->freq, ch->freq);
		ch->is_compound=1;
		ch->seqno = char_list[0]->seqno + char_list[1]->seqno;
		ch->u.compound.left=char_list[0];
		ch->u.compound.right=char_list[1];

		//add compound character to the list and delete the used characters
		char_list[0]=ch;
		int i;
		for (i=1; i+1<n; i++) {
			char_list[i]=char_list[i+1];
		}
		n--;
	}
	this->tree = char_list[0];
	//int tmp = this->tree->freq;
	//printf("tree built, the root freq = %d\n", tmp);
}

// recursive function to convert the Huffman tree into a table of
// Huffman codes
void tree2table_recursive(struct huffcoder * this, struct huffchar * node, int * path, int depth)
{
	if (node != NULL) {
		if (node->u.compound.left != NULL) {
			//change path but don't
			//path[depth]=0;
			int * temp = malloc( sizeof(depth+1) );
			for ( int i=0; i<depth; i++ ) {
				temp[i]=path[i];
			}
			temp[depth] = 0;
			tree2table_recursive(this, node->u.compound.left, temp, depth+1);
			
		} 
		if (node->u.compound.right != NULL) {
			//change path
			//path[depth]=1;
			int * temp = malloc( sizeof(depth+1) );
			for ( int i=0; i<depth; i++ ) {
				temp[i]=path[i];
			}
			temp[depth] = 1;
			tree2table_recursive(this, node->u.compound.right, temp, depth+1);
		} 
		if (node->u.compound.left==NULL && node->u.compound.right==NULL) {
			this->code_lengths[node->seqno] = depth;
			//int n = sizeof(path)/sizeof(path[0]);
			unsigned long long tmp = 0;
			for (int i=0; i<depth; i++) {
				tmp = tmp << 1;
				tmp = tmp | path[i];
			}
			this->codes[node->seqno] = tmp;
			//printf("this->codes @ %d = %llu\n", node->seqno, this->codes[node->seqno]);
		}
	}
	
}

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffcoder_tree2table(struct huffcoder * this)
{
	tree2table_recursive(this, this->tree, 0, 0);
	//fprintf( "@32 should be 7 is %llu", this->codes[32] );	
} 

// print the Huffman codes for each character in order
void huffcoder_print_codes(struct huffcoder * this)
{
  int i, j;
  char buffer[NUM_CHARS];

  for ( i = 0; i < NUM_CHARS; i++ ) {
    // put the code into a string
    for ( j = this->code_lengths[i]-1; j >= 0; j--) {
      buffer[j] = ((this->codes[i] >> (this->code_lengths[i]-j-1)) & 1) + '0';
    }
    // don't forget to add a zero to end of string
    buffer[this->code_lengths[i]] = '\0';

    // print the code
    printf("char: %d, freq: %d, code: %s\n", i, this->freqs[i], buffer);
  }
}







