#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "bloom.h"

const int BLOOM_HASH1 = 17;
const int BLOOM_HASH2 = 29;

// compute a hash of a string using a seed value, where the result
// falls between zero and range-1
int hash_string(char * string, int seed, int range)
{
  int i;
  int hash = 0;

  // simple loop for mixing the input string
  for ( i = 0; string[i] != '\0'; i++ ) {
    hash = hash * seed + string[i];
  }
  // check for unlikely case that hash is negative
  if ( hash < 0 ) {
    hash = -hash;
  }
  // bring the hash within the range 0..range-1
  hash = hash % range;
  
  return hash;
}


// create a new, empty Bloom filter with 'size' slots
struct bloom * bloom_new(int size)
{
	struct bloom * new_bloom = malloc( sizeof( struct bloom ));
	new_bloom->bit_vector = bitset_new(size);
	new_bloom->size = size;
	return new_bloom;
	
}

// check to see if a string is in the set
int bloom_lookup(struct bloom * this, char * item)
{
	int hash_value = hash_string(item, BLOOM_HASH1, this->size);
	int there = bitset_lookup(this->bit_vector, hash_value);
	if (there==1) {
		hash_value = hash_string(item, BLOOM_HASH2, this->size);
		there = bitset_lookup(this->bit_vector, hash_value);
		if (there == 1 ) {
			return 1;
		}
	}
	return 0;
}

// add a string to the set has no effect if the item is already in the
// set; note that you cannot safely remove items from a Bloom filter
void bloom_add(struct bloom * this, char * item)
{
	bitset_add(this->bit_vector, hash_string(item,BLOOM_HASH1,this->size) );
	bitset_add(this->bit_vector, hash_string(item,BLOOM_HASH2,this->size) ); 

}

// place the union of src1 and src2 into dest
void bloom_union(struct bloom * dest, struct bloom * src1,
		  struct bloom * src2)
{
	bitset_union(dest->bit_vector, src1->bit_vector, src2->bit_vector );
}

// place the intersection of src1 and src2 into dest
void bloom_intersect(struct bloom * dest, struct bloom * src1,
                  struct bloom * src2)
{
	bitset_intersect(dest->bit_vector, src1->bit_vector, src2->bit_vector );
}

// print out the bits that are 1 in the bit vector of
// a bloom filter
void bloom_print(struct bloom * this)
{
  bitset_print(this->bit_vector);
}
