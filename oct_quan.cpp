// OCT_QUAN.CPP - Octree Color Quantization Class

#include "stdafx.h"

#include <stdio.h>
#include "oct_quan.h"
#include "chitem.h"
#include "options.h"
#include "Dl1quant.h"

// Color palette matching
// Build octree
// quantizeall colors deletes the tree
int OctQuant::QuantizeAllColors(LPBYTE pFrameData, const DWORD *pRawBits, CPoint nDimension, palettetype &palette)
{
  int nPixelNum=nDimension.x*nDimension.y;
  OctColor color;
  int i;
  int qualityloss;
  int qcolor; 

  qualityloss=0;
  for(i=0;i<nPixelNum;i++)
  {
    memcpy(&color.data,pRawBits+i,3);
    qcolor = QuantizeColor(proot, color);
    pFrameData[i]=(BYTE) qcolor;
    qualityloss+=ChiSquare((LPBYTE) &color.data,(LPBYTE) (palette+qcolor) );
  }
  DeleteTree();
  return qualityloss;
}

int OctQuant::QuantizeAllColors(LPBYTE pFrameData, const LPBYTE pRawBits, CPoint nDimension,
                                palettetype &oldpalette, palettetype &palette)
{
  int nPixelNum=nDimension.x*nDimension.y;
  OctColor color;
  int i;
  int qualityloss;
  int qcolor; 

  qualityloss=0;
  for(i=0;i<nPixelNum;i++)
  {
    //this will work only if both bams got the 0. index as transparency, but it is default
    if(pRawBits[i])
    {
      memcpy(&color.data,oldpalette+pRawBits[i],3);
      qcolor = QuantizeColor(proot, color);
    }
    else
    {
      qcolor=0;
    }
    pFrameData[i]=(BYTE) qcolor;
    qualityloss+=ChiSquare((LPBYTE) &color.data,(LPBYTE) (palette+qcolor) );
  }
  DeleteTree();
  return qualityloss;
}

int OctQuant::AddPalette(palettetype &palette)
{
  OctColor color;
  int i;

  if(!proot)
  {
    if ((proot = MakeNode(0)) == NULL)
      return -1;
  }
  for (i = 0; i < 256; i++)
  {
      // Insert pixel color into octree
      memcpy(&color.data,palette+i,3);      
      if (InsertNode(proot, color, i ) == FALSE)
      {
        DeleteTree();      // Delete the octree
        return -1;
      }

      // Reduce octree if too many colors
      if (num_leaf > O_NumColor)
        ReduceTree();
  }
  return 0;
}

int OctQuant::BuildTree(const DWORD *pRawBits, LPBYTE pFrameData, CPoint nDimension, palettetype &palette)
{
  int nPixelNum=nDimension.x*nDimension.y;
  int i;
  OctColor color;

  if(editflg&OCTREE)
  {
    return dl1quant((LPBYTE) pRawBits, pFrameData, nDimension, palette);
  }
  if(proot) return -1;
  // Allocate octree root node
  if ((proot = MakeNode(0)) == NULL)
    return -1;

  // Build the octree
  for (i = 0; i < nPixelNum; i++)
  {
      // Insert pixel color into octree
      memcpy(&color.data,pRawBits+i,3);      
      if (InsertNode(proot, color, -1 ) == FALSE)
      {
        DeleteTree();      // Delete the octree
        return -1;
      }
      
      // Reduce octree if too many colors
      if (num_leaf > O_NumColor)
        ReduceTree();
  }
  i=0;
  FillPalette(proot, palette, &i);
  return QuantizeAllColors(pFrameData, pRawBits, nDimension, palette);
}

// Recursively delete octree nodes
void OctQuant::DeleteNode( OctNode *pn )
{
  int i;        // Loop index
  OctNode *pc;  // Child node pointer

  if (pn == NULL)
    return;

  if (pn->IsLeaf() == FALSE)
  {
    // Delete child nodes
    for (i = 0; i < 8; i++)
    {
      if ((pc = pn->GetChild(i)) != NULL)
      {
        DeleteNode(pc);
        pn->SetChild(i, NULL);
        pn->DecNumChild();
      }
    }
  }
  else
    num_leaf--;

  delete pn;
  pn=NULL;
}

