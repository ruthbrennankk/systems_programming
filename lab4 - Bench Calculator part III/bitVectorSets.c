struct bitset {
	unsigned int * bits;
	int size_int_bits;
	int size_in_words;
	int bits_per_word;
}

//create new empty bitset
struct bitset *initalise_bitset (int size) {
	struct bitset *result;
	int size_in_words, i, bits_per_word;
	result = malloc( sizeof(struct bitset) );
	bits_per_word = ( sizeof(unsigned int) *8);
	size_in_words = size / bits_per_words;
	if ( size % bits_per_word != 0 ) {
		size_in_words++;
	}
	result->bits=malloc( sizeof( unsigned int ) * size_in_words );
	for (i=0; i<size_in_words; i ++) {
		result->bits[I]
	}
}

