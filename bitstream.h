#ifndef _ACM_LAB_BIT_STREAM_H
#define _ACM_LAB_BIT_STREAM_H

#include <stdio.h>

class CBitStream {
private:
	FILE* file;
	unsigned long accumulator; // not yet stored bits
	int shift; // next bit position
	unsigned long bits_written, bytes_written; // for statistics
	long seq_zeros; // count of sequential zeros

	void internal_flush(); // drops all the fully filled bytes
	void drop_zeros(); // flushes all the sequentially coming zero bytes
public:
	CBitStream (FILE* a_file) 
		: file (a_file),
		accumulator (0), shift (0),
		bits_written (0), bytes_written (0),
		seq_zeros (0) {};
	~CBitStream() {	flush(); };
	
	int init_bit_stream() { return 1; }; // no initialization in current version
	void flush(); // flushes the last, unfinished byte. MUST be called at the and of processing
	void write_bits (unsigned long value, int count);
	unsigned long get_bits_written() { return bits_written; };
	unsigned long get_bytes_written() { return bytes_written; };
};

#endif