// Set color palette entries
void OctQuant::FillPalette( OctNode *pn, palettetype &Palette, int *pindex)
{
  int i;    // Loop index

  // Perform recursive depth-first traversal of octree
  if (pn != NULL)
  {
    if ((pn->IsLeaf() == TRUE) || pn->GetLevel() ==
        leaf_level)
    {
      // Set color palette entry
      Palette[*pindex] = pn->GetColor();

      // Set node color palette index
      pn->SetIndex(*pindex);

      // Advance to next color palette entry
      *pindex = *pindex + 1;
    }
    else
    {
      // Visit child nodes
      for (i = 0; i < 8; i++)
        FillPalette(pn->GetChild(i), Palette, pindex);
    }
  }
}

// Get next reducible node pointer
OctNode *OctQuant::GetReducible()
{
  int new_level;        // New reducible node level
  OctNode *prn;         // Reducible node pointer
  OctNode *plcn = NULL; // Largest pixel count node
  OctNode *pnext;       // Next node
  OctNode *pprev;       // Previous node

  new_level = leaf_level - 1;

  // Find lowest reducible node level 
  while (prnl[new_level] == NULL)
    new_level--;

  // Find node with largest pixel count
  prn = prnl[new_level];
  while (prn != NULL)
  {
    if (plcn == NULL)
      plcn = prn;
    else if (prn->GetCount() < plcn->GetCount())
      plcn = prn;
    prn = prn->GetNext();
  }
  
  // Remove node from reducible list
  pnext = plcn->GetNext();
  pprev = plcn->GetPrev();
  if (pprev == NULL)
  {
    prnl[new_level] = pnext;
    if (pnext != NULL)
      pnext->SetPrev(NULL);
  }
  else
  {
    pprev->SetNext(pnext);
    if (pnext != NULL)
      pnext->SetPrev(pprev);
  }

  plcn->SetNext(NULL);
  plcn->SetPrev(NULL);
  plcn->SetMark(FALSE);
  
  return plcn;
}

void OctQuant::ReduceTree()     // Reduce octree
{
  int i;        // Loop index
  OctNode *pn;  // Node pointer
  OctNode *pc;  // Child node pointer

  pn = GetReducible();  // Get next reducible node

  // Delete children
  for (i = 0; i < 8; i++)
  {
    if ((pc = pn->GetChild(i)) != NULL)
    {
      DeleteNode(pc);
      pn->SetChild(i, NULL);
      pn->DecNumChild();
    }
  }

  pn->SetLeaf(TRUE);    // Mark node as leaf
  num_leaf++;           // Increment leaf count

  // Update reduction and leaf levels
  if (pn->GetLevel() < (leaf_level - 1))
    leaf_level = pn->GetLevel() + 1;
}

// Insert node into octree
BOOL OctQuant::InsertNode( OctNode *pn, OctColor &c, int idx)
{
  int c_index;          // Child index
  int level;            // Node level
  BOOL status = TRUE;   // Return status
  OctNode *pc;          // Child node pointer

  level = pn->GetLevel();       // Get node level
  pn->AddColor(c);              // Add RGB color to node

  if (pn->IsLeaf() == FALSE && level < leaf_level)
  {
    // Find child node
    c_index = pn->FindChild(c);
    if ((pc = pn->GetChild(c_index)) != NULL)
    {
      if ((pn->GetNumChild() > 1) && (pn->IsMark() ==
          FALSE))
      {
        // Mark node as candidate for reduction
        MakeReducible(pn);
      }
    }
    else
    {
      // Allocate child node
      if ((pc = MakeNode(level + 1)) == NULL)
        return FALSE;

      // Set child node pointer
      pn->SetChild(c_index, pc);

      pn->IncNumChild();      // Increment child count      
    }

    // Insert child node into octree
    status = InsertNode(pc, c, idx);
  }
  if(idx!=-1) pn->SetIndex(idx);

  return status;
}
