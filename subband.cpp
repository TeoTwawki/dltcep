#include "stdafx.h"
// Pyramidal Subband Coding Scheme.

// Based on:
// Edward H. Adelson, Eero P. Simoncelli.  Subband Image Coding with
//    Three-tap Pyramids.  Picture Coding Symposium, 1990.  Cambridge, MA.
//    [ftp://ftp.cis.upenn.edu/pub/eero/EPIC.ps.Z]

// This module contains the following filters:
// 1. "RZF-filter" introduces the error only in the first and the last point.
// 2. "Delta-optimal" filter minimizes the norm( IPF*Delta_Opt_Flt - delta ),
//    where IPF is the matrix of [1,2,1] subband decoding.
// 3. Custom filter allows loading the filter coeffs from a text file.
//    This file has the following structure:
//       integer count of coeffs (up to central tap of a filter) as the first value,
//       next values contain the tap values (floating point numbers).
//    The signs of the values do not matter, they are corrected automatically.

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "subband.h"
#include "general.h"

int CSubbandCoder::init_filter() {
	f_len = (f_half << 1) - 1;
	subbands = (1 << levels) - 1; // ???

	// allocating necessary structures
	if ( !allocate_coeffs() ) return 0;
	if (levels == 0) return 1; // if there are no levels, we do not need any structures
	incomp = new unsigned char [f_len * subbands * get_incomp_size()]; if (!incomp) return 0;// each subband has its own incompete values
	ques = new FilterQueue [subbands]; if (!ques) return 0;
	levs = new FilterLevel [levels]; if (!levs) return 0;
	clear_filter();
	return 1;
}
void CSubbandCoder::clear_filter() {
	int i;
	if (levels == 0) return; // if there are no levels, we do not need to initialize anything

	long i_size = get_incomp_size(); // incomplete item size
	memset (incomp, 0, i_size * f_len * subbands);
	memset (ques, 0, sizeof (FilterQueue) * subbands);
	memset (levs, 0, sizeof (FilterLevel) * levels);

	for (i=0; i<subbands; i++)
		ques[i]. cur = ( ques[i]. incomp = incomp + f_len*i*i_size );
	FilterQueue* curr = ques;
	for (i=0; i<levels; i++) {
		levs[i]. cur = ( levs[i]. first = curr );
		curr += ( levs[i]. count = 1 << i );
	}
}
long CSubbandCoder::get_init_size() {
	return f_half * subbands;
}
long CSubbandCoder::filter_data (short* data, long count, long* res_data) {
	short* cur_inp = data;
	long* cur_out = res_data;
	for (int i=0; i<count; i++, cur_inp++, cur_out++)
		*cur_out = *cur_inp;// << levels;***
	if (levels == 0) return count; // when there are no levels, there ain't no transformation

	long i_size = get_incomp_size();
	FilterLevel* f_lev = levs;
	for (int l=0; l<levels; l++) {
		long res = 0;
		long *input = res_data,
			*output = res_data;
		FilterQueue* f_que = f_lev->cur;
		for (int i=0; i<count; i++) {
			long next_val = add_value (*input, f_que);
			// make an output, if available
			if (f_que->input_count >= f_half) {
				*output = next_val;
//				if (l == levels-2 && f_lev->cur_no == 0)
//					output[0] -= 1;
				output++;
				res++;
			}

			// advance the current queue
			memset (f_que->cur, 0, i_size);
			f_que->cur_no++; f_que->cur += i_size;
			if (f_que->cur_no == f_len) {
				f_que->cur_no = 0;
				f_que->cur = f_que->incomp;
			}
			f_que->input_count++;
			f_que->is_odd = !f_que->is_odd;

			// advance the input
			input++;

			// switch the queue
			if (l != 0) { // at zero level no switching is made, 'cause it has only one subband
				f_que++; f_lev->cur++;
				f_lev->cur_no++;
				if (f_lev->cur_no == f_lev->count) {
					f_lev->cur_no = 0;
					f_que = ( f_lev->cur = f_lev->first );
				}
			}
		}
		count = res;
		f_lev++;
	}
	return count;
}


int C_DRZF_SubbandCoder::allocate_coeffs() {
	lp_filter = new double [f_len]; if (!lp_filter) return 0;
	hp_filter = new double [f_len]; if (!hp_filter) return 0;

	int i;
	double prev_0 = 0, prev_1 = 1, cur;
	memset (lp_filter, 0, sizeof (double) * f_len);
	lp_filter[0] = 1;
	for (i=1; i<f_half; i++) {
		cur = prev_0 + prev_1*2.0;
		prev_0 = prev_1;
		lp_filter[i] = (prev_1 = cur);
	}
        divisor = (prev_0 + prev_1);// * 2.0;***
	complete_filter();

	return 1;
}
long C_DRZF_SubbandCoder::add_value (long val, FilterQueue* into) {
	if (val != 0 ) {
		double* conv_ptr = (double*)into->cur;
		int conv_no = into->cur_no;
		double* coeffs = (into->is_odd) ? hp_filter: lp_filter;
		for (int i=0; i<f_len; i++) {
			*conv_ptr += coeffs[i]*val;
			conv_no++; conv_ptr++;
			if (conv_no == f_len) {
				conv_no = 0;
				conv_ptr = (double*)into->incomp;
			}
		}
	}
//	return ( (long)floor (0.5 + *(double*)into->cur) );
	return ( (long)(*(double*)into->cur) );
}
void C_DRZF_SubbandCoder::complete_filter() {
	int i;
	for (i=f_half-3; i>=0; i-=4) {
		lp_filter[i] = -lp_filter[i];
		if (i > 0) lp_filter[i-1] = -lp_filter[i-1];
	}
	for (i=0; i<f_half-1; i++) lp_filter[f_len-i-1] = lp_filter[i];
	for (i=0; i<f_len; i++) {
		lp_filter[i] /= divisor;
		hp_filter[i] = lp_filter[i];
		if (i % 2 == f_half % 2) hp_filter[i] = -hp_filter[i];
	}

}


int C_DOpt_SubbandCoder::allocate_coeffs() {
	lp_filter = new double [f_len]; if (!lp_filter) return 0;
	hp_filter = new double [f_len]; if (!hp_filter) return 0;

	int i;
	memset (lp_filter, 0, sizeof (double) * f_len);

	lp_filter[0] = 1;
	for (int s=1; s<f_half; s++) {
		double old_val;
		lp_filter[s-1] = (2*lp_filter[s-1]) + ((s>1)?lp_filter[s-2]:0);
		if ((s-1) % 2 == 0) lp_filter[s-1]++;
		for (i=s-2; i>=0; i--) {
			if (i % 2 == 0) lp_filter[i] = old_val;
			else if (i % 2 == 1) {
				old_val = lp_filter[i];
				lp_filter[i] = (2*lp_filter[i]) + lp_filter[i-1];
			}
		}
		lp_filter[s] =(2*lp_filter[s-1]) + ((s>1)?lp_filter[s-2]:0);
		lp_filter[s]++;
		old_val = lp_filter[s];
	}
	divisor = lp_filter[0]*lp_filter[1] / 2; // ***
	complete_filter();
	return 1;
}
