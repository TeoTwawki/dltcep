#include "stdafx.h"
// CValuePacker packs the values into an ACM-stream.

// IP's packing algorithm is not optimal. For full compatibility uncomment the next line
// #define FULL_IP_COMPAT
// No, do not uncomment. Not implemented yet.

#include <math.h>
#include <string.h>
#include "packer.h"
#include "bitstream.h"
#include "utils.h"

// Two different nearest-roundings. The first is more adequate, but it is slow.
#define ROUND(x) ((int) floor(0.5 + (double)(x)))
//#define ROUND(x) ((int) x)

// returns the approximate packed length of block (based on the max value in the block)
double approx_len (int max, int plus_max) {
	switch (max) {
		case 0: return 0;
		case 1: return 5.0/3;
		case 2: return 7.0/3;
		case 3: return 3.0;
		case 4: if (plus_max <= 3) return 3; else return 7.0/2;
		case 5: return 7.0/2;
		default: plus_max++;
			if (max < plus_max) max = plus_max;
			return (ceil (log ((double) max)/log ((double) 2)) + 1);
	}
}

void CValuePacker::analyse (const short* block) {
	long i, sub_number;
	const short *block_ptr = block;
	short *pblock_ptr = pblock;

	memset (max_abs, 0, sizeof(short)*sb_size);
	memset (max_plus, 0, sizeof(short)*sb_size);

	// 1. transfer the values into internal buffer and locating
	//    max. absolute and max. positive values in the columns
	for (i=0, sub_number=0; i<pblock_size; i++, block_ptr++, pblock_ptr++) {
		int abs_val = *block_ptr;
		*pblock_ptr = (short) abs_val;
		if (abs_val > 0) {
			if (max_plus[sub_number] < abs_val) max_plus[sub_number] = (short) abs_val;
		} else {
			abs_val = -abs_val;
		}
		if (max_abs[sub_number] < abs_val) max_abs[sub_number] = (short) abs_val;
		sub_number++;
		if (sub_number == sb_size) sub_number = 0;
	}

	// 2. try to find the best quantization value:
	//    it is the first value, using which we can fit information into specified bit-limit
	//    We begin from the Greatest Common Divisor (not necessary from 1).
	int val = GCD (pblock, pblock_size); // Greatest Common Divisor
	if (!val) val++; // if GCD is zero, turn it into one

	if (max_bits_limit != -1) {
	// This is not an optimal quantizer in any way (both in the sense of speed and the weighted distortion-rate function).
		// Try to implement bisection alg:
		// 1. looking for an initial range with growing step
		int init_val_was_ok = 1;
		int step = 8;
		while ( estimate (val) > max_bits_limit ) {
			val += (step << 1);
			step <<= 1;
			if (init_val_was_ok) init_val_was_ok = 0;
		}
		// 2. If initial approximation was insufficient, perform the binary search.
		if (!init_val_was_ok) {
			step -= 1;
			val -= step;
			while (step > 0) {
				int half_step = step >> 1;
				if ( estimate (val + half_step) > max_bits_limit ) {
					val += half_step + 1;
					step -= half_step + 1;
				} else
					step = half_step;
			}
		}
	}
	granulate (val);
}

double CValuePacker::estimate (int val) {
	double res = 0;
	for (int i=0; i<sb_size; i++)
		res += approx_len (ROUND(max_abs[i]/val), ROUND(max_plus[i]/val));
	res *= subblocks;
// Headers are not taken into consideration at the moment.
// No, they are! Because when sb_size is large, they cannot be ignored.
// No, they are not!
//	res += 20 + 5*sb_size; // size of headers
	return res;
}

void CValuePacker::granulate (int val) {
	int max = 0; // the maximum amplitude
	for (int i=0; i<pblock_size; i++) {
		int n = ROUND (pblock[i] / val); // "degranulate"
		pblock[i] = (short) n;
		n = (n<0)? -n: n+1;
		if (n > max) max = n;
	}

	int pwr = (int)ceil (log ((double) max)/log ((double) 2));
#ifdef FULL_IP_COMPAT
// In Interplay's ACMs the pwr is not less than 3:
	if (pwr < 3) pwr = 3;
#endif

	bit_stream->write_bits (pwr, 4);
	bit_stream->write_bits (val, 16);
}

enum k_enum {K13, K12, K24, K23, K35, K34, K45, K44};

void CValuePacker::pack_column (int col) {
	int p0 = 0, p_all_0 = 0, p00 = 0, p1 = 0;  // count of single zero, all zeros, pairs of zeros, and ones
	int max_amp = 0;  // max absolute amplitude
	int max_plus_amp = 0;
	int p00_x3, pall0_x3; // triple p00 and p_all_0 (user very frequently)
	int i;
	short* current = pblock + col;

	// 1. collecting statistics
	for (i=0; i<subblocks; i++, current += sb_size) {
		if (*current == 0) {
			p_all_0++; // some zero
			if (current[sb_size] == 0) {
				p00++ ; // doubled zero
				i++;
				current += sb_size; // skip the next item
				if (i<subblocks) p_all_0++; // but remeber, it was a zero
			} else
				p0++; // single zero
		} else {
			int abs_val = *current;
			if (abs_val > 0) {
				if (max_plus_amp < abs_val) max_plus_amp = abs_val;
			} else {
				abs_val = -abs_val;
			}
			if (max_amp < abs_val) max_amp = abs_val;
			if (abs_val == 1) p1++; // one more one
		}
	}

	// 2. thinking about gained results
	p00_x3 = p00 * 3;
	pall0_x3 = p_all_0 * 3;
	switch (max_amp) {
		case 0: bit_stream->write_bits (0, 5); break;// ZeroFill
		case 1:
			if (p00_x3 > subblocks) 	make_k (K13, col);
			else if (pall0_x3 > subblocks)	make_k (K12, col);
			else                            make_t15 (col);
			break;
		case 2:
			if (p00_x3 > subblocks) 	make_k (K24, col);
			else if (pall0_x3 > subblocks)	make_k (K23, col);
			else                            make_t27 (col);
			break;
		case 3:
			if (p00_x3 > subblocks) 	make_k (K35, col);
			else if (pall0_x3 + p1 > subblocks) make_k (K34, col);
			else                            make_linear (3, col);
			break;
		case 4:
		if (max_plus_amp <= 3) {
			if (p00_x3 > subblocks) 	make_k (K45, col);
			else if (pall0_x3 > subblocks)	make_k (K44, col);
			else                            make_linear (3, col);
			break;
		} else {
			if (p00_x3 > subblocks) 	make_k (K45, col);
			else if (2*pall0_x3 > subblocks) make_k (K44, col);
			else                            make_t37 (col);
		}
			break;
		case 5:  make_t37 (col); break;
		default:
			max_plus_amp++;
			if (max_amp < max_plus_amp) max_amp = max_plus_amp;
			int pwr = (int)ceil (log ((double) max_amp)/log ((double) 2));
			make_linear (pwr+1, col);
	}
}

