#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct bitset {
	unsigned int * bits; //bit vector
	int size_in_bits;
	int size_in_words;
	int bits_per_word;
};
//flip bit b of input
unsigned flip_bit(unsigned input, int b) {
	unsigned mask = 1<<b;
	return input^mask;
}

//clear bit b of input
unsigned clear_bit( unsigned input, int b) {
	//unsigned mask =0-1 //twos complement all 1s
	unsigned mask = ~(1<<b);
	return input & mask;
}

//set bit of input
unsigned set_bit (unsigned input, int b) {
	unsigned mask = 1<<b;
	return input | mask;
}

//return the value of a bit index
int get_bit(unsigned num, int index) {
	return (num>>index) & 1;
}

//create new empty bitset
struct bitset * bitset_new(int size) {
	struct bitset * result;
	int size_in_words, i, bits_per_word;
	result = malloc( sizeof( struct bitset ) );
	bits_per_word = sizeof( unsigned int ) * 8 ;
	size_in_words = size / bits_per_word;
	if ( size % bits_per_word != 0 ) {
		size_in_words++;
	}
	result->bits = malloc( sizeof( unsigned int ) * size_in_words );
	for ( i=0; i<size_in_words; i++) {
		result->bits[i]=0;
	}
	result->bits_per_word = bits_per_word;
	result->size_in_bits = size;
	result->size_in_words = size_in_words;
	return result;
}

// get the size of the universe of items that could be stored in the set
int bitset_size(struct bitset * this) {
	return this->size_in_bits;
}

// get the number of items that are stored in the set
int bitset_cardinality(struct bitset * this) {
	int i, j, count=0;
	for ( i=0; i<this->size_in_words; i++ ) {
		for (j=0; i<this->size_in_bits; j++ ) {
			if( get_bit(this->bits[i], j) == 1) {
				count++;
			}
		}
	}
	return count;
}

// check to see if an item is in the set
int bitset_lookup(struct bitset * this, int item) {
	int word_index = item / this->bits_per_word;
	int bit_index = item % this->bits_per_word;
	if ( get_bit( this->bits[word_index], bit_index ) > 0 ) {
		return 1;
	}
	return 0;
}


// add an item, with number 'item' to the set
// has no effect if the item is already in the set
void bitset_add(struct bitset * this, int item) {
	assert( item >= 0 && item < this->size_in_bits );
	int word_index = item / this->bits_per_word;
	int bit_index = item % this->bits_per_word;
	this->bits[word_index] = set_bit( this->bits[word_index], bit_index );
}

// remove an item with number 'item' from the set
void bitset_remove(struct bitset * this, int item) {
	assert( item >= 0 && item < this->size_in_bits );
	int word_index = item / this->bits_per_word;
	int bit_index = item % this->bits_per_word;
	this->bits[word_index] = clear_bit( this->bits[word_index], bit_index );
}

//create a new set that is the union of 2 sets
void bitset_union(struct bitset * dest, struct bitset * src1, struct bitset * src2) {	
	assert(src1->size_in_bits == src2->size_in_bits && src2->size_in_bits == dest->size_in_bits );
	int i;
	for ( i=0; i<src1->size_in_words; i++ ) {
		dest->bits[i] = src1->bits[i] | src2->bits[i] ;
	}
}

// place the intersection of src1 and src2 into dest
void bitset_intersect(struct bitset * dest, struct bitset * src1, struct bitset * src2) {
	assert(src1->size_in_bits == src2->size_in_bits && src2->size_in_bits == dest->size_in_bits );
	int i;
	for ( i=0; i<src1->size_in_words; i++ ) {
		dest->bits[i] = src1->bits[i] & src2->bits[i] ;
	}
}

// print the contents of the bitset
void bitset_print(struct bitset * this)
{
  int i;
  int size = bitset_size(this);
  for ( i = 0; i < size; i++ ) {
    if ( bitset_lookup(this, i) == 1 ) {
      printf("%c ", i);
    }
  }
  printf("\n");
}

// add the characters from a string to a bitset
void add_chars_to_set(struct bitset * this, char * s)
{
  int i;
  for ( i = 0; s[i] != 0; i++ ) {
    unsigned char temp = s[i];
    bitset_add(this, temp);
  }
}

// small routine to test a bitset
void mytest()
{
  struct bitset * a = bitset_new(256);
  struct bitset * b = bitset_new(256);
  struct bitset * c = bitset_new(256);
  char * string1 = "What can you hear";
  char * string2 = "Nothing but the rain";

  add_chars_to_set(a, string1);
  add_chars_to_set(b, string2);

  // print the contents of the sets
  bitset_print(a);
  bitset_print(b);

  // compute and print the union of sets
  bitset_union(c, a, b);
  bitset_print(c);

  // compute and print the intersection of sets
  bitset_intersect(c, a, b);
  bitset_print(c);
}

int main() {

	mytest();
	return 1;
}

