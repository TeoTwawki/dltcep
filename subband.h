#ifndef _ACM_LAB_SUBBAND_CODER_H
#define _ACM_LAB_SUBBAND_CODER_H

//These structures could be much simpler, but some items were introduced
//  for little speed-optimization.

// A subband queue
struct FilterQueue {
	// these are unsigned char, because each implementation can use its own type here:
	unsigned char* incomp; // pointer to incomplete values
	unsigned char* cur; // pointer to top of a queue
	int cur_no; // current top number
	long input_count; // count of processed values
	int is_odd; // is input_count an odd number?
};
// One level
struct FilterLevel {
	FilterQueue* first; // pointer to first subband of the level
	int count; // count of subbands on this level
	FilterQueue* cur; // pointer to current subband
	int cur_no; // current subband number
};

// Just an abstract FIR-based pyramidal subband coder
class CSubbandCoder {
protected:
	int f_len, f_half; // length and half-length of the filter
	int levels; // count of levels
	int subbands; // count of subbands
	unsigned char* incomp; // large array of incomplete values, the FQ.first_incomp and FQ.cur are mapped to this array
	FilterQueue* ques; // all the ques are stored here, the FL.first and FL.cur are mapped to this array
	FilterLevel* levs; // the levels of the filter

	// These need to be overwritten in the subclasses:
	virtual long get_incomp_size() = 0; // returns the size of buffer's elements;
		// long long (int64) or double can be used as elems.
	virtual int allocate_coeffs() = 0; // allocates the coeffs of the high- and low-pass filters.
	virtual long add_value (long val, FilterQueue* into) = 0; // adds a new value into a queue
public:
	CSubbandCoder (int half, int lev_cnt) // size of a filter and count of levels
		: incomp (NULL),
		ques (NULL), levs (NULL),
		f_half (half), levels (lev_cnt) {};
	virtual ~CSubbandCoder() {
		if (incomp) delete incomp; incomp = NULL;
		if (ques) delete ques; ques = NULL;
		if (levs) delete levs; levs = NULL;
	};

	int init_filter(); // initialize the structures of the filter
	void clear_filter(); // clears all the data in the filter
	long get_init_size(); // returns the size of initializing block
	long filter_data (short* data, long count, long* res_data);
		// filters the data, returns the count of resulting items
};

// FP-implementation of "Return 0; Filter".
class C_DRZF_SubbandCoder : public CSubbandCoder {
protected:
	double *lp_filter, *hp_filter; // low- and high-pass coefficients
	double divisor;
	virtual int allocate_coeffs();
	virtual long add_value (long val, FilterQueue* into);
	virtual long get_incomp_size() { return sizeof(double); };

	void complete_filter(); // builds the lp and hp filters based on half of taps of lp filter
public:
	C_DRZF_SubbandCoder (int half, int lev_cnt)
		: CSubbandCoder (half, lev_cnt),
		lp_filter (NULL), hp_filter (NULL),
		divisor (1.0) {};
	virtual ~C_DRZF_SubbandCoder() {
		if (lp_filter) delete lp_filter; lp_filter = NULL;
		if (hp_filter) delete hp_filter; hp_filter = NULL;
	};
};

// FP-implementation of delta-optimizing filter.
class C_DOpt_SubbandCoder : public C_DRZF_SubbandCoder {
protected:
	virtual int allocate_coeffs();
public:
	C_DOpt_SubbandCoder (int half, int lev_cnt)
		: C_DRZF_SubbandCoder (half, lev_cnt) {};
};

// Custom FP filter. Takes the coeffs from a text file.

#endif
