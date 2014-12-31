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
  int fhandle;
  int startpoint;
  int maxlen;
public:
  //.wed
  bool m_night;
  bool wedchanged, wedavailable;
  wed_header wedheader;
  wed_secondary_header secheader;
  int polyoffset;

  int overlaycount, overlaytilecount, overlaytileidxcount;
  int weddoorcount, doortileidxcount, doorpolygoncount;
  int wallgroupidxcount;
  int wallpolygoncount;  //this is the count of the wallpolygons
  int polygonidxcount; //this is the size of the wallgroup index table
  int wedvertexcount;

  wed_overlay *overlayheaders;
  wed_tilemap *overlaytileheaders;
  wed_door *weddoorheaders;
  short *doortileindices;
  short *overlaytileindices;
  wed_polyidx *wallgroupindices;
  wed_polygon *doorpolygonheaders;
  wed_polygon *wallpolygonheaders;
  short *polygonindices; //wallgroup polygon indices
  area_vertex *wedvertices; //a wed vertex is the same as an area vertex (POINTS)

  unsigned short m_width, m_height;
  unsigned char *searchmap;
  unsigned char *lightmap;
  unsigned char *heightmap;
  bool changedmap[3];
  bool m_changed;
  bool m_smallpalette;
  COLORREF lmpal[256];
  COLORREF srpal[16];
  COLORREF htpal[16];
  COLORREF ht8pal[256];

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
  int tileflagcount;
  int doorcount;
  int mapnotecount;
  int trapcount;
  int effectcount;
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
  CMyPtrList vertexheaderlist;
  CMyPtrList wedvertexheaderlist;
  CMyPtrList doortilelist;
  area_variable *variableheaders;
  area_door *doorheaders;
  area_mapnote *mapnoteheaders;
  pst_area_mapnote *pstmapnoteheaders;
  area_trap *trapheaders;
  creature_effect *effects;
  area_tile *tileheaders;
  area_tileflag *tileflagheaders;
  char *explored;
  char **credatapointers;

	Carea();
	virtual ~Carea();
  void new_area();
  int RemoveDoorPolygon(int first);
  int GetWedDoorIndex(char *doorid);
  int AddWedDoor(CString doorid);
  int RemoveWedDoor(char *doorid);
  int DefaultAreaOverlays();
  int AddAreaOverlay();
  int WriteAreaToFile(int fh, int calculate);
  int WriteWedToFile(int fh);
  int WriteMap(const char *suffix, unsigned char *pixels, COLORREF *pal, int palsize);
  int LoadPLY(int fhandle);
  int LoadSVG(int fhandle);
  int SavePLY(int fhandle);
  int ReadActorData();
  void ConvertFromPstMapnote();
  void ConvertToPstMapnote();
  int ReadAreaFromFile(int fh, long ml);
  int getotc(int overlay);
  int getfoti(int overlay);
  int getotic(int overlay);
  int ReadMap(const char *Suffix, unsigned char *&Storage, COLORREF *Palette, int size);
  int ReadWedFromFile(int fh, long ml);
  int CheckDestination(int fh, long ml, CString entryname);
  int FillDestination(int fh, long ml, CComboBox *cb);
  CString RetrieveMosRef(int fh);
  bool WedChanged()
  {
    int i;

    for(i=0;i<3;i++) if(changedmap[i]) return true;
    return wedchanged;
  }
  bool WedAvailable() { return wedavailable; }
  void ConvertNightMap(int r, int g, int b, int strength);
  void MirrorMap(unsigned char *poi);
  void RecalcBox(int pos, wed_polygon *header, area_vertex *vertices);
  //int VertexOrder(area_vertex *wedvertex, int count);
  int ConvertOffsetToIndex(int polyoffset, int &dpc);
//  int ConsolidateVertices();
//  int ConsolidateDoortiles();

  inline void KillMaps()
  {
    if(searchmap)
    {
      delete [] searchmap;
      searchmap=NULL;
    }
    if(lightmap)
    {
      delete [] lightmap;
      lightmap=NULL;
    }
    if(heightmap)
    {
      delete [] heightmap;
      heightmap=NULL;
    }
  }
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
    if(wallpolygonheaders)
    {
      delete[] wallpolygonheaders;
      wallpolygonheaders=NULL;
    }
    wallpolygoncount=0;
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
  /* these will be in lists now
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
  */
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
    if(pstmapnoteheaders)
    {
      delete[] pstmapnoteheaders;
      pstmapnoteheaders=NULL;
    }
    mapnotecount=0;
  }
  inline void KillTraps()
  {
    if(trapheaders)
    {
      delete[] trapheaders;
      trapheaders=NULL;
    }
    trapcount=0;
  }
  inline void KillEffects()
  {
    if(effects)
    {
      delete[] effects;
      effects=NULL;
    }
    effectcount=0;
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
  inline void KillTileFlags()
  {
    if(tileflagheaders)
    {
      delete[] tileflagheaders;
      tileflagheaders=NULL;
    }
    tileflagcount=0;
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

  inline void KillDoorTileList()
  {
    int i;
    void *poi;
    
    i=doortilelist.GetCount();
    while(i--)
    {
      poi=doortilelist.RemoveHead();
      if(poi) delete [] poi;      
    }
  }

  inline void KillWedVertexList()
  {
    int i;
    void *poi;
    
    i=wedvertexheaderlist.GetCount();
    while(i--)
    {
      poi=wedvertexheaderlist.RemoveHead();
      if(poi) delete [] poi;      
    }
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
  void FlipWedVertex(long id, int count, int width);
  void FlipDoorTile(long id, int count, int width);
  int RecalcBoundingBoxes();
private:
  int adjust_actpointa(long offset);
  int adjust_actpointw(long offset);
  void MergeVertex(long &offset, int count);
  void MergeWedVertex(long &offset, int count);
  void MergeDoortile(short &offset, int count);
  int ImplodeVertices();
  int ImplodeWedVertices();
  int ImplodeDoortiles();
  void HandleVertex(long offset, int count);
  void HandleWedVertex(long offset, int count);
  void HandleDoortile(long offset, int count);
  int ExplodeVertices();
  int ExplodeWedVertices();
  int ExplodeDoorTiles();
  void ConsolidateItems();

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
