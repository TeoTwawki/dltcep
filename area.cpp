// area.cpp: implementation of the Carea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>

#include "chitem.h"
#include "area.h"

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
  tileheaders=NULL;
  vertexheaders=NULL;
  explored=NULL;
  credatapointers=NULL;

  overlayheaders=NULL;
  overlaytileheaders=NULL;
  overlaytileindices=NULL;
  weddoorheaders=NULL;
  doortileindices=NULL;
  doorpolygonheaders=NULL;
  wallgroupheaders=NULL;
  wallgroupindices=NULL;
  polygonindices=NULL;
  wedvertices=NULL;
  wallgroupvertices=NULL;
  doorvertices=NULL;

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
  tilecount=0;
  vertexcount=0;
  exploredsize=0;

  overlaycount=0;
  overlaytilecount=0;
  overlaytileidxcount=0;
  weddoorcount=0;
  doortileidxcount=0;
  wallgroupcount=0;
  doorpolygoncount=0;
  polygonidxcount=0;
  wedvertexcount=0;
  wallgroupvertexcount=0;
  doorvertexcount=0;

  revision=10;
  wedchanged=true;
  wedavailable=true;
  changedmap[0]=changedmap[1]=changedmap[2]=true;
  memset(&intheader,0,sizeof(intheader) );
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
  KillTiles();
  KillVertices();
  KillVertexList();
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
  KillWallgroupvertices();
  KillDoorvertices();
  KillMaps();
  
  wedchanged=true;
  wedavailable=true;
  changedmap[0]=changedmap[1]=changedmap[2]=true;
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

//This function orders the vertices of a polygon (without ruining it)
int Carea::VertexOrder(area_vertex *wedvertex, int count)
{
  int i;
  int shift;
  area_vertex *tmp;
  area_vertex max;

  max=wedvertex[0];
  shift=0;
  for(i=1;i<count;i++)
  {
    if(wedvertex[i].point.x<max.point.x)
    {
      shift=i;
      max=wedvertex[i];
    }
    else
    {
      if(wedvertex[i].point.x==max.point.x)
      {
        if(wedvertex[i].point.y>max.point.y)
        {
          shift=i;
          max=wedvertex[i];
        }
      }
    }
  }
  if(shift)
  {
    tmp=new area_vertex[count];
    if(!tmp)
    {
      return -3;
    }
    for(i=0;i<count;i++)
    {
      tmp[i]=wedvertex[(i+shift)%count];
    }
    memcpy(wedvertex,tmp,count*sizeof(area_vertex) );
    delete [] tmp;
  }
  return 0;
}

void Carea::ShiftVertex(long id, int count, POINTS &point)
{
  area_vertex *poi;
  int i;

  poi=(area_vertex *) vertexheaderlist.GetAt(vertexheaderlist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    poi[i].point.x=(short) (poi[i].point.x+point.x);
    poi[i].point.y=(short) (poi[i].point.y+point.y);
  }
}

