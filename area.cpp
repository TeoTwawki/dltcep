// area.cpp: implementation of the Carea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>

#include "chitem.h"
#include "area.h"
#include "ChitemDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Carea::Carea()
{
  fhandlea=fhandlew=-1;
  startpointa=startpointw=-1;
  fullsizea=fullsizew=-1;
  maxlena=maxlenw=-1;
  actorheaders=NULL;
  triggerheaders=NULL;
  spawnheaders=NULL;
  entranceheaders=NULL;
  containerheaders=NULL;
  itemheaders=NULL;
  ambientheaders=NULL;
  variableheaders=NULL;
  doorheaders=NULL;
  animheaders=NULL;
  mapnoteheaders=NULL;
  pstmapnoteheaders=NULL;
  trapheaders=NULL;
  effects=NULL;
  tileheaders=NULL;
  tileflagheaders=NULL;
  vertexheaders=NULL;
  explored=NULL;
  credatapointers=NULL;

  overlayheaders=NULL;
  overlaytileheaders=NULL;
  overlaytileindices=NULL;
  weddoorheaders=NULL;
  doortileindices=NULL;
  doorpolygonheaders=NULL;
  wallpolygonheaders=NULL;
  wallgroupindices=NULL;
  polygonindices=NULL;
  wedvertices=NULL;

  searchmap=NULL;
  lightmap=NULL;
  heightmap=NULL;

  actorcount=0;
  triggercount=0;
  spawncount=0;
  entrancecount=0;
  containercount=0;
  itemcount=0;
  ambientcount=0;
  variablecount=0;
  doorcount=0;
  animcount=0;
  mapnotecount=0;
  trapcount=0;
  tilecount=0;
  tileflagcount=0;
  vertexcount=0;
  exploredsize=0;

  overlaycount=0;
  overlaytilecount=0;
  overlaytileidxcount=0;
  weddoorcount=0;
  doortileidxcount=0;
  wallpolygoncount=0;
  doorpolygoncount=0;
  polygonidxcount=0;
  wedvertexcount=0;

  revision=10;
  m_changed=false;
  wedchanged=true;
  wedavailable=true;
  changedmap[0]=changedmap[1]=changedmap[2]=true;
  memset(&intheader,0,sizeof(intheader) );
  m_smallpalette=true;
}

Carea::~Carea()
{
  new_area();
}

void Carea::new_area()
{
  KillActors();
  KillTriggers();
  KillSpawns();
  KillEntrances();
  KillContainers();
  KillItems();
  KillAmbients();
  KillVariables();
  KillDoors();
  KillAnimations();
  KillMapnotes();
  KillTraps();
  KillEffects();
  KillTiles();
  KillTileFlags();
  KillVertices();
  KillVertexList();
  KillWedVertexList();
  KillDoorTileList();
  KillExplored();

  KillOverlays();
  KillOverlaytiles();
  KillWeddoors();
  KillDoortiles();
  KillOverlayindices();
  KillWallgroups();
  KillDoorpolygons();
  KillWallgrouppolygons();
  KillPolygonindices();
  KillWedvertices();
  KillMaps();
  
  wedchanged=true;
  wedavailable=true;
  changedmap[0]=changedmap[1]=changedmap[2]=true;
  //this could be dangerous
  the_mos.FreeOverlay(); //clearing up the associated tileset
  m_width=256;
  m_height=256;
}

int Carea::adjust_actpointa(long offset)
{
  if(actpointa()==offset) return 0;
  if(maxlena<offset) return -2;
  if(myseeka(offset)!=offset) return -2;
  return 1;
}

int Carea::adjust_actpointw(long offset)
{
  if(actpointw()==offset) return 0;
  if(maxlenw<offset) return -2;
  if(myseekw(offset)!=offset) return -2;
  return 1;
}

void Carea::ShiftVertex(long id, int count, POINTS &point)
{
  area_vertex *poi;
  int i;

  poi=(area_vertex *) vertexheaderlist.GetAt(vertexheaderlist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    poi[i].x=(unsigned short) (poi[i].x+point.x);
    poi[i].y=(unsigned short) (poi[i].y+point.y);
  }
}

void Carea::FlipDoorTile(long id, int count, int width)
{
  short *poi;
  int i;
  int x,y;

  poi=(short *) doortilelist.GetAt(doortilelist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    x=poi[i]%width;
    y=poi[i]/width;
    poi[i]=(unsigned short) ((y+1)*width-x-1);
  }
}

void Carea::FlipWedVertex(long id, int count, int width)
{
  area_vertex *poi;
  int i;

  poi=(area_vertex *) wedvertexheaderlist.GetAt(wedvertexheaderlist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    poi[i].x=(unsigned short) (width-poi[i].x);
  }
}

void Carea::FlipVertex(long id, int count, int width)
{
  area_vertex *poi;
  int i;

  poi=(area_vertex *) vertexheaderlist.GetAt(vertexheaderlist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    poi[i].x=(unsigned short) (width-poi[i].x);
  }
}

void Carea::MergeDoortile(short &offset, int count)
{
  void *poi;

  offset=(short) doortileidxcount;
  poi=doortilelist.RemoveHead();
  memcpy(doortileindices+offset,poi,sizeof(short)*count);
  delete [] poi;
  doortileidxcount+=count;
}

void Carea::MergeWedVertex(long &offset, int count)
{
  void *poi;

  offset=wedvertexcount;
  poi=wedvertexheaderlist.RemoveHead();
  memcpy(wedvertices+offset,poi,sizeof(area_vertex)*count);
  delete [] poi;
  wedvertexcount+=count;
}

void Carea::MergeVertex(long &offset, int count)
{
  void *poi;

  offset=header.vertexcnt;
  poi=vertexheaderlist.RemoveHead();
  memcpy(vertexheaders+offset,poi,sizeof(area_vertex)*count);
  delete [] poi;
  header.vertexcnt=(short) (header.vertexcnt+count);
}

int Carea::ImplodeVertices()
{
  int i;

  KillVertices(); //for safety reasons, we just kill them again
  //first count all the vertices (killvertices zeroes vertexcount)
  for(i=0;i<header.infocnt;i++)
  {
    vertexcount+=triggerheaders[i].vertexcount;
  }
  for(i=0;i<header.containercnt;i++)
  {
    vertexcount+=containerheaders[i].vertexcount;
  }
  for(i=0;i<header.doorcnt;i++)
  {
    vertexcount+=doorheaders[i].countvertexopen;
    vertexcount+=doorheaders[i].countvertexclose;
    vertexcount+=doorheaders[i].countblockopen;
    vertexcount+=doorheaders[i].countblockclose;
  }

  vertexheaders=new area_vertex[vertexcount];
  if(!vertexheaders) return -3;
  //this will sum up header.vertexcnt
  header.vertexcnt=0;
  for(i=0;i<header.infocnt;i++)
  {
    MergeVertex(triggerheaders[i].vertindx,triggerheaders[i].vertexcount);
  }
  for(i=0;i<header.containercnt;i++)
  {
    MergeVertex(containerheaders[i].firstvertex,containerheaders[i].vertexcount);
  }
  for(i=0;i<header.doorcnt;i++)
  {
    MergeVertex(doorheaders[i].firstvertexopen,doorheaders[i].countvertexopen);
    MergeVertex(doorheaders[i].firstvertexclose,doorheaders[i].countvertexclose);
    MergeVertex(doorheaders[i].firstblockopen,doorheaders[i].countblockopen);
    MergeVertex(doorheaders[i].firstblockclose,doorheaders[i].countblockclose);
  }

  if(header.vertexcnt!=vertexcount) return -1; //internal error
  if(vertexheaderlist.GetCount())
  {
    KillVertexList();
    return 1;
  }
  return 0;
}

int Carea::ImplodeWedVertices()
{
  int i;

  KillWedvertices(); //kill them for safety reason

  for(i=0;i<secheader.wallpolycnt;i++)
  {
    wedvertexcount+=wallpolygonheaders[i].countvertex;
  }
  
  for(i=0;i<doorpolygoncount;i++)
  {
    wedvertexcount+=doorpolygonheaders[i].countvertex;
  }
  
  wedvertices=new area_vertex[wedvertexcount];
  if(!wedvertices) return -3;
  wedvertexcount=0;
  for(i=0;i<secheader.wallpolycnt;i++)
  {
    MergeWedVertex(wallpolygonheaders[i].firstvertex,wallpolygonheaders[i].countvertex);
  }
  for(i=0;i<doorpolygoncount;i++)
  {
    MergeWedVertex(doorpolygonheaders[i].firstvertex,doorpolygonheaders[i].countvertex);
  }

  if(wedvertexheaderlist.GetCount())
  {
    KillWedVertexList();
    return 1;
  }
  return 0;
}

int Carea::ImplodeDoortiles()
{
  int i;

  KillDoortiles();

  for(i=0;i<wedheader.doorcnt;i++)
  {
    doortileidxcount+=weddoorheaders[i].countdoortileidx;
  }
  doortileindices=new short[doortileidxcount];
  if(!doortileindices) return -3;
  doortileidxcount=0;
  for(i=0;i<wedheader.doorcnt;i++)
  {
    MergeDoortile(weddoorheaders[i].firstdoortileidx,weddoorheaders[i].countdoortileidx);
  }
  if(doortilelist.GetCount())
  {
    KillDoorTileList();
    return 1;
  }
  return 0;
}

void Carea::HandleVertex(long offset, int count)
{
  area_vertex *newvertices;

  newvertices=new area_vertex[count];
  if(!newvertices) return; //can't allocate, heh
  memcpy(newvertices,vertexheaders+offset,sizeof(area_vertex)*count);
  vertexcount-=count;
  //we store the pointer in the offset field, it needs to be resolved before save
  vertexheaderlist.AddTail(newvertices);
}

