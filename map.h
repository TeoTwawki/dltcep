// map.h: interface for the Cmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAP_H__765880D8_0353_4668_B610_58432EB8A1A2__INCLUDED_)
#define AFX_MAP_H__765880D8_0353_4668_B610_58432EB8A1A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cmap  
{
private:
  int fhandle;
  long maxlen;
  long fullsize;
  long startpoint;

public:
  map_mainheader mainheader;
  map_header *headers;
  map_area **areas;
  map_arealink **arealinks;

  int mapcount;
  int *areacount;
  int *linkcount;

	Cmap();
	virtual ~Cmap();
  int adjust_actpoint(long offset);
  int WriteMapToFile(int fh, int calculate);
  int ReadMapFromFile(int fh, long ml);

  inline void KillAreas()
  {
    int i;

    if(areas)
    {
      for(i=0;i<mapcount;i++)
      {
        if(areas[i])
        {
          delete [] areas[i];
          areas[i]=NULL;
        }
        if(areacount) areacount[i]=0;
      }
      delete [] areas;
      areas=NULL;
    }
    if(areacount)
    {
      delete [] areacount;
      areacount=NULL;
    }
  }
  inline void KillAreas(int map)
  {
    if(areas)
    {
      if(areas[map])
      {
        delete [] areas[map];
        areas[map]=NULL;
      }
      if(areacount) areacount[map]=0;
    }
  }

  inline void KillAreaLinks()
  {
    int i;

    if(arealinks)
    {
      for(i=0;i<mapcount;i++)
      {
        if(arealinks[i])
        {
          delete [] arealinks[i];
          arealinks[i]=NULL;
        }
        if(linkcount) linkcount[i]=0;
      }
      delete [] arealinks;
      arealinks=NULL;
    }
    if(linkcount)
    {
      delete [] linkcount;
      linkcount=NULL;
    }
  }

  inline void KillAreaLinks(int map)
  {
    if(arealinks)
    {
      if(arealinks[map])
      {
        delete [] arealinks[map];
        arealinks[map]=NULL;
      }
      if(linkcount) linkcount[map]=0;
    }
  }

  inline void KillMaps()
  {
    KillAreas();
    KillAreaLinks();
    if(headers)
    {
      delete [] headers;
      headers=NULL;
      mapcount=0;
    }
  }
  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
};

#endif // !defined(AFX_MAP_H__765880D8_0353_4668_B610_58432EB8A1A2__INCLUDED_)