void Carea::FlipVertex(long id, int count, int width)
{
  area_vertex *poi;
  int i;

  poi=(area_vertex *) vertexheaderlist.GetAt(vertexheaderlist.FindIndex(id));
  for(i=0;i<count;i++)
  {
    poi[i].point.x=(short) (width-poi[i].point.x);
  }
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
  //first count all the vertices
  vertexcount=0;
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

  KillVertexList();
  if(header.vertexcnt!=vertexcount) return -1; //internal error
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

int Carea::ExplodeVertices()
{
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

  if(vertexcount)
  {
    KillVertices();
    return 1; //something is fishy here
  }
  KillVertices();
  return 0;
}

int Carea::WriteMap(const char *suffix, unsigned char *pixels, COLORREF *pal, int palsize)
{
  CString filepath, tmpath;
  unsigned char *buffer=NULL;
  int fhandle;
  int fullsize;
  int bitspercolor;
  int scanline, width, height, x;
  int ret;

  filepath=bgfolder+"override\\"+header.wed+suffix+".BMP";
  tmpath=bgfolder+"override\\"+header.wed+suffix+".TMP";
  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=0;
  width=the_area.width/GR_WIDTH;
  height=the_area.height/GR_HEIGHT;
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
  close(fhandle);
  if(!ret)
  {
    unlink(filepath);
    rename(tmpath,filepath);
  }
  return ret;
}

int Carea::WriteWedToFile(int fh, int night)
{
  int otc, dtc, dpc;
  int tmp;
  int esize;
  int fullsize;
  int i;

  memcpy(&wedheader,"WED V1.3",8);

  if(heightmap)
  {
    if(changedmap[0])
    {
      WriteMap("HT",heightmap,htpal,16);
      changedmap[0]=false;
    }
  }
  if(lightmap)
  {
    if(changedmap[1])
    {
      WriteMap("LM",lightmap,lmpal,256);
      changedmap[1]=false;
    }
  }
  if(searchmap)
  {
    if(changedmap[2])
    {
      WriteMap("SR",searchmap,srpal,16);
      changedmap[2]=false;
    }
  }
  if(!wedavailable) return -1; //internal error

  //adjusting offsets
  RecalcBoundingBoxes();
  fullsize=sizeof(wed_header);
  wedheader.overlayoffset=fullsize;
  fullsize+=wedheader.overlaycnt*sizeof(wed_overlay);
  wedheader.secheaderoffset=fullsize;
  fullsize+=sizeof(wed_secondary_header);
  wedheader.dooroffset=fullsize;
  fullsize+=wedheader.doorcnt*sizeof(wed_door);

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
  if(otc!=overlaytilecount) return -1; //internal error
  for(i=0;i<wedheader.overlaycnt;i++)
  {
    overlayheaders[i].tileidxoffset=fullsize+tmp*sizeof(short);
    tmp+=overlayheaders[i].height*overlayheaders[i].width;
  }
  fullsize+=overlaytileidxcount*sizeof(short);
  secheader.wallgroupoffset=fullsize;
  fullsize+=wallgroupidxcount*sizeof(wed_polyidx);
  secheader.polygonoffset=fullsize;
  secheader.wallgroupcnt=wallgroupcount;
  fullsize+=secheader.wallgroupcnt*sizeof(wed_polygon);
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
  fullsize+=(wallgroupvertexcount+doorvertexcount)*sizeof(area_vertex);

  //write it
  fullsizew=sizeof(wed_header)+sizeof(wed_secondary_header);
  if(write(fh,&wedheader,sizeof(wed_header) )!=sizeof(wed_header) )
  {
    return -2;
  }
  //writing overlay headers
  fullsizew+=esize=wedheader.overlaycnt*sizeof(wed_overlay);
  //hack for saving a night wed
  if(night)
  {
    for(i=0;i<wedheader.overlaycnt;i++)
    {
      overlayheaders[i].tis[6]='N';
    }
  }
  if(write(fh,overlayheaders,esize)!=esize)
  {
    return -2;
  }
  if(night)
  {
    for(i=0;i<wedheader.overlaycnt;i++)
    {
      overlayheaders[i].tis[6]=0;
    }
  }

  //writing secondary header
  if(write(fh,&secheader,sizeof(wed_secondary_header) )!=sizeof(wed_secondary_header) )
  {
    return -2;
  }
  //writing doorheaders
  fullsizew+=esize=wedheader.doorcnt*sizeof(wed_door);
  if(write(fh,weddoorheaders,esize)!=esize)
  {
    return -2;
  }
  //converting back to indices
  tmp=ConvertOffsetToIndex(secheader.polygonoffset+secheader.wallgroupcnt*sizeof(wed_polygon));
  if(tmp<0) return tmp;

  //writing overlay tileheaders
  fullsizew+=esize=overlaytilecount*sizeof(wed_tilemap);
  if(write(fh, overlaytileheaders, esize)!=esize)
  {
    return -2;
  }
  //writing door tileindices
  fullsizew+=esize=dtc*sizeof(short);
  if(write(fh,doortileindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=overlaytileidxcount*sizeof(short);
  if(write(fh,overlaytileindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=wallgroupidxcount*sizeof(wed_polyidx);
  if(write(fh,wallgroupindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=secheader.wallgroupcnt*sizeof(wed_polygon);
  if(write(fh,wallgroupheaders, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=doorpolygoncount*sizeof(wed_polygon);
  if(write(fh,doorpolygonheaders, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=polygonidxcount*sizeof(short);
  if(write(fh,polygonindices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=wallgroupvertexcount*sizeof(area_vertex);
  if(write(fh,wallgroupvertices, esize)!=esize)
  {
    return -2;
  }
  fullsizew+=esize=doorvertexcount*sizeof(area_vertex);
  if(write(fh,doorvertices, esize)!=esize)
  {
    return -2;
  }

  if(fullsizew==fullsize)
  {
    wedchanged=false;
    return 0;
  }
  return -2;
}

int Carea::WriteAreaToFile(int fh, int calculate)
{
  int esize;
  int fullsize;
  int ret;

  ImplodeVertices();
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
  fullsize+=header.actorcnt*sizeof(area_actor);             //actor
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
  if(header.intoffset) header.intoffset=fullsize;
  fullsize+=sizeof(area_int);                               //interruption
  if(header.mapnotecnt)
  {
    header.mapnoteoffset=fullsize;
    fullsize+=header.mapnotecnt*sizeof(area_mapnote);         //mapnotes
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
    if(write(fh,&header,sizeof(area_header) )!=sizeof(area_header) )
    {	
      ret=-2;
      goto endofquest;
    }
  }
//actor
  esize=header.actorcnt*sizeof(area_actor);
  if(write(fh,actorheaders,esize )!=esize )
  {	
    ret=-2;
    goto endofquest;
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
  {
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
  
  esize=header.mapnotecnt*sizeof(area_mapnote);
  if(write(fh,mapnoteheaders,esize )!=esize )
  {
    ret=-2;
    goto endofquest;
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

  if(fh<1) return -1;
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

//door = -1 don't calculate vertex offset
//door = 0 wallgroupvertex
//door = xxx doorvertex

void Carea::RecalcBox(int pos, wed_polygon *header, area_vertex *vertices, int door)
{
  POINTS tmp, min, max;
  int idx,count;
  int i;

  if(!header[pos].countvertex)
  {
    header[pos].maxx=-1;
    header[pos].maxy=-1;
    header[pos].minx=-1;
    header[pos].miny=-1;
    return;
  }
  if(door!=-1)
  {
    idx=header[pos].firstvertex;
    idx-=door;
  }
  else idx=0;
  count=header[pos].countvertex;
  min=max=vertices[idx].point;
  for(i=1;i<count;i++)
  {
    tmp=vertices[i+idx].point;
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
int Carea::RecalcBoundingBoxes()
{
  int i;

  for(i=0;i<doorpolygoncount;i++)
  {
    RecalcBox(i,doorpolygonheaders, doorvertices, wallgroupvertexcount);
  }
  for(i=0;i<wallgroupcount;i++)
  {
    RecalcBox(i,wallgroupheaders, wallgroupvertices,0);
  }
  return 0;
}

//cleaning up holes, mixups and other fuckups
//recreating Wallgroup vertices, removing the Wedvertices
int Carea::CleanUpVertices()
{
  int i;
  int wvc, dvc;
  int pos, pos2;
  int ret;

  wvc=0;
  for(i=0;i<wallgroupcount;i++)
  {
    wvc+=wallgroupheaders[i].countvertex;
  }
  if(wallgroupvertexcount!=wvc)
  {
    KillWallgroupvertices();
    wallgroupvertices = new area_vertex[wvc];
    if(!wallgroupvertices) return -3;
    wallgroupvertexcount=wvc;
  }
  ret=0;
  pos=0;
  for(i=0;i<wallgroupcount;i++)
  {
    memcpy(wallgroupvertices+pos,wedvertices+wallgroupheaders[i].firstvertex,wallgroupheaders[i].countvertex*sizeof(area_vertex) );
    if(wallgroupheaders[i].firstvertex!=pos)
    {
      wallgroupheaders[i].firstvertex=pos;
      wedchanged=true;
      ret=1;
    }
    pos+=wallgroupheaders[i].countvertex;
  }

  dvc=0;
  for(i=0;i<doorpolygoncount;i++)
  {
    dvc+=doorpolygonheaders[i].countvertex;
  }
  if(doorvertexcount!=dvc)
  {
    KillDoorvertices();
    doorvertices = new area_vertex[dvc];
    if(!doorvertices) return -3;
    doorvertexcount=dvc;
  }
  pos2=0;
  for(i=0;i<doorpolygoncount;i++)
  {
    memcpy(doorvertices+pos2,wedvertices+doorpolygonheaders[i].firstvertex,doorpolygonheaders[i].countvertex*sizeof(area_vertex) );
    if(doorpolygonheaders[i].firstvertex!=pos+pos2)
    {
      doorpolygonheaders[i].firstvertex=pos+pos2;
      wedchanged=true;
      ret=1;
    }
    pos2+=doorpolygonheaders[i].countvertex;
  }

  pos2=0;
  for(i=0;i<wallgroupidxcount;i++)
  {
    if(wallgroupindices[i].index!=pos2)
    {
      wallgroupindices[i].index=(short) pos2;
      wedchanged=true;
      ret=1;
    }
    pos2+=wallgroupindices[i].count;
  }
  for(i=0;i<polygonidxcount;i++)
  {
    if(polygonindices[i]>=wallgroupcount+doorpolygoncount)
    {
      polygonindices[i]=0; //quick hack
      ret=2;
    }
  }
  KillWedvertices();
  return ret;
}

//read all polygonheaders, remove unused vertices
int Carea::ConsolidateVertices()
{
  area_vertex *newvertices;
  int newvertexcount;
  int pos;
  int i;

  newvertexcount=0;
  for(i=0;i<wallgroupcount;i++)
  {
    newvertexcount+=wallgroupheaders[i].countvertex;
  }
  newvertices=new area_vertex[newvertexcount];
  if(!newvertices) return -3;
  pos=0;
  for(i=0;i<wallgroupcount;i++)
  {
    memcpy(newvertices+pos, wallgroupvertices+wallgroupheaders[i].firstvertex,wallgroupheaders[i].countvertex*sizeof(area_vertex) );
    if(wallgroupheaders[i].firstvertex!=pos)
    {
      wallgroupheaders[i].firstvertex=pos;
      wedchanged=true;
    }
    pos+=wallgroupheaders[i].countvertex;
  }

  delete [] wallgroupvertices;
  wallgroupvertices=newvertices;
  wallgroupvertexcount=newvertexcount;

  newvertexcount=0;
  for(i=0;i<doorpolygoncount;i++)
  {
    newvertexcount+=doorpolygonheaders[i].countvertex;
  }
  newvertices=new area_vertex[newvertexcount];
  if(!newvertices) return -3;
  
  //don't clear pos, it now contains wallgroupvertexcount
  for(i=0;i<doorpolygoncount;i++)
  {
    memcpy(newvertices+(pos-wallgroupvertexcount),
           doorvertices+(doorpolygonheaders[i].firstvertex-wallgroupvertexcount),
           doorpolygonheaders[i].countvertex*sizeof(area_vertex) );
    if(doorpolygonheaders[i].firstvertex!=pos)
    {
      doorpolygonheaders[i].firstvertex=pos;
      wedchanged=true;
    }
    pos+=doorpolygonheaders[i].countvertex;
  }
  delete [] doorvertices;
  doorvertices=newvertices;
  doorvertexcount=newvertexcount;
  return 0;
}

int Carea::ConsolidateDoortiles()
{
  short *newdoortileindices;
  int newdoortileidxcount;
  int pos;
  int i;

  newdoortileidxcount=0;
  for(i=0;i<weddoorcount;i++)
  {
    newdoortileidxcount+=weddoorheaders[i].countdoortileidx;
  }
  newdoortileindices=new short[newdoortileidxcount];
  if(!newdoortileindices) return -3;
  pos=0;
  for(i=0;i<weddoorcount;i++)
  {
    memcpy(newdoortileindices+pos, doortileindices+weddoorheaders[i].firstdoortileidx,weddoorheaders[i].countdoortileidx*sizeof(short) );
    if(weddoorheaders[i].firstdoortileidx!=pos)
    {
      weddoorheaders[i].firstdoortileidx=(short) pos;
      wedchanged=true;
    }
    pos+=weddoorheaders[i].countdoortileidx;
  }
  delete [] doortileindices;
  doortileindices=newdoortileindices;
  doortileidxcount=newdoortileidxcount;
  return 0;
}

int Carea::ConvertOffsetToIndex(int polyoffset)
{
  int i;
  int dpc;

  dpc=0;
  for(i=0;i<weddoorcount;i++)
  {
    weddoorheaders[i].offsetpolygonopen=(weddoorheaders[i].offsetpolygonopen-polyoffset)/sizeof(wed_polygon);
    if(weddoorheaders[i].offsetpolygonopen!=dpc)
    {
      return -1;
    }
    dpc+=weddoorheaders[i].countpolygonopen;
    weddoorheaders[i].offsetpolygonclose=(weddoorheaders[i].offsetpolygonclose-polyoffset)/sizeof(wed_polygon);
    if(weddoorheaders[i].offsetpolygonclose!=dpc)
    {
      return -1;
    }
    dpc+=weddoorheaders[i].countpolygonclose;
  }
  return dpc;
}

int Carea::getfoti(int overlay) //first overlay tile index
{
  int i, otc, foti;

  otc=0;
  for(i=0;i<overlay-1;i++)
  {
    otc+=overlayheaders[i].height*overlayheaders[i].width;
  }
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

  first=0;
  for(i=0;i<overlay;i++)
  {
    first+=overlayheaders[i].height*overlayheaders[i].width;
  }
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

  if(Storage)
  {
    delete [] Storage;
    Storage=NULL;
  }
  RetrieveResref(resname,the_area.header.wed);
  resname+=Suffix;
  if(read_bmp(resname, &tmpbam, 0)) return -2;
  if(tmpbam.GetFrameCount()!=1) return -2;
  if(size<tmpbam.m_palettesize) return -2;
  Storage = tmpbam.GetFrameData(0);
  memset(Palette,0,size);
  memcpy(Palette, tmpbam.m_palette, tmpbam.m_palettesize);
  return tmpbam.DetachFrameData(0);
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

  ret=0;
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
  width=(unsigned short) (overlayheaders[0].width*64);
  height=(unsigned short) (overlayheaders[0].height*64);
  ReadMap("LM", lightmap, lmpal, sizeof(lmpal) );
  ReadMap("SR", searchmap, srpal, sizeof(srpal) );
  ReadMap("HT", heightmap, htpal, sizeof(htpal) );

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
  
  dpc=ConvertOffsetToIndex(secheader.polygonoffset+secheader.wallgroupcnt*sizeof(wed_polygon));
  if(dpc<0) return dpc;

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
  if(secheader.wallgroupcnt!=wallgroupcount)
  {
    KillWallgrouppolygons();
    wallgroupheaders=new wed_polygon[secheader.wallgroupcnt];
    if(!wallgroupheaders) return -3;
    wallgroupcount=secheader.wallgroupcnt;
  }
  esize=wallgroupcount*sizeof(wed_polygon);
  if(read(fhandlew,wallgroupheaders,esize)!=esize)
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
  for(i=0;i<wallgroupcount;i++)
  {
    if(wallgroupheaders[i].firstvertex!=wvc)
    {
      ret|=4;
    }
    wvc=max(wvc,wallgroupheaders[i].firstvertex+wallgroupheaders[i].countvertex);
  }
  //here is the limit between door/wallgroup, normally
  for(i=0;i<doorpolygoncount;i++)
  {
    if(doorpolygonheaders[i].firstvertex!=wvc)
    {
      ret|=4;
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

  flg=ConsolidateDoortiles();
  if(flg<0) return flg;
  ret|=flg;

  flg=CleanUpVertices();
  if(flg<0) return flg;
  ret|=flg;

  wedavailable=true;
  if(fullsizew!=maxlenw)
  {
    ret|=2;
  }
  return ret;
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
    if(actorheaders[i].creoffset)
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
    else credatapointers[i]=NULL;
  }

  return 0;
}

int Carea::ReadAreaFromFile(int fh, long ml)
{
  int flg, ret;
  int esize;

  wedavailable=false;
  wedchanged=false;
  for(ret=0;ret<3;ret++) changedmap[ret]=false;
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
  if(header.mapnoteoffset==-1)
  {
    header.mapnotecnt=0; //hack to fix this inconsistency in saved pst areas
  }
  if(header.mapnotecnt)
  {
    flg=adjust_actpointa(header.mapnoteoffset);
    if(flg<0) return flg;
    if(flg) ret|=flg;
  }

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

  if(fullsizea!=maxlena)
  {
    ret=2;
  }
  flg=ExplodeVertices();
  if(flg<0) return flg;
  if(flg) ret|=flg;
  return ret;
}
