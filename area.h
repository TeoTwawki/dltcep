// area.h: interface for the Carea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREA_H__C975A46B_7D3C_433C_8F35_9582A94DEF85__INCLUDED_)
#define AFX_AREA_H__C975A46B_7D3C_433C_8F35_9582A94DEF85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"
#include <io.h>

class Carea  
{
private:
  int fhandlea, fhandlew;
  long maxlena, maxlenw;
  long fullsizea, fullsizew;
  long startpointa, startpointw;
public:
  //.wed
  bool wedchanged, wedavailable;
  wed_header wedheader;
  wed_secondary_header secheader;
  int polyoffset;

  int overlaycount, overlaytilecount, overlaytileidxcount;
  int weddoorcount, doortileidxcount, doorpolygoncount;
  int wallgroupidxcount, wallgroupcount, polygonidxcount;
  int wedvertexcount, wallgroupvertexcount, doorvertexcount;

  wed_overlay *overlayheaders;
  wed_tilemap *overlaytileheaders;
  wed_door *weddoorheaders;
  short *doortileindices;
  short *overlaytileindices;
  wed_polyidx *wallgroupindices;
  wed_polygon *doorpolygonheaders;
  wed_polygon *wallgroupheaders;
  short *polygonindices; //wallgroup polygon indices
  area_vertex *wedvertices; //a wed vertex is the same as an area vertex (POINTS)
  //CleanupVertices will separate wedvertices into these parts
  area_vertex *wallgroupvertices;
  area_vertex *doorvertices; 

  unsigned short width, height;
  unsigned char *searchmap;
  unsigned char *lightmap;
  unsigned char *heightmap;

  //.area
  int revision;
  area_header header;
  area_song songheader;
  area_int intheader;
  iwd2_header iwd2header;
  int actorcount;
  int triggercount;
  int spawncount;
  int entrancecount;
  int containercount;
  int itemcount;
  int ambientcount;
  int animcount;
  int vertexcount;
  int variablecount;
  int doorcount;
  int mapnotecount;
  int tilecount;
  int exploredsize;

  area_actor *actorheaders;
  area_trigger *triggerheaders;
  area_spawn *spawnheaders;
  area_entrance *entranceheaders;
  area_container *containerheaders;
  area_item *itemheaders;
  area_ambient *ambientheaders;
  area_anim *animheaders;
  area_vertex *vertexheaders;
  CVertexPtrList vertexheaderlist;
  area_variable *variableheaders;
  area_door *doorheaders;
  area_mapnote *mapnoteheaders;
  area_tile *tileheaders;
  char *explored;
  char **credatapointers;

	Carea();
	virtual ~Carea();
  void new_area();
  int WriteAreaToFile(int fh, int calculate);
  int WriteWedToFile(int fh, int night);
  int ReadActorData();
  int ReadAreaFromFile(int fh, long ml);
  int getfoti(int overlay);
  int getotic(int overlay);
  int ReadMap(const char *Suffix, unsigned char *Storage);
  int ReadWedFromFile(int fh, long ml);
  int CheckDestination(int fh, long ml, CString entryname);
  int FillDestination(int fh, long ml, CComboBox *cb);
  CString RetrieveMosRef(int fh);
  bool WedChanged() { return wedchanged; }
  bool WedAvailable() { return wedavailable; }
  void RecalcBox(int pos, wed_polygon *header, area_vertex *vertices, int door);
  int VertexOrder(area_vertex *wedvertex, int count);
  int ConvertOffsetToIndex(int polyoffset);
  int ConsolidateVertices();
  int ConsolidateDoortiles();

  inline void KillOverlays()
  {
    if(overlayheaders)
    {
      delete[] overlayheaders;
      overlayheaders=NULL;
    }
    overlaycount=0;
  }
  inline void KillOverlaytiles()
  {
    if(overlaytileheaders)
    {
      delete[] overlaytileheaders;
      overlaytileheaders=NULL;
    }
    overlaytilecount=0;
  }
  inline void KillOverlayindices()
  {
    if(overlaytileindices)
    {
      delete[] overlaytileindices;
      overlaytileindices=NULL;
    }
    overlaytileidxcount=0;
  }
  inline void KillWeddoors()
  {
    if(weddoorheaders)
    {
      delete[] weddoorheaders;
      weddoorheaders=NULL;
    }
    weddoorcount=0;
  }
  inline void KillDoortiles()
  {
    if(doortileindices)
    {
      delete[] doortileindices;
      doortileindices=NULL;
    }
    doortileidxcount=0;
  }
  inline void KillDoorpolygons()
  {
    if(doorpolygonheaders)
    {
      delete[] doorpolygonheaders;
      doorpolygonheaders=NULL;
    }
    doorpolygoncount=0;
  }
  inline void KillWallgroups()
  {
    if(wallgroupindices)
    {
      delete[] wallgroupindices;
      wallgroupindices=NULL;
    }
    wallgroupidxcount=0;
  }
  inline void KillWallgrouppolygons()
  {
    if(wallgroupheaders)
    {
      delete[] wallgroupheaders;
      wallgroupheaders=NULL;
    }
    wallgroupcount=0;
  }
  inline void KillPolygonindices()
  {
    if(polygonindices)
    {
      delete[] polygonindices;
      polygonindices=NULL;
    }
    polygonidxcount=0;
  }
  inline void KillWedvertices()
  {
    if(wedvertices)
    {
      delete[] wedvertices;
      wedvertices=NULL;
    }
    wedvertexcount=0;
  }
  inline void KillWallgroupvertices()
  {
    if(wallgroupvertices)
    {
      delete[] wallgroupvertices;
      wallgroupvertices=NULL;
    }
    wallgroupvertexcount=0;
  }
  inline void KillDoorvertices()
  {
    if(doorvertices)
    {
      delete[] doorvertices;
      doorvertices=NULL;
    }
    doorvertexcount=0;
  }

