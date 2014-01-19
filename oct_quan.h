// OCT_QUAN.H - Octree Color Quantization Class
// from Ian Ashdown's article on octree color quantization

#ifndef _OCT_QUAN_H
#define _OCT_QUAN_H

#include "structs.h"

// Maximum octree reduction level
#define O_MaxDepth 7

// Number of quantized colors
#define O_NumColor 256

class OctColor          // 24-bit RGB color model
{
public:
  struct
  {
    BYTE red;
    BYTE green;
    BYTE blue;
  } data;

    BYTE GetBlue() { return data.blue; }
    BYTE GetGreen() { return data.green; }
    BYTE GetRed() { return data.red; }
    void SetBlue( BYTE b ) { data.blue = b; }
    void SetGreen( BYTE g ) { data.green = g; }
    void SetRed( BYTE r ) { data.red = r; }
    bool IsEmpty() { return (data.blue+data.green+data.red)==0; }
};

class OctNode           // Octree node
{                       
  private:
    int level;          // Node level
    BOOL leaf_flag;     // Leaf flag
    BOOL mark_flag;     // Marked flag
    DWORD count;        // Pixel count
    struct
    {
      DWORD red;
      DWORD green;
      DWORD blue;
    }
    sum;                // Summed color value
    int index;          // Color palette index
    int children;       // Number of child nodes
    OctNode *pchild[8]; // Children node pointers
    OctNode *pnext;     // Next reducible node pointer
    OctNode *pprev;     // Previous reducible node pointer

    int TestBit(BYTE val, int index)
    { return ((val & (1 << index)) > index) ? 1 : 0; }

  public:
    OctNode( int node_level, BOOL leaf )
    {
      int i;    // Loop index

      level = node_level;
      leaf_flag = leaf;
      mark_flag = FALSE;
      count = 0L;
      index = 0;
      children = 0;
      sum.red = sum.green = sum.blue = 0L;

      for (i = 0; i < 8; i++)
        pchild[i] = NULL;

      pnext = pprev = NULL;
    };

    BOOL IsLeaf() { return leaf_flag; }
    BOOL IsMark() { return mark_flag; }
    
    DWORD GetCount() { return count; }

    COLORREF GetColor()
    {
      return RGB((sum.red / count),(sum.green / count),(sum.blue / count));

    }

    int GetIndex() { return index; }
    int GetLevel() { return level; }

    // Determine child node according to color
    int FindChild( OctColor &c )
    {
      int index;    // Child node pointer index

      // Determine child node pointer index
      index = TestBit(c.GetRed(), O_MaxDepth - level) << 2 |
          TestBit(c.GetGreen(), O_MaxDepth - level) << 1 |
          TestBit(c.GetBlue(), O_MaxDepth - level);

      return index;
    }

    OctNode *GetChild( int i ) { return pchild[i]; }
    OctNode *GetNext() { return pnext; }
    OctNode *GetPrev() { return pprev; }
    int GetNumChild() { return children; }

    // Add RGB color to node
    void AddColor( OctColor &c )
    {
      sum.red += (DWORD) c.GetRed();
      sum.green += (DWORD) c.GetGreen();
      sum.blue += (DWORD) c.GetBlue();

      count++;
    }

    void DecNumChild() { children--; }
    void IncNumChild() { children++; }
    void SetChild( int i, OctNode *pc ) { pchild[i] = pc; }
    void SetIndex( int i ) { index = i; }
    void SetLeaf( BOOL flag ) { leaf_flag = flag; }
    void SetMark( BOOL flag ) { mark_flag = flag; }
    void SetNext( OctNode *pn ) { pnext = pn; }
    void SetPrev( OctNode *pn ) { pprev = pn; }
};

class OctQuant      // Octree color quantization
{
  private:
    int leaf_level;             // Leaf level
    int num_leaf;               // Number of leaf nodes

    // Reducible node list pointers
    OctNode *prnl[O_MaxDepth +1];

  protected:
    int height;                 // Bitmap height
    int width;                  // Bitmap width

    OctNode *proot;             // Octree root node pointer

    BOOL InsertNode( OctNode *, OctColor &, int idx );

    OctNode *MakeNode( int level )
    {
      BOOL leaf;    // Leaf node flag

      leaf = (level >= leaf_level) ? TRUE : FALSE;

      if (leaf == TRUE)
        num_leaf++;

      return new OctNode(level, leaf );
    }

    OctNode *GetReducible();

    // Quantize color
    int QuantizeColor( OctNode *pn, OctColor &c )
    {
      OctNode *pc;
      int c_index;      // Child node pointer index

      if ((pn->IsLeaf() == TRUE) || pn->GetLevel() ==
          leaf_level)
        return pn->GetIndex();
      else
      {
        c_index = pn->FindChild(c);

        pc=pn->GetChild(c_index);
        if(!pc)
        {
          return pn->GetIndex(); //found no child, we use this node
        }
        return QuantizeColor(pn->GetChild(c_index), c);
      }
    }

    void DeleteNode( OctNode * );
    void DeleteTree() { DeleteNode(proot); }
    void FillPalette( OctNode *, palettetype &palette, int *index);

    // Add node to reducible list
    void MakeReducible( OctNode *pn )
    {
      int level;        // Node level
      OctNode *phead;   // List head node
      
      level = pn->GetLevel();
      phead = prnl[level];
      pn->SetNext(phead);
      if (phead != NULL)
        phead->SetPrev(pn);
      prnl[level] = pn;

      pn->SetMark(TRUE);
    }
    
    void ReduceTree();

  public:
    OctQuant()
    {
      int i;    // Loop index

      width = height = 0;
      num_leaf = 0;
      leaf_level = O_MaxDepth + 1;

      // Clear the reducible node list pointers
      for (i = 0; i < leaf_level; i++)
        prnl[i] = NULL;

      proot = NULL;
    }

    //use this for truecolor rawbits
    int QuantizeAllColors(LPBYTE pFrameData, const DWORD *pRawBits, CPoint nDimension, // int nPixelNum,
      palettetype &palette);
    //use this for transforming a palette
    int QuantizeAllColors(LPBYTE pFrameData, LPBYTE pRawBits, CPoint nDimension, // int nPixelNum,
      palettetype &oldpalette, palettetype &palette);
    int AddPalette(palettetype &palette);
    //use this for converting rawbits to indices
    BOOL BuildTree(const DWORD *pRawBits, LPBYTE pFrameData, 
      CPoint nDimension, palettetype &palette);
};

#endif

