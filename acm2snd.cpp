#include "stdafx.h"
//acm2snd.cpp
// Acm to Wav conversion
#include <io.h>
#include <errno.h>
#include "readers.h"
#include "general.h"
#include "riffhdr.h"

#define OV_EXCLUDE_STATIC_CALLBACKS //who needs that
#include "vorbisfile.h"

static size_t myread(void *memory, size_t size, size_t count, void *src)
{
  long cur = tell( ( (MyFile *) src)->fhandle);
  if (cur>(( MyFile *) src)->end)
  {
    errno=EINVAL;
    return 0;
  }

  long tmp = size*count;
  if (tmp+cur<= ( ((MyFile *) src)->end) )
  {
    return read(((MyFile *) src)->fhandle, memory, tmp);
  }
  return read( ((MyFile *) src)->fhandle, memory, (unsigned int) ( (MyFile *) src)->end-cur);
}

static int myseek(void *src, ogg_int64_t offset, int whence)
{
  switch(whence)
  {
  case SEEK_SET:
    return lseek(((MyFile *) src)->fhandle, (long) offset + ((MyFile *) src)->base, whence)-((MyFile *) src)->base;
  case SEEK_CUR:
    return lseek(((MyFile *) src)->fhandle, (long) offset, whence)-((MyFile *) src)->base;
  case SEEK_END:
    return lseek(((MyFile *) src)->fhandle,  ((MyFile *) src)->end-(long) offset, SEEK_SET)-((MyFile *) src)->base;
  }
  return -1;
}

/*
static int myclose(void *src)
{
  return close(((MyFile *) src)->fhandle);
}
*/

static long mytell(void *src)
{
  return tell( ( (MyFile *) src)->fhandle) - ((MyFile *) src)->base;
}

static ov_callbacks OV_CALLBACKS_DLTCEP = {
  (size_t (*)(void *, size_t, size_t, void *))  myread,
  (int (*)(void *, ogg_int64_t, int))           myseek,
  (int (*)(void *))                             NULL,
  (long (*)(void *))                            mytell
};

int ogg_decode(int fhandle, long maxlen, unsigned char *&memory, long &samples_written)
{
  unsigned char *pcmout;
  unsigned char *pcmend;
  OggVorbis_File vf;
  int eof=0;
  int current_section;
  MyFile tmp;

  lseek(fhandle, -4, SEEK_CUR); //repositioning to OggS
  tmp.fhandle=fhandle;
  tmp.base=tell(fhandle);
  tmp.end=tmp.base+maxlen;
  
  if(ov_open_callbacks(&tmp, &vf, NULL, 0, OV_CALLBACKS_DLTCEP) < 0) {
      return -1;
  }

  {
    char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    while(*ptr){
      fprintf(stderr,"%s\n",*ptr);
      ++ptr;
    }
    long cnt = (long) ov_pcm_total(&vf,-1);
    samples_written = cnt*vi->channels*sizeof(short);
    //memory = (unsigned char *) new char[samples_written];
    memory=new unsigned char[samples_written+sizeof(RIFF_HEADER)];
    memset(memory,0,sizeof(memory));
    write_riff_header (memory, cnt, vi->channels, vi->rate);
    pcmout = memory+sizeof(RIFF_HEADER);
    pcmend = pcmout+samples_written;
    
    //fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    //fprintf(stderr,"\nDecoded length: %ld samples\n",
    //        (long)ov_pcm_total(&vf,-1));
    //fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
  }
  
  while(!eof){
    long ret=ov_read(&vf,(char *) pcmout,4096,0,2,1,&current_section);
    if (ret == 0) {
      /* EOF */
      if (pcmout!=pcmend) {
        //something was messed up
      }
      eof=1;
    } else if (ret < 0) {
      //something was messed up
    } else {    
      pcmout+=ret;
      if (pcmout>pcmend) {
        eof=1; //something was messed up!!!
      }
    }
  }
  
  ov_clear(&vf);
  return 0;
}

