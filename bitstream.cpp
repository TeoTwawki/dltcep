#include "stdafx.h"
// CBitStream class provides the bit-output capabilities.

#include <stdio.h>
#include <io.h>
#include "bitstream.h"

void CBitStream::internal_flush() {
	int bytes_ready = shift >> 3;
	int tail_zeros = 0;
	if (bytes_ready) {
		for (int i=0; i<bytes_ready; i++)
			if ( ((char*)&accumulator)[i] == 0 ) tail_zeros++; else tail_zeros = 0;
		if (bytes_ready > tail_zeros) {
			if (seq_zeros) drop_zeros();
			fwrite (&accumulator, bytes_ready-tail_zeros, 1, file);
			bytes_written += bytes_ready-tail_zeros;
		}
		seq_zeros += tail_zeros;
		bytes_ready <<= 3; // making bits instead of bytes
		if (bytes_ready == 32)
			accumulator = 0;
		else
			accumulator >>= bytes_ready;
		shift -= bytes_ready;
	}
}
void CBitStream::write_bits (unsigned long value, int count) {
	bits_written += count;

	int total_bits = count + shift,
		not_fit_bits = (total_bits > 32) ? total_bits - 32: 0;
	count -= not_fit_bits;
	unsigned long mask = (count == 32)? (0xFFFFFFFF): (1L << count) - 1;
	accumulator |= ((value & mask) << shift);
	shift += count;
	internal_flush();

	if (not_fit_bits) {
		value >>= count;
		mask = (1L << not_fit_bits) - 1;
		accumulator |= ((value & mask) << shift);
		shift += not_fit_bits;
		internal_flush();
	}
}
void CBitStream::flush() {
	internal_flush();
	if (shift) {
		accumulator &= ((1L << shift) - 1);
		if (accumulator) {
			drop_zeros();
			fwrite (&accumulator, 1, 1, file);
			bytes_written++;
			fflush (file);
		}
		accumulator = 0; // these values must be zero now
		shift = 0;
	}
}
void CBitStream::drop_zeros() {
	long zero = 0;
	while (seq_zeros) {
		int to_write = (seq_zeros>4)? 4: seq_zeros;
		fwrite (&zero, to_write, 1, file);
		bytes_written += to_write;
		seq_zeros -= to_write;
	}
}