void Carea::HandleWedVertex(long offset, int count)
{
  area_vertex *newvertices;

  newvertices=new area_vertex[count];
  if(!newvertices) return; //can't allocate, heh
  memcpy(newvertices,wedvertices+offset,sizeof(area_vertex)*count);
  wedvertexcount-=count;
  //we store the pointer in the offset field, it needs to be resolved before save
  wedvertexheaderlist.AddTail(newvertices);
}

void Carea::HandleDoortile(long offset, int count)
{
  short *newtiles;

  newtiles=new short[count];
  if(!newtiles) return; //can't allocate, heh
  memcpy(newtiles,doortileindices+offset,sizeof(short)*count);
  doortileidxcount-=count;
  //we store the pointer in the offset field, it needs to be resolved before save
  doortilelist.AddTail(newtiles);
}

int Carea::ExplodeVertices()
{
  int ret;
  int i;

  KillVertexList();
  //move vertices into a list (so we can handle them separately)
  for(i=0;i<header.infocnt;i++)
  {
    HandleVertex(triggerheaders[i].vertindx,triggerheaders[i].vertexcount);
  }
  for(i=0;i<header.containercnt;i++)
  {
    HandleVertex(containerheaders[i].firstvertex,containerheaders[i].vertexcount);
  }
  for(i=0;i<header.doorcnt;i++)
  {
    HandleVertex(doorheaders[i].firstvertexopen,doorheaders[i].countvertexopen);
    HandleVertex(doorheaders[i].firstvertexclose,doorheaders[i].countvertexclose);
    HandleVertex(doorheaders[i].firstblockopen,doorheaders[i].countblockopen);
    HandleVertex(doorheaders[i].firstblockclose,doorheaders[i].countblockclose);
  }

  ret=vertexcount;
  KillVertices();
  return ret?1:0;
}
  
int Carea::ExplodeWedVertices()
{
  int ret;
  int i;

  KillWedVertexList();
  for(i=0;i<secheader.wallpolycnt;i++)
  {
    HandleWedVertex(wallpolygonheaders[i].firstvertex,wallpolygonheaders[i].countvertex);
  }
  for(i=0;i<doorpolygoncount;i++)
  {
    HandleWedVertex(doorpolygonheaders[i].firstvertex,doorpolygonheaders[i].countvertex);
  }
  ret=wedvertexcount;
  KillWedvertices();
  return ret?1:0;
}

int Carea::ExplodeDoorTiles()
{
  int ret;
  int i;

  KillDoorTileList();
  for(i=0;i<wedheader.doorcnt;i++)
  {
    HandleDoortile(weddoorheaders[i].firstdoortileidx, weddoorheaders[i].countdoortileidx);
  }
  ret=doortileidxcount;
  KillDoortiles();
  return ret?1:0;
}

int Carea::WriteMap(const char *suffix, unsigned char *pixels, COLORREF *pal, int palsize)
{
  CString filepath, tmpath, key;
  unsigned char *buffer=NULL;
  int fhandle;
  int fullsize;
  int bitspercolor;
  int scanline, width, height, x;
  int ret;

  RetrieveResref(key, header.wed, 0);
  if (key.GetLength()>6)
  {    
    return -4;
  }
  key+=suffix;
  filepath="override\\"+key+".BMP";
  tmpath=bgfolder+"override\\"+key+".TMP";
  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=0;
  width=m_width/GR_WIDTH;
  height=(m_height+GR_HEIGHT-1)/GR_HEIGHT;
  palsize*=sizeof(COLORREF);
  bitspercolor=palsize==64?4:8;
  scanline=GetScanLineLength(width,bitspercolor);
  fullsize=scanline*height;
  bmp_header header={'B','M',fullsize+sizeof(header)+palsize, 0,
  sizeof(header)+palsize, 40, width, height, (short) 1, (short) bitspercolor, 0, fullsize,
  0xb12, 0xb12,0,0};

  if(write(fhandle,&header,sizeof(header))!=sizeof(header))
  {
    ret=-2;
    goto endofquest;
  }

  if(write(fhandle,pal,palsize)!=palsize)
  {
    ret=-2;
    goto endofquest;
  }
  buffer=new unsigned char[scanline];
  if(!buffer)
  {
    ret=-3;
    goto endofquest;
  }
  memset(buffer,0,scanline);
  pixels+=height*width;
  while(height--)
  {
    pixels-=width;
    if(bitspercolor==8) memcpy(buffer, pixels, scanline);
    else
    {
      for(x=0;x<width;x++)
      {
        if(x&1) buffer[x/2]|=pixels[x];
        else buffer[x/2]=(unsigned char) (pixels[x]<<4);
      }      
    }
    if(write(fhandle,buffer,scanline)!=scanline)
    {
      ret=-2;
      goto endofquest;
    }
  }
endofquest:
  if(buffer) delete [] buffer;
  fullsize = filelength(fhandle);
  close(fhandle);
  if(!ret)
  {
    unlink(bgfolder+filepath);
    rename(tmpath,bgfolder+filepath);
    UpdateIEResource(key,REF_BMP,filepath,fullsize);
  }
  return ret;
}

