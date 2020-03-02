// C code file for  a file ADT where we can read a single bit at a
// time, or write a single bit at a time

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "bitfile.h"

// open a bit file in "r" (read) mode or "w" (write) mode
struct bitfile * bitfile_open(char * filename, char * mode)
{
	struct bitfile * result;
	result = malloc( sizeof( struct bitfile ) );
	result -> buffer = 0;
	result -> index = 0;
	result -> file = fopen(filename, mode);
	return result;
}

// write a bit to a file; the file must have been opened in write mode
void bitfile_write_bit(struct bitfile * this, int bit)
{
	if (bit==1 || bit==0) {
		
		//add the passed bit to the buffer at the position of the index
		this->buffer = this->buffer | bit<<this->index;
		this->index++;

		//if the index is greater than the size of a byte 
		assert(this->index<=8);
		if (this->index==8) {
			//print out the buffer and reset
			//printf("buffer: %d", this->buffer);
			fputc(this->buffer, this->file);
			this->index=0;
			this->buffer=0;
		}
	}
}

// read a bit from a file; the file must have been opened in read mode
int bitfile_read_bit(struct bitfile * this)
{
	assert(this->index<=8);
	if (this->index==8) {
		this->buffer = fgetc(this->file);
		//printf("buffer = %d\n", this->buffer);
		this->index=0;
	}

	unsigned char tmp = 0;
	tmp = this->buffer>>(this->index);
	int temp = (tmp & 1);	//gets the individual bit to send from the code(byte)
	this->index++;
	return temp;	
}

// close a bitfile; flush any partially-filled buffer if file is open
// in write mode
void bitfile_close(struct bitfile * this) {
	if (this->index != 0 ) {
		fputc(this->buffer, this->file);
		this->index=0;
		this->buffer=0;
	}
	fclose(this->file);
}

// check for end of file
int bitfile_end_of_file(struct bitfile * this)
{
	if ( feof(this->file) ) {
		return 1;
	} else {
		return 0;
	}
}

