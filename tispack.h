#define DESTBUFSIZE 8192

#define TP_BLUR 0
#define TP_QUALITY 100

int perform_tis2tiz(CString infname, CString outpath);
int perform_tiz2tis(CString infname, CString outpath);
int perform_pvrz2pvr(CString infname, CString outpath);
int perform_pvr2pvrz(CString infname, CString outpath);
int pvrunpack(int infile, int outfile, unsigned long insize, unsigned long outsize);