void CValuePacker::add_one_block (const short* block) {
	analyse (block);
	for (int i=0; i<sb_size; i++)
		pack_column (i);
}

int CValuePacker::init_packer() {
	pblock = new short [pblock_size + 2*sb_size]; // two more lines (are always zero)
	if (!pblock) return 0;
	bit_stream = new CBitStream (file); if ( !bit_stream || !bit_stream->init_bit_stream() ) return 0;
	for (int i=0; i<2*sb_size; i++) pblock[pblock_size + i] = 0;
	max_abs = new short [sb_size]; if (!max_abs) return 0;
	max_plus = new short [sb_size]; if (!max_plus) return 0;
	return 1;
}

long CValuePacker::flush_bit_stream() {
	bit_stream->flush();
	return ( bit_stream->get_bytes_written() );
}

struct one_val { char bits; char val; };
struct maker_desc {
	char number;
	int double_zero;
	char base;
	one_val* data;
};
// Values:         -4     -3     -2      -1       0      1       2       3       4
one_val k13v[] = {                      {3,3},  {2,1}, {3,7}                         },
        k12v[] = {                      {2,1},  {1,0}, {2,3}                         },
        k24v[] = {              {4,3},  {4,7},  {2,1}, {4,11}, {4,15}                },
        k23v[] = {              {3,1},  {3,3},  {1,0}, {3,5},  {3,7}                 },
        k35v[] = {       {5,7}, {5,15}, {4,3},  {2,1}, {4,11}, {5,23}, {5,31}        },
        k34v[] = {       {4,3}, {4,7},  {3,1},  {1,0}, {3,5},  {4,11}, {4,15}        },
        k45v[] = {{5,3}, {5,7}, {5,11}, {5,15}, {2,1}, {5,19}, {5,23}, {5,27}, {5,31}},
        k44v[] = {{4,1}, {4,3}, {4,5},  {4,7},  {1,0}, {4,9},  {4,11}, {4,13}, {4,15}};
maker_desc k_desc[] = {
	{17, 1, 1, k13v}, {18, 0, 1, k12v},
	{20, 1, 2, k24v}, {21, 0, 2, k23v},
	{23, 1, 3, k35v}, {24, 0, 3, k34v},
	{26, 1, 4, k45v}, {27, 0, 4, k44v}
};

void CValuePacker::make_k (int ind, int col) {
	int double_zero = k_desc[ind].double_zero;
	char base = k_desc[ind].base;
	one_val* data = k_desc[ind].data;
	short* curr = pblock + col;

	bit_stream->write_bits (k_desc[ind].number, 5);
	for (int i=0; i<subblocks; i++, curr += sb_size) {
		if (double_zero && (curr[0] == 0) && (curr[sb_size] == 0)) {
			bit_stream->write_bits (0, 1);
			i++; curr += sb_size;
		} else {
			one_val item = data[base + *curr];
			bit_stream->write_bits (item.val, item.bits);
		}
	}
}

void CValuePacker::make_linear (int bits, int col) {
	short base = (short) (1 << (bits-1));
	short* curr = pblock + col;

	bit_stream->write_bits (bits, 5);
	for (int i=0; i<subblocks; i++, curr += sb_size) {
		bit_stream->write_bits (base + *curr, bits);
	}
}

void CValuePacker::make_t15 (int col) {
	short *curr = pblock + col;
	int step = sb_size*3;

	bit_stream->write_bits (19, 5);
	for (int i=0; i<subblocks; i+=3, curr+=step) {
		int val = (1 + *curr) + (1 + curr[sb_size])*3 + (1 + curr[2*sb_size])*9;
		bit_stream->write_bits (val, 5);
	}
}

void CValuePacker::make_t27 (int col) {
	short *curr = pblock + col;
	int step = sb_size*3;

	bit_stream->write_bits (22, 5);
	for (int i=0; i<subblocks; i+=3, curr+=step) {
		int val = (2 + *curr) + (2 + curr[sb_size])*5 + (2 + curr[2*sb_size])*25;
		bit_stream->write_bits (val, 7);
	}
}

void CValuePacker::make_t37 (int col) {
	short *curr = pblock + col;
	bit_stream->write_bits (29, 5);
	for (int i=0; i<subblocks; i+=2, curr+=2*sb_size) {
		int val = (5 + *curr) + (5 + curr[sb_size])*11;
		bit_stream->write_bits (val, 7);
	}
}