#if 0
int ogg_decode(int fhandle, long maxlen, unsigned char *&memory, long &samples_written)
{
  ogg_page og;
  ogg_packet op;
  ogg_stream_state os_de;
  ogg_sync_state oy;

  char *buffer;
  ogg_sync_init(&oy);
  lseek(fhandle, -4, SEEK_CUR); //repositioning to OggS

  while(1){ /* we repeat if the bitstream is chained */
    int eos = 0;
    int i;

    buffer=ogg_sync_buffer(&oy,4096);
    int len = maxlen;
    if (len>4096) len = 4096;
    maxlen -= len;

    len = read(fhandle, buffer, len);
    ogg_sync_wrote(&oy, len);
    if(ogg_sync_pageout(&oy,&og)!=1){
      /* have we simply run out of data?  If so, we're done. */
      if(len<4096) break;
      
      /* error case.  Must not be Vorbis data */
      return 3;
    }
//    ogg_stream_init(&os_de,0x04030201);
    ogg_stream_init(&os_de,0);
    if(ogg_stream_pagein(&os_de,&og)<0){ 
      /* error; stream version mismatch perhaps */
      return 3;
    }
    
    if(ogg_stream_packetout(&os_de,&op)!=1){ 
      /* no page? must not be vorbis */     
      return 3;
    }

    /* At this point, we're sure we're Vorbis. We've set up the logical
       (Ogg) bitstream decoder. Get the comment and codebook headers and
       set up the Vorbis decoder */
    
    /* The next two packets in order are the comment and codebook headers.
       They're likely large and may span multiple pages. Thus we read
       and submit data until we get our two packets, watching that no
       pages are missing. If a page is missing, error out; losing a
       header page is the only place where missing data is fatal. */
    
    i=0;
    while(i<2){
      while(i<2){
        int result=ogg_sync_pageout(&oy,&og);
        if(result==0)break; /* Need more data */
        /* Don't complain about missing or corrupt data yet. We'll
           catch it at the packet output phase */
        if(result==1){
          ogg_stream_pagein(&os_de,&og); /* we can ignore any errors here
                                         as they'll also become apparent
                                         at packetout */
          while(i<2){
            result=ogg_stream_packetout(&os_de,&op);
            if(result==0)break;
            if(result<0){
              /* Uh oh; data at some point was corrupted or missing!
                 We can't tolerate that in a header.  Die. */              
              return 3;
            }
            i++;
          }
        }
      }
      /* no harm in not checking before adding more */
      buffer=ogg_sync_buffer(&oy,4096);
      len = maxlen;
      if (len>4096) len = 4096;
      maxlen -= len;

      len=read(fhandle, buffer, len);
      if(len==0 && i<2) {
        return 3;
      }
      ogg_sync_wrote(&oy,len);
    }
    
    while(!eos) {
      while(!eos) {
        int result=ogg_sync_pageout(&oy,&og);
        if(result==0) break; /* need more data */
        if(result<0) { /* missing or corrupt data at this page position */
          return 3;
        }
        else
        {
          ogg_stream_pagein(&os_de,&og); /* can safely ignore errors at this point */
          while(1) {
            result=ogg_stream_packetout(&os_de,&op);
            
            if(result==0)break; /* need more data */
            if(result<0){ /* missing or corrupt data at this page position */
              /* no reason to complain; already complained above */
            }else{
              /* we have a packet.  Decode it */
              float **pcm;
              int samples;
              
            }
            if(ogg_page_eos(&og))eos=1;
          }
        }
        if(!eos) {
          buffer=ogg_sync_buffer(&oy,4096);
          len = maxlen;
          if (len>4096) len=4096;
          len=read(fhandle, buffer,len);
          ogg_sync_wrote(&oy,len);
          if(len==0) eos=1;
        }
      }
    }      
    /* clean up this logical bitstream; before exit we see if we're
       followed by another [chained] */
    
    ogg_stream_clear(&os_de);
  }
  //
  //
  //
  ogg_sync_clear(&oy);
  return 0;
}
#endif

