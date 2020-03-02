// code for a huffman coder

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "huff.h"
#include "bitfile.h"


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
    		//makeprintf("%c", c);
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
        for (j = i+1; j < n; j++) {
          if ( arr[j]->freq < arr[min_idex]->freq || (arr[j]->freq == arr[min_idex]->freq && arr[j]->seqno < arr[min_idex]->seqno ))  {
			 min_idex = j; 
	  }
  	}
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
	struct huffchar *char_list[n];
	for (int i=0; i<n; i++) {
		char_list[i]=malloc(sizeof(struct huffchar));
		char_list[i]->freq = this->freqs[i];
		char_list[i]->is_compound = 0;
		char_list[i]->u.c = (unsigned char) i;
		char_list[i]->seqno = i;
	}
	int seq_count=NUM_CHARS;

	while (n > 1) {
		//get lowest two frequencies
		selectionSort(char_list, n);

		//make a compound character
		struct huffchar *ch = malloc( sizeof(struct huffchar) );
		ch->freq = char_list[0]->freq + char_list[1]->freq;
		ch->is_compound=1;
		ch->seqno = seq_count;
		ch->u.compound.left=char_list[0];
		ch->u.compound.right=char_list[1];
		seq_count++;

		//add compound character to the list and delete the used characters and reduce size of array by 1
		char_list[0]=ch;
		for (int i=1; i+1<n; i++) {
			char_list[i]=char_list[i+1];
		}
		n--;
	}
	this->tree = char_list[0];
}

// recursive function to convert the Huffman tree into a table of
// Huffman codes
void tree2table_recursive(struct huffcoder * this, struct huffchar * node, int * path, int depth)
{
	if (node != NULL) {
		 if (node->is_compound == 1) {
			if (node->u.compound.left != NULL) {
				//change path 
				int * temp = malloc( sizeof(int) *depth +1 );
				for ( int i=0; i<depth; i++ ) {
					temp[i]=path[i];
				}
				temp[depth] = 0;
				tree2table_recursive(this, node->u.compound.left, temp, depth+1);
				
				temp[depth] = 1;
				tree2table_recursive(this, node->u.compound.right, temp, depth+1);
			} 
		}
		else { // (node->is_compound == 0) {
			this->code_lengths[node->seqno] = depth;
			unsigned long long tmp = 0;
			for (int i=0; i<depth; i++) {
				tmp = tmp << 1;
				tmp = tmp | path[i];
			}
			this->codes[node->seqno] = tmp;
		}
	}
	
}

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffcoder_tree2table(struct huffcoder * this)
{

	tree2table_recursive(this, this->tree, 0, 0);
}


// print the Huffman codes for each character in order;
// you should not modify this function
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


// encode the input file and write the encoding to the output file
void huffcoder_encode(struct huffcoder * this, char * input_filename, char * output_filename)
{
	//initalise the input and output bitfiles
	struct bitfile * output_bitfile = bitfile_open(output_filename, "w");
	struct bitfile * input_bitfile = bitfile_open(input_filename, "r");

	//read the input file in byte by byte (ie character by character)
	assert( input_bitfile->file != NULL );
	unsigned char c;
	c = fgetc(input_bitfile->file); //get first character
  	while( !feof(input_bitfile->file) ) { //check that we are not at the end of the file
		//printf("%c\n", c);		
		//get the code and write them out bit by bit
		unsigned long long code = this->codes[c];
		unsigned long long tmp = 0;
		int length = this->code_lengths[c];

		for (int i=length; i>0; i--) {
			tmp = code>>(i-1);
			int temp = (tmp & 1);	//gets the individual bit to send from the code(byte)
			bitfile_write_bit(output_bitfile, temp);
			tmp=0;
		}
		c = fgetc(input_bitfile->file); //get next character
	}

	unsigned long long code = this->codes[4];
	unsigned long long tmp=0;
	int length = this->code_lengths[4];
	for (int i = length; i > 0; i--) {
		tmp = code >> (i-1);
		int temp = (tmp & 1);
		bitfile_write_bit(output_bitfile, temp);
		tmp=0;
	}
	
	bitfile_close(input_bitfile);
	bitfile_close(output_bitfile);
}


// decode the input file and write the decoding to the output file
void huffcoder_decode(struct huffcoder * this, char * input_filename, char * output_filename)
{
	//initalise the input and output bitfiles
	struct bitfile * output_bitfile = bitfile_open(output_filename, "w");
	struct bitfile * input_bitfile = bitfile_open(input_filename, "r");

	//loop though until the end of the input file
	input_bitfile->index=8;
	int done =0;
	while ( !feof(input_bitfile->file) && !done) {

		//traverse through the tree until we reach a non-compund huffchar
		struct huffchar * current = this->tree;
		while(current->is_compound==1){
			int bit = bitfile_read_bit(input_bitfile);
			//printf("%d",bit);
			if (bit==0) 
				current = current->u.compound.left;
			else 
				current = current->u.compound.right;
		} 
		
		//print the non-compund huffchar character
		assert(current->is_compound==0);
		if (current->seqno == 4) {
			done=1;
		} else {
			fputc(current->u.c, output_bitfile->file);
			//printf(" \n");
		}
		
	}
	bitfile_close(output_bitfile);
	bitfile_close(input_bitfile);	
}