  inline void KillActors()
  {
    KillCreDataPointers();
    if(actorheaders)
    {
      delete[] actorheaders;
      actorheaders=NULL;
    }
    actorcount=0;
  }
  inline void KillCreDataPointers()
  {
    if(credatapointers)
    {
      int i;

      for(i=0;i<actorcount;i++)
      {
        if(credatapointers[i])
        {
          delete [] credatapointers[i];
          credatapointers[i]=NULL;
        }
      }
      delete [] credatapointers;
      credatapointers=NULL;
    }
  }

  inline void KillTriggers()
  {
    if(triggerheaders)
    {
      delete[] triggerheaders;
      triggerheaders=NULL;
    }
    triggercount=0;
  }
  inline void KillSpawns()
  {
    if(spawnheaders)
    {
      delete[] spawnheaders;
      spawnheaders=NULL;
    }
    spawncount=0;
  }
  inline void KillEntrances()
  {
    if(entranceheaders)
    {
      delete[] entranceheaders;
      entranceheaders=NULL;
    }
    entrancecount=0;
  }
  inline void KillContainers()
  {
    if(containerheaders)
    {
      delete[] containerheaders;
      containerheaders=NULL;
    }
    containercount=0;
  }
  inline void KillItems()
  {
    if(itemheaders)
    {
      delete[] itemheaders;
      itemheaders=NULL;
    }
    itemcount=0;
  }
  inline void KillAmbients()
  {
    if(ambientheaders)
    {
      delete[] ambientheaders;
      ambientheaders=NULL;
    }
    ambientcount=0;
  }
  inline void KillAnimations()
  {
    if(animheaders)
    {
      delete[] animheaders;
      animheaders=NULL;
    }
    animcount=0;
  }
  inline void KillVariables()
  {
    if(variableheaders)
    {
      delete[] variableheaders;
      variableheaders=NULL;
    }
    variablecount=0;
  }
  inline void KillDoors()
  {
    if(doorheaders)
    {
      delete[] doorheaders;
      doorheaders=NULL;
    }
    doorcount=0;
  }
  inline void KillMapnotes()
  {
    if(mapnoteheaders)
    {
      delete[] mapnoteheaders;
      mapnoteheaders=NULL;
    }
    mapnotecount=0;
  }
  inline void KillTiles()
  {
    if(tileheaders)
    {
      delete[] tileheaders;
      tileheaders=NULL;
    }
    tilecount=0;
  }
  inline void KillVertices()
  {
    if(vertexheaders)
    {
      delete[] vertexheaders;
      vertexheaders=NULL;
    }
    vertexcount=0;
  }
  inline void KillVertexList()
  {
    int i;
    void *poi;

    i=vertexheaderlist.GetCount();
    while(i--)
    {
      poi=vertexheaderlist.RemoveHead();
      if(poi) delete [] poi;      
    }
  }
  inline void KillExplored()
  {
    if(explored)
    {
      delete [] explored;
      explored=NULL;
    }  
    exploredsize=0;
  }
  void ShiftVertex(long id, int count, POINTS &point);
  void FlipVertex(long id, int count, int width);
private:
  int adjust_actpointa(long offset);
  int adjust_actpointw(long offset);
  void MergeVertex(long &offset, int count);
  int RecalcBoundingBoxes();
  int CleanUpVertices();
  int ImplodeVertices();
  void HandleVertex(long offset, int count);
  int ExplodeVertices();
  inline long myseeka(long pos)
  {
    return lseek(fhandlea, pos+startpointa,SEEK_SET)-startpointa;
  }
  inline long myseekw(long pos)
  {
    return lseek(fhandlew, pos+startpointw,SEEK_SET)-startpointw;
  }
  inline long actpointa()
  {
    return tell(fhandlea)-startpointa;
  }
  inline long actpointw()
  {
    return tell(fhandlew)-startpointw;
  }
};

#endif // !defined(AFX_AREA_H__C975A46B_7D3C_433C_8F35_9582A94DEF85__INCLUDED_)