static CACMReader* acm = NULL;

void finalize() {
  if (acm) delete acm;
  acm=NULL;
}

int ConvertAcmWav(int fhandle, long maxlen, unsigned char *&memory, long &samples_written, int forcestereo)
{
  int riff_chans;
  long rawsize=0;
  long cnt, cnt1;
  RIFF_HEADER riff;
  
  memory=0;
  if(maxlen==-1) maxlen=filelength(fhandle);
  try
  {
    //handling normal riff, it is not a standard way, but hopefully able to handle most of the files
    if(read(fhandle,&riff, 4) !=4) return 3;

    if (!memcmp(riff.riff_sig, "OggS", 4))
    {
      return ogg_decode(fhandle, maxlen, memory, samples_written);
    }

    if(!memcmp(riff.riff_sig,"RIFF",4))
    {    
      if(read(fhandle,&riff.total_len_m8, sizeof(RIFF_HEADER)-4 )==sizeof(RIFF_HEADER)-4 )
      {
        // data_sig-wformattag=16
        if(riff.formatex_len!=(unsigned long) ((BYTE *) riff.data_sig-(BYTE *) &riff.wFormatTag))
        {
          cnt=riff.formatex_len-24;
          lseek(fhandle,cnt,SEEK_CUR);
          read(fhandle,riff.data_sig,8);//8 = sizeof sig+sizeof length
          riff.formatex_len=16;
        }
        if(!memcmp(riff.data_sig,"fact",4) ) //skip 'fact' (an optional RIFF tag)
        {
          cnt=riff.raw_data_len; 
          lseek(fhandle,cnt,SEEK_CUR);
          read(fhandle,riff.data_sig,8); //8 is still the same 
          if(memcmp(riff.data_sig,"data",4) )
          {
            finalize();
            return 3;
          }
        }
        memory=new unsigned char[riff.raw_data_len+sizeof(RIFF_HEADER)];
        if(!memory)
        {
          finalize();
          return 3;
        }
        maxlen-=sizeof(RIFF_HEADER);
        if(riff.raw_data_len>(unsigned long) maxlen)
        {
          riff.total_len_m8=maxlen+sizeof(RIFF_HEADER);
          riff.raw_data_len=maxlen;
        }
        memcpy(memory,&riff,sizeof(RIFF_HEADER) );
        samples_written = riff.raw_data_len+sizeof(RIFF_HEADER);
        if(read(fhandle,(unsigned char *) memory+sizeof(RIFF_HEADER),riff.raw_data_len)!=(long) riff.raw_data_len)
        {
          finalize();
          return 3;
        }
        finalize();
        return 0;
      }
    }
    lseek(fhandle,-4,SEEK_CUR);
    acm = (CACMReader*)CreateSoundReader(fhandle, SND_READER_ACM, maxlen);
    if (!acm)
    {
      //Error 1: Cannot create sound reader
      finalize();
      return 1;
    }
    
    cnt = acm->get_length();
    riff_chans = acm->get_channels();
    if(forcestereo && (riff_chans==1)) riff_chans=2;
    if(riff_chans!=1 && riff_chans!=2)
    {
      finalize();
      return 4;
    }
    
    memory=new unsigned char[cnt*2+sizeof(RIFF_HEADER)];
    if(!memory)
    {
      finalize();
      return 3;
    }
    samples_written = sizeof(RIFF_HEADER);  
    memset(memory,0,sizeof(memory));
    write_riff_header (memory, cnt, riff_chans, acm->get_samplerate());
    
    cnt1 = acm->read_samples ((short*)(memory+samples_written), cnt);
    rawsize=cnt1*sizeof(short);
    samples_written+=rawsize;
    cnt -= cnt1;
    
    finalize();
    if(cnt) return 4;
    return 0;
  }
  catch(...)
  {
    finalize();
		return 4;
  }
}