int Carea::WriteWedToFile(int fh)
{
  int otc, dtc, dpc;
  int tmp, ret;
  int esize;
  int fullsize;
  int i;

  memcpy(&wedheader,"WED V1.3",8);

  if(heightmap)
  {
    if(changedmap[0])
    {
      if(m_smallpalette)
      {
        ret = WriteMap("HT",heightmap,htpal,16);
      }
      else
      {
        ret = WriteMap("HT",heightmap,ht8pal,256);
      }
      if (ret<0) return ret;
      changedmap[0]=false;
    }
  }
  if(lightmap)
  {
    if(changedmap[1])
    {
      if(m_night)
      {
        ret = WriteMap("LN",lightmap,lmpal,256);
      }
      else
      {
        ret = WriteMap("LM",lightmap,lmpal,256);
      }
      if (ret<0) return ret;
      changedmap[1]=false;
    }
  }
  if(searchmap)
  {
    if(changedmap[2])
    {
      ret = WriteMap("SR",searchmap,srpal,16);
      if (ret<0) return ret;
      changedmap[2]=false;
    }
  }
  if(!wedavailable) return -1; //internal error

  //adjusting wallgroups
  RecalcBoundingBoxes();

  fullsize=sizeof(wed_header);
  wedheader.overlayoffset=fullsize;
  fullsize+=wedheader.overlaycnt*sizeof(wed_overlay);
  wedheader.secheaderoffset=fullsize;
  fullsize+=sizeof(wed_secondary_header);
  wedheader.dooroffset=fullsize;
  fullsize+=wedheader.doorcnt*sizeof(wed_door);

  if(wedheader.overlaycnt!=5) return -1;

  otc=0;
  for(i=0;i<wedheader.overlaycnt;i++)
  {
    overlayheaders[i].tilemapoffset=fullsize;    
    otc+=overlayheaders[i].height*overlayheaders[i].width;
    fullsize+=overlayheaders[i].height*overlayheaders[i].width*sizeof(wed_tilemap);
  }
  dtc=0;
  dpc=0;
  wedheader.tilecelloffset=fullsize;
  for(i=0;i<weddoorcount;i++)
  {
    dtc+=weddoorheaders[i].countdoortileidx;
    dpc+=weddoorheaders[i].countpolygonopen+weddoorheaders[i].countpolygonclose;
  }
  fullsize+=dtc*sizeof(short);

  tmp=0;
  if(otc!=overlaytilecount)
  {
    return -1; //internal error
  }

  ImplodeDoortiles();
  ImplodeWedVertices();
  for(i=0;i<wallpolygoncount;i++)
  {
    RecalcBox(i,wallpolygonheaders, wedvertices+wallpolygonheaders[i].firstvertex);
  }
  for(i=0;i<doorpolygoncount;i++)
  {
    RecalcBox(i,doorpolygonheaders, wedvertices+doorpolygonheaders[i].firstvertex);
  }
  for(i=0;i<wedheader.overlaycnt;i++)
  {
    overlayheaders[i].tileidxoffset=fullsize+tmp*sizeof(short);
    tmp+=getotic(i);
  }
  fullsize+=overlaytileidxcount*sizeof(short);
  secheader.wallgroupoffset=fullsize;
  fullsize+=wallgroupidxcount*sizeof(wed_polyidx);
  secheader.polygonoffset=fullsize;
  secheader.wallpolycnt=wallpolygoncount;
  fullsize+=secheader.wallpolycnt*sizeof(wed_polygon);
  for(i=0;i<weddoorcount;i++)
  {
    weddoorheaders[i].offsetpolygonopen=fullsize;
    fullsize+=weddoorheaders[i].countpolygonopen*sizeof(wed_polygon);
    weddoorheaders[i].offsetpolygonclose=fullsize;
    fullsize+=weddoorheaders[i].countpolygonclose*sizeof(wed_polygon);
  }

  secheader.polygonidxoffset=fullsize;
  fullsize+=polygonidxcount*sizeof(short);
  secheader.verticeoffset=fullsize;
  fullsize+=wedvertexcount*sizeof(area_vertex);

  //write it
  fullsizew=sizeof(wed_header)+sizeof(wed_secondary_header);
  if(write(fh,&wedheader,sizeof(wed_header) )!=sizeof(wed_header) )
  {
    ret=-2;
    goto endofquest;
  }
  //writing overlay headers
  fullsizew+=esize=wedheader.overlaycnt*sizeof(wed_overlay);
  //hack for saving a night wed
  if(write(fh,overlayheaders,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }

  //writing secondary header
  if(write(fh,&secheader,sizeof(wed_secondary_header) )!=sizeof(wed_secondary_header) )
  {
    ret=-2;
    goto endofquest;
  }
  //writing doorheaders
  fullsizew+=esize=wedheader.doorcnt*sizeof(wed_door);
  if(write(fh,weddoorheaders,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  //converting back to indices
  ret=ConvertOffsetToIndex(secheader.polygonoffset+secheader.wallpolycnt*sizeof(wed_polygon), dpc);
  if(ret<0)
  {
    goto endofquest;
  }

  //writing overlay tileheaders
  fullsizew+=esize=overlaytilecount*sizeof(wed_tilemap);
  if(write(fh, overlaytileheaders, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  //writing door tileindices
  fullsizew+=esize=dtc*sizeof(short);
  if(write(fh,doortileindices, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=overlaytileidxcount*sizeof(short);
  if(write(fh,overlaytileindices, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=wallgroupidxcount*sizeof(wed_polyidx);
  if(write(fh,wallgroupindices, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=secheader.wallpolycnt*sizeof(wed_polygon);
  if(write(fh,wallpolygonheaders, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=doorpolygoncount*sizeof(wed_polygon);
  if(write(fh,doorpolygonheaders, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=polygonidxcount*sizeof(short);
  if(write(fh,polygonindices, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  fullsizew+=esize=wedvertexcount*sizeof(area_vertex);
  if(write(fh,wedvertices, esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }

  if(fullsizew==fullsize)
  {
    wedchanged=false;
    ret=0;
  }
  else ret=-2;

endofquest:
  ExplodeWedVertices();
  ExplodeDoorTiles();
  return ret;
}

void Carea::ConsolidateItems()
{
  area_item *newitems;
  int i,cnt,curcnt;

  cnt=0;
  for(i=0;i<containercount;i++)
  {
    cnt+=containerheaders[i].itemcount;
  }
  newitems = new area_item[cnt];
  if(!newitems)
  {
    return;
  }
  cnt=0;
  for(i=0;i<containercount;i++)
  {
    curcnt=containerheaders[i].itemcount;
    memcpy(newitems+cnt, itemheaders+containerheaders[i].firstitem, curcnt*sizeof(area_item));
    containerheaders[i].firstitem=cnt;
    cnt+=curcnt;
  }
  if(the_area.itemheaders)
  {
    delete [] the_area.itemheaders;
  }
  the_area.itemheaders=newitems;
  itemcount=cnt;
  header.itemcnt=(short) cnt;
}

int Carea::WriteAreaToFile(int fh, int calculate)
{
  int esize;
  int fullsize;
  int ret;
  int i;

  ImplodeVertices();
  ConsolidateItems();
  fullsize=sizeof(area_header);
  if(revision==91)
  {
    fullsize+=sizeof(iwd2_header);
    memcpy(header.filetype,"AREAV9.1",8);
  }
  else
  {
    memcpy(header.filetype,"AREAV1.0",8);
  }

  header.actoroffset=fullsize;
  fullsize+=header.actorcnt*sizeof(area_actor);             //actor headers
  for(i=0;i<actorcount;i++)                                 //actor data
  {
    if (actorheaders[i].cresize)
    {
      actorheaders[i].creoffset=fullsize;
    }
    else
    {
      actorheaders[i].creoffset=0;
    }
    fullsize+=actorheaders[i].cresize;
  }
  header.infooffset=fullsize;
  fullsize+=header.infocnt*sizeof(area_trigger);            //trigger (infopoint)
  header.spawnoffset=fullsize; 
  fullsize+=header.spawncnt*sizeof(area_spawn);             //spawn
  header.entranceoffset=fullsize; 
  fullsize+=header.entrancecnt*sizeof(area_entrance);       //entrance
  header.containeroffset=fullsize;
  fullsize+=header.containercnt*sizeof(area_container);     //container
  header.itemoffset=fullsize;
  fullsize+=header.itemcnt*sizeof(area_item);               //item
  header.ambioffset=fullsize;
  fullsize+=header.ambicnt*sizeof(area_ambient);            //ambient
  header.variableoffset=fullsize;
  fullsize+=header.variablecnt*sizeof(area_variable);       //variable
  header.exploredoffset=fullsize;
  fullsize+=header.exploredsize;                            //explored
  header.dooroffset=fullsize;
  fullsize+=header.doorcnt*sizeof(area_door);               //door
  header.tileoffset=fullsize;
  fullsize+=header.tilecnt*sizeof(area_tile);               //tiled objects
  header.vertexoffset=fullsize;
  fullsize+=header.vertexcnt*sizeof(area_vertex);           //vertex
  header.animationoffset=fullsize;
  fullsize+=header.animationcnt*sizeof(area_anim);          //animation
  //set these only when needed
  if(header.songoffset) header.songoffset=fullsize;
  fullsize+=sizeof(area_song);                              //song
  //set this only when needed
  if(header.intoffset)
  {
    header.intoffset=fullsize;
    fullsize+=sizeof(area_int);                             //interruption
  }
  if(header.mapnotecnt)
  {
    header.mapnoteoffset=fullsize;
    fullsize+=header.mapnotecnt*sizeof(area_mapnote);       //mapnotes
  }
  if(header.trapcnt && !pst_compatible_var())               //traps are not compatible with pst
  {
    for(i=0;i<trapcount;i++)
    {
      trapheaders[i].offset=fullsize;
      fullsize += trapheaders[i].size;                      //trap effects
    }
    header.pstmapnotecnt=fullsize;
    fullsize+=header.trapcnt*sizeof(area_trap);             //trap projectiles
  }

  if(calculate)
  {
    ExplodeVertices();
    return fullsize;
  }
  ret=0;
  if(fh<1)
  {
    ret=-1;
    goto endofquest;
  }
//header
  if(revision==91)
  { //iwd2 area format sucks (the new, empty unknowns come before the offsets)
    esize=(BYTE *) &header.actoroffset-(BYTE *) header.filetype;
    if(write(fh,&header,esize)!=esize)
    {
      ret=-2;
      goto endofquest;
    }
    if(write(fh,&iwd2header,sizeof(iwd2_header) )!=sizeof(iwd2_header) )
    {
      ret=-2;
      goto endofquest;
    }
    esize=sizeof(area_header)-esize;
    if(write(fh,&header.actoroffset,esize)!=esize)
    {
      ret=-2;
      goto endofquest;
    }
  }
  else
  {
    if(pst_compatible_var()) //quick hack for shifting the mapnotes in place
    {
      header.pstmapnotecnt=header.mapnotecnt;
      header.mapnotecnt=header.mapnoteoffset;
      header.mapnoteoffset=~0;
    }
    if(write(fh,&header,sizeof(area_header) )!=sizeof(area_header) )
    {	
      ret=-2;
      goto endofquest;
    }
    if(pst_compatible_var()) //quick hack for shifting the mapnotes in place
    {
      header.mapnoteoffset=header.mapnotecnt;
      header.mapnotecnt=header.pstmapnotecnt;
    }
  }
//actor
  esize=header.actorcnt*sizeof(area_actor);
  if(write(fh,actorheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  for(i=0;i<actorcount;i++)
  {
    esize=actorheaders[i].cresize;
    if(write(fh,credatapointers[i],esize)!=esize)
    {
      ret=-2;
      goto endofquest;
    }
  }
  //trigger (infopoint)
  esize=header.infocnt*sizeof(area_trigger);
  if(write(fh,triggerheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  //spawnpoint
  esize=header.spawncnt*sizeof(area_spawn);
  if(write(fh,spawnheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  //entrances
  esize=header.entrancecnt*sizeof(area_entrance);
  if(write(fh,entranceheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  //container
  esize=header.containercnt*sizeof(area_container);
  if(write(fh,containerheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //items
  esize=header.itemcnt*sizeof(area_item);
  if(write(fh,itemheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //ambients
  esize=header.ambicnt*sizeof(area_ambient);
  if(write(fh,ambientheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //variable
  esize=header.variablecnt*sizeof(area_variable);
  if(write(fh,variableheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //explored
  if(write(fh,explored,header.exploredsize )!=header.exploredsize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //door
  esize=header.doorcnt*sizeof(area_door);
  if(write(fh,doorheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //tiled
  esize=header.tilecnt*sizeof(area_tile);
  if(write(fh,tileheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //vertices
  esize=header.vertexcnt*sizeof(area_vertex);
  if(write(fh,vertexheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //animation
  esize=header.animationcnt*sizeof(area_anim);
  if(write(fh,animheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
  }
  
  //song
  if(header.songoffset)
  {/*
    for(int i=0;i<sizeof(songheader.unused);i+=4) {
      memcpy(songheader.unused+i, &songheader.songflag,4);
    }
    */
    if(write(fh,&songheader,sizeof(area_song) )!=sizeof(area_song) )
    {	
      ret=-2;
      goto endofquest;
    }
  }
  
//interruption
  if(header.intoffset)
  {
    if(write(fh,&intheader,sizeof(area_int) )!=sizeof(area_int) )
    {	
      ret=-2;
      goto endofquest;
    }
  }
  
  if(pst_compatible_var())
  {
    ConvertToPstMapnote();
    esize=header.mapnotecnt*sizeof(pst_area_mapnote);
    if(write(fh,pstmapnoteheaders,esize )!=esize )
    {
      delete[] pstmapnoteheaders;
      pstmapnoteheaders=NULL;
      ret=-2;
      goto endofquest;
    }
    delete[] pstmapnoteheaders;
    pstmapnoteheaders=NULL;
    m_changed=false;
  }
  else
  {
    esize=header.mapnotecnt*sizeof(area_mapnote);
    if(write(fh,mapnoteheaders,esize )!=esize )
    {
      ret=-2;
      goto endofquest;
    }
    int cnt, i;

    cnt = 0;
    for(i=0;i<trapcount;i++)
    {
      esize=trapheaders[i].size;
      if(write(fh,effects+cnt, esize)!=esize )
      {
        ret=-2;
        goto endofquest;
      }
      cnt +=esize/sizeof(creature_effect);
    }
    esize=header.trapcnt*sizeof(area_trap);
    if(write(fh,trapheaders,esize )!=esize )
    {
      ret=-2;
      goto endofquest;
    }
    m_changed=false;
  }
  
endofquest:
  ExplodeVertices();
  return ret;
}

//looks for entryname in the area opened
int Carea::CheckDestination(int fh, long ml, CString entryname)
{
  int flg, ret;
  int esize;
  int i;
  CString tmpdestination;
  
  ret=0;
  fhandlea=fh;
  if(ml==-1) maxlena=filelength(fhandlea);
  else maxlena=ml;
  if(maxlena<1) return -1; //short file, invalid item
  startpointa=tell(fhandlea);
  esize=(BYTE *) &header.actoroffset-(BYTE *) header.filetype;
  if(read(fhandlea,&header,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"AREA",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }
  if(memcmp(header.revision,"V1.0",4) )
  {
    if(memcmp(header.revision,"V9.1",4) )
    {
      return -2;
    }
    else
    {
      if(read(fhandlea,&iwd2header,sizeof(iwd2header))!=sizeof(iwd2header) )
      {
        return -2;
      }
      revision=91;
    }
  }
  else revision=10;

  //reading the second part of the main area header (offsets)
  esize=sizeof(area_header)-esize;
  if(read(fhandlea,&header.actoroffset,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }

  flg=adjust_actpointa(header.entranceoffset);
  if(flg<0)
  {
    return flg;
  }
  if(entrancecount!=header.entrancecnt)
  {
    KillEntrances();
    entranceheaders=new area_entrance[header.entrancecnt];
    if(!entranceheaders) return -3;
    entrancecount=header.entrancecnt;
  }
  esize=header.entrancecnt*sizeof(area_entrance);
  if(read(fhandlea,entranceheaders,esize )!=esize )
  {
    return -2;
  }
  for(i=0;i<header.entrancecnt;i++)
  {
    RetrieveVariable(tmpdestination,entranceheaders[i].entrancename);
    if(entryname==tmpdestination) return 0;
  }
  return 1; //cannot find it
}

CString Carea::RetrieveMosRef(int fh)
{
  CString tmpstr;
  int required;

  if(fh<1) return "";
  required=(BYTE *) &header.wed-(BYTE *) &header+8;
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return "";
  }
  close(fh);
  RetrieveResref(tmpstr,header.wed);
  return tmpstr;
}

//looks for entryname in the area opened
int Carea::FillDestination(int fh, long ml, CComboBox *cb)
{
  int flg, ret;
  int esize;
  int i;
  CString tmpdestination;

  cb->ResetContent();
  ret=0;
  fhandlea=fh;
  if(ml==-1) maxlena=filelength(fhandlea);
  else maxlena=ml;
  if(maxlena<1) return -1; //short file, invalid item
  startpointa=tell(fhandlea);
  esize=(BYTE *) &header.actoroffset-(BYTE *) header.filetype;
  if(read(fhandlea,&header,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"AREA",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }
  if(memcmp(header.revision,"V1.0",4) )
  {
    if(memcmp(header.revision,"V9.1",4) )
    {
      return -2;
    }
    else
    {
      if(read(fhandlea,&iwd2header,sizeof(iwd2header))!=sizeof(iwd2header) )
      {
        return -2;
      }
      revision=91;
    }
  }
  else revision=10;

  //reading the second part of the main area header (offsets)
  esize=sizeof(area_header)-esize;
  if(read(fhandlea,&header.actoroffset,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }

  flg=adjust_actpointa(header.entranceoffset);
  if(flg<0)
  {
    return flg;
  }
  if(entrancecount!=header.entrancecnt)
  {
    KillEntrances();
    entranceheaders=new area_entrance[header.entrancecnt];
    if(!entranceheaders) return -3;
    entrancecount=header.entrancecnt;
  }
  esize=header.entrancecnt*sizeof(area_entrance);
  if(read(fhandlea,entranceheaders,esize )!=esize )
  {
    return -2;
  }
  for(i=0;i<header.entrancecnt;i++)
  {
    RetrieveVariable(tmpdestination,entranceheaders[i].entrancename);
    cb->AddString(tmpdestination);
  }
  return 0;
}

void Carea::ConvertNightMap(int r, int g, int b, int strength)
{
  int i;
  BYTE *pClr;

  for(i=0;i<256;i++)
  {
    pClr = (BYTE *) (lmpal+i);
    pClr[0] = (pClr[0]*r)>>8;
    pClr[1] = (pClr[1]*g)>>8;
    pClr[2] = (pClr[2]*b)>>8;
    pClr[3] = (pClr[3]*strength)>>8;
  }
}

void Carea::MirrorMap(unsigned char *poi)
{
  int maxx,maxy;
  int x,y;
  unsigned char tmp;

  maxx=m_width/GR_WIDTH;
  maxy=(m_height+GR_HEIGHT-1)/GR_HEIGHT;
  for(y=0;y<maxy;y++)
  {
    for(x=0;x<maxx/2;x++)
    {
      tmp=poi[y*maxx+x];
      poi[y*maxx+x]=poi[(y+1)*maxx-x-1];
      poi[(y+1)*maxx-x-1]=tmp;
    }
  }
}

//door = -1 don't calculate vertex offset

void Carea::RecalcBox(int pos, wed_polygon *header, area_vertex *vertices)
{
  POINTS tmp, min, max;
  int count;
  int i;

  count=header[pos].countvertex;
  if(!count)
  {
    header[pos].maxx=-1;
    header[pos].maxy=-1;
    header[pos].minx=-1;
    header[pos].miny=-1;
    return;
  }
  
  min=max=(POINTS &) vertices[0];
  for(i=1;i<count;i++)
  {
    tmp=(POINTS &) vertices[i];
    if(max.x<tmp.x) max.x=tmp.x;
    if(max.y<tmp.y) max.y=tmp.y;
    if(min.x>tmp.x) min.x=tmp.x;
    if(min.y>tmp.y) min.y=tmp.y;
  }
  header[pos].maxx=max.x;
  header[pos].maxy=max.y;
  header[pos].minx=min.x;
  header[pos].miny=min.y;
}

//recalculate all bounding boxes for area_polygons
//sorting wallpolygons into wallgroups
int Carea::RecalcBoundingBoxes()
{
  int wgx, wgy;
  int minx,miny, maxx,maxy;
  int x,y;
  POSITION pos;
  void *poi;
  int count;
  int i;

  pos = wedvertexheaderlist.GetHeadPosition();
  //this is being done for all polygons
  for(i=0;i<wedvertexheaderlist.GetCount();i++)
  {
    if(i<secheader.wallpolycnt)
    {
      RecalcBox(i, wallpolygonheaders, (area_vertex *) wedvertexheaderlist.GetNext(pos) );
    }
    else
    {
      RecalcBox(i-secheader.wallpolycnt,doorpolygonheaders, (area_vertex *) wedvertexheaderlist.GetNext(pos) );
    }
  }
  wgx=(overlayheaders[0].width+9)/10;
  wgy=(overlayheaders[0].height*2+14)/15;
  wallgroupidxcount=wgx*wgy;
  if(wallgroupindices) 
  {
    delete [] wallgroupindices;
  }
  wallgroupindices=new wed_polyidx[wallgroupidxcount];
  if(!wallgroupindices) return -3;
  memset(wallgroupindices,0,wallgroupidxcount*sizeof(wed_polyidx) );
  pos = wedvertexheaderlist.GetHeadPosition();
//  for(i=0;i<secheader.wallpolycnt;i++)
  for(i=0;i<wedvertexheaderlist.GetCount();i++)
  {
    poi = wedvertexheaderlist.GetNext(pos);
    if(i<secheader.wallpolycnt)
    {
      minx=wallpolygonheaders[i].minx/640;
      maxx=wallpolygonheaders[i].maxx/640;
      miny=wallpolygonheaders[i].miny/480;
      maxy=wallpolygonheaders[i].maxy/480;
      count=wallpolygonheaders[i].countvertex;
    }
    else
    {
      minx=doorpolygonheaders[i-secheader.wallpolycnt].minx/640;
      maxx=doorpolygonheaders[i-secheader.wallpolycnt].maxx/640;
      miny=doorpolygonheaders[i-secheader.wallpolycnt].miny/480;
      maxy=doorpolygonheaders[i-secheader.wallpolycnt].maxy/480;
      count=doorpolygonheaders[i-secheader.wallpolycnt].countvertex;
    }
    if (minx<0) minx=0;
    if (miny<0) miny=0;
    if (maxx>=wgx) maxx=wgx-1;
    if (maxy>=wgy) maxy=wgy-1;
    for(x=minx;x<=maxx;x++) for(y=miny;y<=maxy;y++)
    {
      if(PolygonInBox( (area_vertex *) poi, count, CRect(x*640, y*480, (x+1)*640, (y+1)*480) ) )
      {
        wallgroupindices[y*wgx+x].count++;
      }
    }
  }
  polygonidxcount=0;
  for(i=0;i<wallgroupidxcount;i++)
  {
    wallgroupindices[i].index=(short) polygonidxcount;
    polygonidxcount+=wallgroupindices[i].count;
    wallgroupindices[i].count=0;
  }
  if(polygonindices)
  {
    delete [] polygonindices;
  }
  polygonindices=new short[polygonidxcount];
  pos = wedvertexheaderlist.GetHeadPosition();
//  for(i=0;i<secheader.wallpolycnt;i++)
  for(i=0;i<wedvertexheaderlist.GetCount();i++)
  {
    poi = wedvertexheaderlist.GetNext(pos);
    if(i<secheader.wallpolycnt)
    {
      minx=wallpolygonheaders[i].minx/640;
      maxx=wallpolygonheaders[i].maxx/640;
      miny=wallpolygonheaders[i].miny/480;
      maxy=wallpolygonheaders[i].maxy/480;
      count=wallpolygonheaders[i].countvertex;
    }
    else
    {
      minx=doorpolygonheaders[i-secheader.wallpolycnt].minx/640;
      maxx=doorpolygonheaders[i-secheader.wallpolycnt].maxx/640;
      miny=doorpolygonheaders[i-secheader.wallpolycnt].miny/480;
      maxy=doorpolygonheaders[i-secheader.wallpolycnt].maxy/480;
      count=doorpolygonheaders[i-secheader.wallpolycnt].countvertex;
    }
    if (minx<0) minx=0;
    if (miny<0) miny=0;
    if (maxx>=wgx) maxx=wgx-1;
    if (maxy>=wgy) maxy=wgy-1;
    for(x=minx;x<=maxx;x++) for(y=miny;y<=maxy;y++)
    {
      if(PolygonInBox((area_vertex *) poi, count, CRect(x*640, y*480, (x+1)*640, (y+1)*480) ) )
      {
        wed_polyidx *wgi=wallgroupindices+y*wgx+x;
        polygonindices[wgi->index+wgi->count++]=(short) i;
      }
    }
  }
  return 0;
}

int Carea::ConvertOffsetToIndex(int polyoffset, int &dpc)
{
  int ret;
  int i;

  dpc=0;
  ret=0;
  for(i=0;i<weddoorcount;i++)
  {
    weddoorheaders[i].offsetpolygonopen=(weddoorheaders[i].offsetpolygonopen-polyoffset)/sizeof(wed_polygon);
    if(weddoorheaders[i].offsetpolygonopen!=dpc)
    {
      ret = 1;
    }
    dpc+=weddoorheaders[i].countpolygonopen;
    weddoorheaders[i].offsetpolygonclose=(weddoorheaders[i].offsetpolygonclose-polyoffset)/sizeof(wed_polygon);
    if(weddoorheaders[i].offsetpolygonclose!=dpc)
    {
      ret = 1;
    }
    dpc+=weddoorheaders[i].countpolygonclose;
  }
  return ret;
}

int Carea::getotc(int overlay)
{
  int i, otc;

  otc=0;
  for(i=0;i<overlay;i++)
  {
    otc+=overlayheaders[i].height*overlayheaders[i].width;
  }
  return otc;
}

int Carea::getfoti(int overlay) //first overlay tile index
{
  int i, otc, foti;

  otc=getotc(overlay);
  foti=0;
  for(i=0;i<otc;i++)
  {    
    foti+=overlaytileheaders[i].counttileprimary;
  }
  return foti;
}

int Carea::getotic(int overlay) //overlay tile index count
{
  int i, otc, otic, first;

  first=getotc(overlay);
  otc=overlayheaders[overlay].height*overlayheaders[overlay].width;
  otic=0;
  for(i=0;i<otc;i++)
  {    
    otic+=overlaytileheaders[i+first].counttileprimary;
  }
  return otic;
}

int Carea::ReadMap(const char *Suffix, unsigned char *&Storage, COLORREF *Palette, int size)
{
  Cbam tmpbam;
  CString resname;
  int mapsize;
  int ret;

  RetrieveResref(resname,header.wed);
  resname+=Suffix;
  if(read_bmp(resname, &tmpbam, 0)) return -2;
  if(tmpbam.GetFrameCount()!=1) return -2;
  //if(size<tmpbam.m_palettesize) return -2;
  if(Storage)
  {
    delete [] Storage;
    Storage=NULL;
  }
  memset(Palette,0,size);
  if(size>tmpbam.m_palettesize) size=tmpbam.m_palettesize;
  memcpy(Palette, tmpbam.m_palette, size);
  mapsize=(m_width/GR_WIDTH)*((m_height+GR_HEIGHT-1)/GR_HEIGHT);
  Storage = new unsigned char[mapsize];
  if(!Storage) return -3;
  memset(Storage, 0, mapsize);
  ret = 0;
  if(tmpbam.m_palettesize!=size) ret = 1;
  size = tmpbam.GetFrameDataSize(0);
  if(size<mapsize)
  {
    mapsize=size;
    ret = 1;
  }
  memcpy(Storage, tmpbam.GetFrameData(0), mapsize);
  return ret;
}

int Carea::ReadWedFromFile(int fh, long ml)
{
  int flg, ret;
  int esize;
  int otc, dtc, otic, ootic;
  int dpc, wgc, wgpc;
  int wvc;
  int pos;
  int i;

  fhandlew=fh;
  if(ml==-1) maxlenw=filelength(fhandlew);
  else maxlenw=ml;
  if(maxlenw<1) return -1; //short file, invalid item
  startpointw=tell(fhandlew);
  fullsizew=sizeof(wedheader);
  if(read(fhandlew,&wedheader,fullsizew )!=fullsizew )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(wedheader.filetype,"WED ",4) )
  {
	  return -2;
  }

  //wed overlays
  if(wedheader.overlayoffset!=fullsizew)
  {
    return -2;
  }
  otc=0;
  wgc=0;
  if(overlaycount!=wedheader.overlaycnt)
  {
    KillOverlays();
    overlayheaders=new wed_overlay[wedheader.overlaycnt];
    if(!overlayheaders)
    {
      return -3;
    }
    overlaycount=wedheader.overlaycnt;
  }  
  esize=wedheader.overlaycnt*sizeof(wed_overlay);
  if(read(fhandlew,overlayheaders,esize )!=esize )
  {
    return -2;
  }
  if(wedheader.overlaycnt)
  {
    m_width=(unsigned short) (overlayheaders[0].width*64);
    m_height=(unsigned short) (overlayheaders[0].height*64);
  }
  else m_width=m_height=0;
  if(m_night)
  {
    ReadMap("LN", lightmap, lmpal, sizeof(lmpal) );
  }
  else
  {
    ReadMap("LM", lightmap, lmpal, sizeof(lmpal) );
  }
  ReadMap("SR", searchmap, srpal, sizeof(srpal) );
  m_smallpalette=true;
  ret = ReadMap("HT", heightmap, htpal, sizeof(htpal) );
  if(ret==1)
  {
    m_smallpalette=false;
    ReadMap("HT", heightmap, ht8pal, sizeof(ht8pal) );
  }
  wedchanged=false;
  for(ret=0;ret<3;ret++) changedmap[ret]=false;
  ret=0;

  fullsizew+=esize;
  //precalculating the size of the overlaytilemap
  for(i=0;i<overlaycount;i++)
  {
    otc+=overlayheaders[i].width*overlayheaders[i].height;
  }
  if(otc!=overlaytilecount)
  {
    KillOverlaytiles();
    overlaytileheaders=new wed_tilemap[otc];
    if(!overlaytileheaders)
    {
      return -3;
    }
    overlaytilecount=otc;
  }

  if(wedheader.secheaderoffset!=fullsizew )
  {
    return -2;
  }
  if(read(fhandlew,&secheader,sizeof(secheader) )!=sizeof(secheader) )
  {	
  	return -2; // short file, invalid item
  }
  fullsizew+=sizeof(secheader);

  //wed doors
  if(wedheader.dooroffset!=fullsizew)
  {
    return -2;
  }
  if(weddoorcount!=wedheader.doorcnt)
  {
    KillWeddoors();
    weddoorheaders=new wed_door[wedheader.doorcnt];
    if(!weddoorheaders)
    {
      return -3;
    }
    weddoorcount=wedheader.doorcnt;
  }

  esize=wedheader.doorcnt*sizeof(wed_door);
  if(read(fhandlew,weddoorheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizew+=esize;
  //precalculating the number of door tile indices, doorpolygonheaders
  
  ret|=ConvertOffsetToIndex(secheader.polygonoffset+secheader.wallpolycnt*sizeof(wed_polygon), dpc);
  if(ret<0) return -1;

  dtc=0;
  for(i=0;i<weddoorcount;i++)
  {
    dtc+=weddoorheaders[i].countdoortileidx;
  }
  if(dtc!=doortileidxcount)
  {
    KillDoortiles();
    doortileindices=new short[dtc];
    if(!doortileindices)
    {
      return -3;
    }
    doortileidxcount=dtc;
  }

  pos=0;
  for(i=0;i<wedheader.overlaycnt;i++)
  {
    //IETME saves doors first, therefore we need to seek the overlayheaders out
    //we seek each of them one by one to improve flexibility
    flg=adjust_actpointw(overlayheaders[i].tilemapoffset);
    if(flg<0) return flg;
    ret|=flg;
    
    esize=overlayheaders[i].height*overlayheaders[i].width*sizeof(wed_tilemap);
    if(read(fhandlew, overlaytileheaders+pos, esize )!=esize )
    {
      return -2;
    }
    fullsizew+=esize;
    pos+=overlayheaders[i].height*overlayheaders[i].width;    
  }
  if(pos!=otc) return -1; //hmm hmm hmm
  //precalculating overlay tilemap index counter
  otic=0;
  for(i=0;i<otc;i++)
  {
    otic+=overlaytileheaders[i].counttileprimary;
  }
  if(otic!=overlaytileidxcount)
  {
    KillOverlayindices();
    overlaytileindices=new short[otic];
    if(!overlaytileindices) return -3;
    overlaytileidxcount=otic;
  }

  //wed tiles
  flg=adjust_actpointw(wedheader.tilecelloffset);
  if(flg<0) return flg;
  ret|=flg;

  esize=dtc*sizeof(short);
  if(read(fhandlew,doortileindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  if(!overlaycount)
  {
    return -2;
  }
  pos=0;
  for(i=0;i<overlaycount;i++)
  {
    flg=adjust_actpointw(overlayheaders[i].tileidxoffset);
    if(flg<0) return flg;
    ret|=flg;

    ootic=getotic(i);
    esize=ootic*sizeof(short);
    if(read(fhandlew,overlaytileindices+pos, esize)!=esize)
    {
      return -2;
    }
    fullsizew+=esize;
    pos+=ootic;
    otic-=ootic; //should be 0
    if(otic<0) return -1;
  }
  if(otic) return -1;

  //wallgroups
  flg=adjust_actpointw(secheader.wallgroupoffset);
  if(flg<0) return flg;
  ret|=flg;

  wgc=((overlayheaders[0].width+9)/10)*((overlayheaders[0].height*2+14)/15);
  if(wgc!=wallgroupidxcount)
  {
    KillWallgroups();
    wallgroupindices=new wed_polyidx[wgc];
    if(!wallgroupindices) return -3;
    wallgroupidxcount=wgc;
  }
  esize=wgc*sizeof(wed_polyidx); //similar to POINTS structure (sizeof=4)
  if(read(fhandlew,wallgroupindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  wgpc=0;
  for(i=0;i<wgc;i++)
  {
    if(wallgroupindices[i].index!=wgpc) //a little consistency check
    {
      ret|=2;
    }
    //we use MAX to get the maximal extent of this region,
    //there could be holes/mixups inside
    wgpc=max(wgpc,wallgroupindices[i].index+wallgroupindices[i].count);
  }

  //allocating door polygons
  flg=adjust_actpointw(secheader.polygonoffset);
  if(flg<0) return flg;
  ret|=flg;

  if(dpc!=doorpolygoncount)
  {
    KillDoorpolygons();
    doorpolygonheaders=new wed_polygon[dpc];
    if(!doorpolygonheaders) return -3;
    doorpolygoncount=dpc;
  }

  //wallgroup polygons
  if(secheader.wallpolycnt!=wallpolygoncount)
  {
    KillWallgrouppolygons();
    wallpolygonheaders=new wed_polygon[secheader.wallpolycnt];
    if(!wallpolygonheaders) return -3;
    wallpolygoncount=secheader.wallpolycnt;
  }
  esize=wallpolygoncount*sizeof(wed_polygon);
  if(read(fhandlew,wallpolygonheaders,esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  //reading door polygons
  esize=doorpolygoncount*sizeof(wed_polygon);
  if(read(fhandlew,doorpolygonheaders,esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  //precalculating vertex size
  wvc=0;
  for(i=0;i<wallpolygoncount;i++)
  {
    if(wallpolygonheaders[i].firstvertex!=wvc)
    {
      ret|=1;
    }
    wvc=max(wvc,wallpolygonheaders[i].firstvertex+wallpolygonheaders[i].countvertex);
  }
  //here is the limit between door/wallgroup, normally
  for(i=0;i<doorpolygoncount;i++)
  {
    if(doorpolygonheaders[i].firstvertex!=wvc)
    {
      ret|=1;
    }
    wvc=max(wvc,doorpolygonheaders[i].firstvertex+doorpolygonheaders[i].countvertex);
  }

  flg=adjust_actpointw(secheader.polygonidxoffset);
  if(flg<0) return flg;
  ret|=flg;

  if(wgpc!=polygonidxcount)
  {
    KillPolygonindices();
    polygonindices=new short[wgpc];
    if(!polygonindices) return -3;
    polygonidxcount=wgpc;
  }
  esize=wgpc*sizeof(short);
  if(read(fhandlew,polygonindices,esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  //finally: vertices
  //we cannot read the vertices separated, because IETME dumps door polygons among
  //wallgroup polygons
  flg=adjust_actpointw(secheader.verticeoffset);
  if(flg<0) return flg;
  ret|=flg;
  if(wvc!=wedvertexcount)
  {
    KillWedvertices();
    wedvertices=new area_vertex[wvc];  //'official' vertex type: short x,y;
    if(!wedvertices) return -3;
    wedvertexcount=wvc;
  }
  esize=wvc*sizeof(area_vertex);
  if(read(fhandlew,wedvertices,esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize;

  flg=ExplodeDoorTiles();
  if(flg<0) return flg;
  ret|=flg;

  flg=ExplodeWedVertices();
  if(flg<0) return flg;
  ret|=flg;

  wedavailable=true;
  if(fullsizew!=maxlenw)
  {
    ret|=2;
  }
  return ret;
}

extern int read_string(FILE *fpoi, char *pattern, char *tmpbuff=NULL, int length=0);
extern int read_line_until(FILE *fpoi, CString line);
extern void SetupReader(int max);

int Carea::LoadSVG(int fhandle)
{
  int res;
  FILE *fpoi;
  CString tmpstr;
  CString *points;
  int i;
  int count;
  int a,b;
  area_vertex *newvertices;
  wed_polygon *newpolygons;
  POSITION pos;

  maxlen=filelength(fhandle);
  if(maxlen<1) return -1; //short file, invalid item

  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }

  SetupReader(maxlen);  

  do
  {
    res = read_string( fpoi, "\n", external, sizeof(external) );
    if (!res) break;
    if (memcmp(external,"<polygon points=\"",17)) continue;
    
    ((CChitemDlg *) AfxGetMainWnd())->log("%s",external);

    for(i=17;external[i]!='\"';i++);
    tmpstr = CString(external+17, i-17);
    points = explode(tmpstr, ' ', count);
    if (count<3) continue;

    newvertices = new area_vertex[count];

    for(i=0;i<count;i++)
    {
      sscanf(points[i],"%d,%d",&a,&b);
      newvertices[i].x = (unsigned short) a;
      newvertices[i].y = (unsigned short) b;
    }
    delete [] points;


    newpolygons = new wed_polygon [the_area.secheader.wallpolycnt+1];
    if(!newpolygons)
    {
      delete [] newvertices;
      return -3;
    }
    
    memcpy(newpolygons,the_area.wallpolygonheaders,the_area.secheader.wallpolycnt*sizeof(wed_polygon) );
    memcpy(newpolygons+the_area.secheader.wallpolycnt+1,the_area.wallpolygonheaders+the_area.secheader.wallpolycnt,(the_area.secheader.wallpolycnt-the_area.secheader.wallpolycnt)*sizeof(wed_polygon) );
    memset(newpolygons+the_area.secheader.wallpolycnt,0,sizeof(wed_polygon) );
    newpolygons[the_area.secheader.wallpolycnt].unkflags=255;
    delete [] the_area.wallpolygonheaders;
    the_area.wallpolygonheaders=newpolygons;

    pos=the_area.wedvertexheaderlist.FindIndex(the_area.secheader.wallpolycnt);
    the_area.wedvertexheaderlist.InsertBefore(pos,newvertices);
    the_area.secheader.wallpolycnt++;
    the_area.wallpolygoncount=the_area.secheader.wallpolycnt;
  }
  while(res==1);
  the_area.RecalcBoundingBoxes();
  the_area.wedchanged=true;
  return res;
}

int Carea::LoadPLY(int fhandle)
{
  int res;
  FILE *fpoi;
  CString tmpstr;
  CString *points;
  int i;
  int count;
  int a,b;
  area_vertex *newvertices;
  wed_polygon *newpolygons;
  POSITION pos;

  maxlen=filelength(fhandle);
  if(maxlen<1) return -1; //short file, invalid item

  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }

  SetupReader(maxlen);  
  //skip version header
  res = read_string( fpoi, "\n", external, sizeof(external) );
  do
  {
    res = read_string( fpoi, "\n", external, sizeof(external) );
    if (res) break;
    
    ((CChitemDlg *) AfxGetMainWnd())->log("%s",external);

    points = explode(tmpstr, ' ', count);
    if (count<3) continue;

    newvertices = new area_vertex[count];

    for(i=0;i<count;i++)
    {
      sscanf(points[i],"%d,%d",&a,&b);
      newvertices[i].x = (unsigned short) a;
      newvertices[i].y = (unsigned short) b;
    }
    delete [] points;

    newpolygons = new wed_polygon [the_area.secheader.wallpolycnt+1];
    if(!newpolygons)
    {
      delete [] newvertices;
      return -3;
    }
    
    memcpy(newpolygons,the_area.wallpolygonheaders,the_area.secheader.wallpolycnt*sizeof(wed_polygon) );
    memcpy(newpolygons+the_area.secheader.wallpolycnt+1,the_area.wallpolygonheaders+the_area.secheader.wallpolycnt,(the_area.secheader.wallpolycnt-the_area.secheader.wallpolycnt)*sizeof(wed_polygon) );
    memset(newpolygons+the_area.secheader.wallpolycnt,0,sizeof(wed_polygon) );
    newpolygons[the_area.secheader.wallpolycnt].unkflags=255;
    delete [] the_area.wallpolygonheaders;
    the_area.wallpolygonheaders=newpolygons;

    pos=the_area.wedvertexheaderlist.FindIndex(the_area.secheader.wallpolycnt);
    the_area.wedvertexheaderlist.InsertBefore(pos,newvertices);
    the_area.secheader.wallpolycnt++;
    the_area.wallpolygoncount=the_area.secheader.wallpolycnt;  
  }
  while(!res);
  return res;
}

int Carea::SavePLY(int fhandle)
{
  int i,j,count;
  POSITION pos;
  char tmp[20];

  write(fhandle,"PLY V2.0\r\n",10);

  for (i=0;i<the_area.secheader.wallpolycnt;i++)
  {
    count = the_area.wallpolygonheaders[i].countvertex;
    pos = the_area.wedvertexheaderlist.FindIndex(i);
    area_vertex *poly = (area_vertex *) the_area.wedvertexheaderlist.GetAt(pos);
    for (j=0;j<count;j++)
    {
      if (j) write(fhandle, ", ",2);
      sprintf(tmp, "%d.%d\r\n", poly[j].x, poly[j].y);
      write(fhandle, tmp, strlen(tmp));
    }
  }
  return 0;
}

int Carea::ReadActorData()
{
  long oldpos;
  int flg;
  int i;

  KillCreDataPointers();
  credatapointers=new char*[actorcount];
  if(!credatapointers) return -3;
  for(i=0;i<actorcount;i++)
  {
    if(actorheaders[i].creoffset && actorheaders[i].cresize)
    {
      credatapointers[i]=new char[actorheaders[i].cresize];
      if(!credatapointers[i]) return -3;
      oldpos=tell(fhandlea);
      if(lseek(fhandlea,actorheaders[i].creoffset,SEEK_SET)==actorheaders[i].creoffset)
      {
        flg=read(fhandlea,credatapointers[i],actorheaders[i].cresize)!=actorheaders[i].cresize;
        lseek(fhandlea,oldpos,SEEK_SET);
      }
      else flg=1;
      if(flg) return -2;
      fullsizea+=actorheaders[i].cresize; 
    }
    else 
    {
      credatapointers[i]=NULL;
      actorheaders[i].creoffset=0;
      actorheaders[i].cresize=0;
    }
  }

  return 0;
}

void Carea::ConvertToPstMapnote()
{
  int i;

  if(pstmapnoteheaders) delete[] pstmapnoteheaders;
  pstmapnoteheaders = new pst_area_mapnote[header.mapnotecnt];
  for(i=0;i<header.mapnotecnt;i++)
  {
    pstmapnoteheaders[i].px=mapnoteheaders[i].px;
    pstmapnoteheaders[i].py=mapnoteheaders[i].py;
    CString tmpstr=resolve_tlk_text(mapnoteheaders[i].strref);
    int len = tmpstr.GetLength();
    if(len>256) len=256;
    strncpy(pstmapnoteheaders[i].text, tmpstr, len);
    pstmapnoteheaders[i].readonly=!!mapnoteheaders[i].colour;
  }
}

void Carea::ConvertFromPstMapnote()
{
  int i;

  if(mapnoteheaders) delete[] mapnoteheaders;
  mapnoteheaders = new area_mapnote[header.mapnotecnt];
  memset(mapnoteheaders,0,header.mapnotecnt*sizeof(area_mapnote) );
  for(i=0;i<header.mapnotecnt;i++)
  {
    mapnoteheaders[i].px=(unsigned short) pstmapnoteheaders[i].px;
    mapnoteheaders[i].py=(unsigned short) pstmapnoteheaders[i].py;
    mapnoteheaders[i].strref=store_tlk_text(-1,pstmapnoteheaders[i].text);
    mapnoteheaders[i].colour=(unsigned short) pstmapnoteheaders[i].readonly;
  }
  delete[] pstmapnoteheaders;
  pstmapnoteheaders=NULL;
}

int Carea::ReadAreaFromFile(int fh, long ml)
{
  int flg, ret;
  int esize;

  wedavailable=false;
  wedchanged=false;
  m_night=false;
  for(ret=0;ret<3;ret++) changedmap[ret]=false;
  m_changed=false;
  if(overlaycount)
  {
    overlayheaders[0].height=9999;
    overlayheaders[0].width=9999;
  }
  ret=0;
  fhandlea=fh;
  if(ml==-1) maxlena=filelength(fhandlea);
  else maxlena=ml;
  if(maxlena<1) return -1; //short file, invalid item
  startpointa=tell(fhandlea);
  fullsizea=sizeof(header);
  esize=(BYTE *) &header.actoroffset-(BYTE *) header.filetype;
  if(read(fhandlea,&header,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"AREA",4) )
  {
	  return -2;
  }
  if(memcmp(header.revision,"V1.0",4) )
  {
    if(memcmp(header.revision,"V9.1",4) )
    {
      return -2;
    }
    else
    {
      if(read(fhandlea,&iwd2header,sizeof(iwd2header))!=sizeof(iwd2header) )
      {
        return -2;
      }
      fullsizea+=sizeof(iwd2header);
      revision=91;
    }
  }
  else revision=10;

  //reading the second part of the main area header (offsets)
  esize=sizeof(area_header)-esize;
  if(read(fhandlea,&header.actoroffset,esize )!=esize )
  {	
  	return -2; // short file, invalid item
  }
  if(pst_compatible_var()) //quick hack for shifting the mapnotes in place
  {
    header.mapnoteoffset=header.mapnotecnt;
    header.mapnotecnt=header.pstmapnotecnt;
  }
  //actors
  if(header.actorcnt)
  {
    flg=adjust_actpointa(header.actoroffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(actorcount!=header.actorcnt)
  {
    KillActors();
    actorheaders=new area_actor[header.actorcnt];
    if(!actorheaders)
    {
      return -3;
    }
    actorcount=header.actorcnt;
  }
  esize=header.actorcnt*sizeof(area_actor);
  if(read(fhandlea,actorheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;
  flg=ReadActorData();
  if(flg<0) return flg;
  //infopoints
  if(header.infocnt)
  {
    flg=adjust_actpointa(header.infooffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(triggercount!=header.infocnt)
  {
    KillTriggers();
    triggerheaders=new area_trigger[header.infocnt];
    if(!triggerheaders)
    {
      return -3;
    }
    triggercount=header.infocnt;
  }
  esize=header.infocnt*sizeof(area_trigger);
  if(read(fhandlea,triggerheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //spawns
  if(header.spawncnt)
  {
    flg=adjust_actpointa(header.spawnoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(spawncount!=header.spawncnt)
  {
    KillSpawns();
    spawnheaders=new area_spawn[header.spawncnt];
    if(!spawnheaders)
    {
      return -3;
    }
    spawncount=header.spawncnt;
  }
  esize=header.spawncnt*sizeof(area_spawn);
  if(read(fhandlea,spawnheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //entrances
  if(header.entrancecnt)
  {
    flg=adjust_actpointa(header.entranceoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(entrancecount!=header.entrancecnt)
  {
    KillEntrances();
    entranceheaders=new area_entrance[header.entrancecnt];
    if(!entranceheaders)
    {
      return -3;
    }
    entrancecount=header.entrancecnt;
  }
  esize=header.entrancecnt*sizeof(area_entrance);
  if(read(fhandlea,entranceheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //containers
  if(header.containercnt)
  {
    flg=adjust_actpointa(header.containeroffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(containercount!=header.containercnt)
  {
    KillContainers();
    containerheaders=new area_container[header.containercnt];
    if(!containerheaders)
    {
      return -3;
    }
    containercount=header.containercnt;
  }
  esize=header.containercnt*sizeof(area_container);
  if(read(fhandlea,containerheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //items
  if(header.itemcnt)
  {
    flg=adjust_actpointa(header.itemoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(itemcount!=header.itemcnt)
  {
    KillItems();
    itemheaders=new area_item[header.itemcnt];
    if(!itemheaders)
    {
      return -3;
    }
    itemcount=header.itemcnt;
  }
  esize=header.itemcnt*sizeof(area_item);
  if(read(fhandlea,itemheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //ambisounds
  if(header.ambicnt)
  {
    flg=adjust_actpointa(header.ambioffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(ambientcount!=header.ambicnt)
  {
    KillAmbients();
    ambientheaders=new area_ambient[header.ambicnt];
    if(!ambientheaders) return -3;
    ambientcount=header.ambicnt;
  }
  esize=header.ambicnt*sizeof(area_ambient);
  if(read(fhandlea,ambientheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //variables
  if(header.variablecnt)
  {
    flg=adjust_actpointa(header.variableoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(variablecount!=header.variablecnt)
  {
    KillVariables();
    variableheaders=new area_variable[header.variablecnt];
    if(!variableheaders) return -3;
    variablecount=header.variablecnt;
  }
  esize=header.variablecnt*sizeof(area_variable);
  if(read(fhandlea,variableheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  if(tileflagcount!=header.tileflagcnt)
  {
    KillVariables();
    tileflagheaders=new area_tileflag[header.tileflagcnt];
    if(!tileflagheaders) return -3;
    tileflagcount=header.tileflagcnt;
  }
  esize=header.tileflagcnt*sizeof(area_tileflag);
  if(read(fhandlea,tileflagheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //explored
  if(header.exploredsize)
  {
    flg=adjust_actpointa(header.exploredoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }
  
  if(exploredsize!=header.exploredsize)
  {
    KillExplored();
    explored=new char[header.exploredsize];
    if(!explored) return -3;
    exploredsize=header.exploredsize;
  }
  if(read(fhandlea,explored,exploredsize)!=exploredsize)
  {
    return -2;
  }
  fullsizea+=header.exploredsize;

  //door
  if(header.doorcnt)
  {
    flg=adjust_actpointa(header.dooroffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(doorcount!=header.doorcnt)
  {
    KillDoors();
    doorheaders=new area_door[header.doorcnt];
    if(!doorheaders) return -3;
    doorcount=header.doorcnt;
  }
  esize=header.doorcnt*sizeof(area_door);
  if(read(fhandlea,doorheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //tiled objects
  if(header.tilecnt)
  {
    flg=adjust_actpointa(header.tileoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }
  
  if(tilecount!=header.tilecnt)
  {
    KillTiles();
    tileheaders=new area_tile[header.tilecnt];
    if(!tileheaders) return -3;
    tilecount=header.tilecnt;
  }
  esize=header.tilecnt*sizeof(area_tile);
  if(read(fhandlea,tileheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //tile flags objects
  if(header.tileflagcnt)
  {
    flg=adjust_actpointa(header.tileflagoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }
  
  if(tileflagcount!=header.tileflagcnt)
  {
    KillTileFlags();
    tileflagheaders=new area_tileflag[header.tileflagcnt];
    if(!tileflagheaders) return -3;
    tileflagcount=header.tileflagcnt;
  }
  esize=header.tileflagcnt*sizeof(area_tileflag);
  if(read(fhandlea,tileflagheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //vertices
  if(header.vertexcnt)
  {
    flg=adjust_actpointa(header.vertexoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(vertexcount!=header.vertexcnt)
  {
    KillVertices();
    vertexheaders=new area_vertex[header.vertexcnt];
    if(!vertexheaders) return -3;
    vertexcount=header.vertexcnt;
  }
  esize=header.vertexcnt*sizeof(area_vertex);
  if(read(fhandlea,vertexheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //animation
  if(header.animationcnt)
  {
    flg=adjust_actpointa(header.animationoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(animcount!=header.animationcnt)
  {
    KillAnimations();
    animheaders=new area_anim[header.animationcnt];
    if(!animheaders) return -3;
    animcount=header.animationcnt;
  }
  esize=header.animationcnt*sizeof(area_anim);
  if(read(fhandlea,animheaders,esize )!=esize )
  {
    return -2;
  }
  fullsizea+=esize;

  //song offset
  if(header.songoffset)
  {
    flg=adjust_actpointa(header.songoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;

    if(read(fhandlea,&songheader,sizeof(area_song) )!=sizeof(area_song) )
    {
      return -2;
    }
    fullsizea+=sizeof(area_song);
  }

  //interruption
  if(header.intoffset)
  {
    flg=adjust_actpointa(header.intoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;

    if(read(fhandlea,&intheader,sizeof(area_int) )!=sizeof(area_int) )
    {
      return -2;
    }
    fullsizea+=sizeof(area_int);
  }

  //mapnotes
  if(!header.mapnotecnt)
  {
    if(header.mapnoteoffset)
    {
      header.mapnoteoffset=0;
      ret=1;
    }
  }

  //pst was converted to common format before
  if(header.mapnotecnt)
  {
    flg=adjust_actpointa(header.mapnoteoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

  if(pst_compatible_var())
  {
    if(mapnotecount!=header.mapnotecnt)
    {
      KillMapnotes();
      pstmapnoteheaders=new pst_area_mapnote[header.mapnotecnt];
      if(!pstmapnoteheaders) return -3;
      mapnotecount=header.mapnotecnt;
    }
    esize=header.mapnotecnt*sizeof(pst_area_mapnote);
    if(read(fhandlea,pstmapnoteheaders,esize )!=esize )
    {
      return -2;
    }
    ConvertFromPstMapnote();
    fullsizea+=esize;
  }
  else
  {
    if(mapnotecount!=header.mapnotecnt)
    {
      KillMapnotes();
      mapnoteheaders=new area_mapnote[header.mapnotecnt];
      if(!mapnoteheaders) return -3;
      mapnotecount=header.mapnotecnt;
    }
    esize=header.mapnotecnt*sizeof(area_mapnote);
    if(read(fhandlea,mapnoteheaders,esize )!=esize )
    {
      return -2;
    }
    fullsizea+=esize;

    if(header.trapcnt)
    {
      flg=adjust_actpointa(header.pstmapnotecnt); //also trapoffset
      if(flg<0) return flg;
      if(flg) ret|=flg;
    }
    if(trapcount!=header.trapcnt)
    {
      KillTraps();
      trapheaders=new area_trap[header.trapcnt];
      if(!trapheaders) return -3;
      trapcount=header.trapcnt;
    }
    esize=header.trapcnt*sizeof(area_trap);
    if(read(fhandlea,trapheaders,esize )!=esize )
    {
      return -2;
    }
    fullsizea+=esize;

    int i,cnt;
    esize = 0;
    cnt = 0;
    for(i=0;i<trapcount;i++)
    {
      cnt+=trapheaders[i].size/sizeof(creature_effect);
    }
    if(effectcount!=cnt)
    {
      KillEffects();
      effects=new creature_effect[cnt];
      if(!effects) return -3;
      effectcount=cnt;
    }

    cnt = 0;
    for(i=0;i<trapcount;i++)
    {
      flg=adjust_actpointa(trapheaders[i].offset);
      if(flg<0) return flg;
      if(flg) ret|=flg;
      esize = trapheaders[i].size;
      if (esize>(effectcount-cnt)*(int) sizeof(creature_effect))
      {
        return -2;
      }
      if(read(fhandlea,effects+cnt,esize )!=esize )
      {
        return -2;
      }
      cnt+=esize/sizeof(creature_effect);
      fullsizea+=esize;
    }
  }

  if(fullsizea!=maxlena)
  {
    ret=2;
  }
  flg=ExplodeVertices();
  if(flg<0) return flg;
  if(flg) ret|=flg;
  return ret;
}

#define DEF_OVERLAY_CNT  5
int Carea::DefaultAreaOverlays()
{
  wedheader.overlaycnt=DEF_OVERLAY_CNT;
  overlaycount=DEF_OVERLAY_CNT;
  overlayheaders=new wed_overlay[wedheader.overlaycnt];
  if(!overlayheaders)
  {
    return -3;
  }
  memset(overlayheaders,0,DEF_OVERLAY_CNT*sizeof(wed_overlay) );
  return 0;
}

int Carea::RemoveDoorPolygon(int first)
{
  wed_polygon *newpolygons;
  POSITION pos;
  int i;

  pos=wedvertexheaderlist.FindIndex(wallpolygoncount+first);
  if(!pos) return -1;
  newpolygons=new wed_polygon[--doorpolygoncount];
  if(!newpolygons)
  {
    doorpolygoncount++;
    return -3;
  }
  for(i=0;i<weddoorcount;i++)
  {
    if(weddoorheaders[i].offsetpolygonopen>first)
    {
      weddoorheaders[i].offsetpolygonopen--;
    }
    if(weddoorheaders[i].offsetpolygonclose>first)
    {
      weddoorheaders[i].offsetpolygonclose--;
    }
  }
  memcpy(newpolygons, doorpolygonheaders, first*sizeof(wed_polygon));
  memcpy(newpolygons+first, doorpolygonheaders+first+1,(doorpolygoncount-first)*sizeof(wed_polygon) );
  delete [] doorpolygonheaders;
  doorpolygonheaders=newpolygons;
  wedvertexheaderlist.RemoveAt(pos);//this also frees the polygon
  return 0;
}

int Carea::AddWedDoor(CString doorid)
{
  int i;
  wed_door *newweddoors;
  wed_polygon *newpolygonheaders;
  area_vertex *newvertex;
  short *newtiles;

  for(i=0;i<the_area.wedheader.doorcnt;i++)
  {
    if(!strnicmp(doorid, the_area.weddoorheaders[i].doorid,8) )
    {
      return 1;
    }
  }

  the_area.wedchanged=true;
  //wed door (TODO)
  newweddoors=new wed_door[++the_area.wedheader.doorcnt];
  if(!newweddoors)
  {
    the_area.wedheader.doorcnt--;
    return -3;
  }
  memcpy(newweddoors, the_area.weddoorheaders, the_area.weddoorcount*sizeof(wed_door) );
  memset(newweddoors+the_area.weddoorcount,0,sizeof(wed_door) );
  StoreResref(doorid,newweddoors[the_area.weddoorcount].doorid);
  newweddoors[the_area.weddoorcount].firstdoortileidx=(short) the_area.doortileidxcount;
  newweddoors[the_area.weddoorcount].closed=1;
  newweddoors[the_area.weddoorcount].countpolygonopen=1;
  newweddoors[the_area.weddoorcount].countpolygonclose=1;
  newweddoors[the_area.weddoorcount].offsetpolygonopen=the_area.doorpolygoncount;
  newweddoors[the_area.weddoorcount].offsetpolygonclose=the_area.doorpolygoncount+1;
  delete [] the_area.weddoorheaders;
  the_area.weddoorheaders= newweddoors;

  //adding 2 empty polygons (open/closed)
  newpolygonheaders = new wed_polygon[the_area.doorpolygoncount+2];
  if(!newpolygonheaders)
  {
    newweddoors[the_area.weddoorcount].countpolygonopen=0;
    newweddoors[the_area.weddoorcount].countpolygonclose=0;
    return -3;
  }
  memcpy(newpolygonheaders, the_area.doorpolygonheaders, the_area.doorpolygoncount*sizeof(wed_polygon) );
  memset(newpolygonheaders+the_area.doorpolygoncount,0,2*sizeof(wed_polygon) );
  newpolygonheaders[the_area.doorpolygoncount].firstvertex=the_area.wedvertexcount;
  newpolygonheaders[the_area.doorpolygoncount+1].firstvertex=the_area.wedvertexcount+1;
  newpolygonheaders[the_area.doorpolygoncount].flags=0x89;
  newpolygonheaders[the_area.doorpolygoncount+1].flags=0x89;
  newpolygonheaders[the_area.doorpolygoncount].unkflags=255;
  newpolygonheaders[the_area.doorpolygoncount+1].unkflags=255;
  delete [] the_area.doorpolygonheaders;
  the_area.doorpolygonheaders=newpolygonheaders;
  the_area.doorpolygoncount+=2;

  newvertex=new area_vertex[0];
  the_area.wedvertexheaderlist.AddTail(newvertex);
  newvertex=new area_vertex[0];
  the_area.wedvertexheaderlist.AddTail(newvertex);  

  the_area.weddoorcount=the_area.wedheader.doorcnt;

  newtiles=new short[0];
  the_area.doortilelist.AddTail(newtiles);
  return 0;
}

int Carea::GetWedDoorIndex(char *doorid)
{
  int newcount;
  int i;

  newcount=0;
  for(i=0;i<wedheader.doorcnt;i++)
  {
    //the door isn't about to remove
    if(strnicmp(doorid, weddoorheaders[i].doorid,8) )
    {
      newcount++;
    }
  }
  return newcount;
}

int Carea::RemoveWedDoor(char *doorid)
{
  wed_door *newdoors;
  POSITION pos;
  int newcount;
  int i,j;

  newcount=GetWedDoorIndex(doorid);

  if(newcount==wedheader.doorcnt) return 1; //wed didn't need any change
  newdoors=new wed_door[newcount];
  if(!newdoors) return -3;
  j=0;
  for(i=0;i<wedheader.doorcnt;i++)
  {
    //we keep this door
    if(strnicmp(doorid, weddoorheaders[i].doorid,8) )
    {
      newdoors[j++]=weddoorheaders[i];
    }
    else //we don't keep this door
    {
      ////TODO
      while(weddoorheaders[i].countpolygonopen)
      {
        RemoveDoorPolygon(weddoorheaders[i].offsetpolygonopen);
        weddoorheaders[i].countpolygonopen--;
      }
      while(weddoorheaders[i].countpolygonclose)
      {
        RemoveDoorPolygon(weddoorheaders[i].offsetpolygonclose);
        weddoorheaders[i].countpolygonclose--;
      }
      pos=doortilelist.FindIndex(i);
      if(pos)
      {
        doortilelist.RemoveAt(pos);
      }
    }
  }
  wedchanged=true;
  if(weddoorheaders)
  {
    delete [] weddoorheaders;
  }
  weddoorheaders=newdoors;
  weddoorcount=wedheader.doorcnt=newcount;
  return 0;
}
