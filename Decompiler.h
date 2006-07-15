// Decompiler.h: interface for the Decompiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECOMPILER_H__7F9D8A7C_C93D_4D5A_A8BD_D3E0B454F79B__INCLUDED_)
#define AFX_DECOMPILER_H__7F9D8A7C_C93D_4D5A_A8BD_D3E0B454F79B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define READ_BUFFERLENGTH 4096
#define NEGATE_TRIGGER 1

#define MAX_OBJECT_FIELDS	10
#define MAX_NESTING		5

class Object {
public:
	Object()
	{
		objectName[0] = 0;

		memset( objectFields, 0, MAX_OBJECT_FIELDS * sizeof( int ) );
		memset( objectFilters, 0, MAX_NESTING * sizeof( int ) );
		memset( objectRect, 0, 4 * sizeof( int ) );

	};
	~Object()
	{
	}
public:
	int objectFields[MAX_OBJECT_FIELDS];
	int objectFilters[MAX_NESTING];
	int objectRect[4];
	char objectName[65]; 
  bool ReadyToDie(int ObjectFieldsCount);
};

class Point {
public:
	Point(void);
	~Point(void);
	short x,y;
	Point(const Point& pnt);
	Point& operator=(const Point& pnt);
	bool operator==(const Point &pnt);
	bool operator!=(const Point &pnt);
	Point(short x, short y);
	/** if it is [-1.-1] */
	bool isempty() const;
	/** if it is [0.0] */
	bool isnull() const;
};

class Trigger {
public:
	Trigger()
	{
		objectParameter = NULL;
    memset(string0Parameter,0,sizeof(string0Parameter));
		memset(string1Parameter,0,sizeof(string1Parameter));
    flags=0;
		int0Parameter = 0;
		int1Parameter = 0;
    int2Parameter = 0;
		pointParameter.x = 0;
		pointParameter.y = 0;
	};
	~Trigger()
	{
		if (objectParameter) {
      delete objectParameter;
      objectParameter = NULL;
		}
	}
public:
	unsigned short triggerID;
	int int0Parameter;
	int flags;
	int int1Parameter;
	int int2Parameter;
	Point pointParameter;
	char string0Parameter[65];
	char string1Parameter[65];
	Object* objectParameter; 
};

class Action {
public:
	Action()
	{
		actionID = 0;
		objects[0] = NULL;
		objects[1] = NULL;
		objects[2] = NULL;
    memset(string0Parameter,0,sizeof(string0Parameter));
		memset(string1Parameter,0,sizeof(string1Parameter));
		int0Parameter = 0;
		pointParameter.x = 0;
		pointParameter.y = 0;
		int1Parameter = 0;
		int2Parameter = 0;
	}
	~Action()
	{
		for (int c = 0; c < 3; c++) {
			if (objects[c]) {
				delete objects[c];
				objects[c] = NULL;
			}
		}
	}
public:
	unsigned short actionID;
	Object* objects[3];
	int int0Parameter;
	Point pointParameter;
	int int1Parameter;
	int int2Parameter;
	char string0Parameter[65];
	char string1Parameter[65]; 
};

class Decompiler  
{
public:
	Decompiler(int arg_log);
	virtual ~Decompiler();
public:
  bool Decompile(CString filepath, CString output);
  void CloseAll();
private:
  void ReadLine(int maxlength);
  int ReadResponseBlock();
  int ReadCondition();
  CString PrintPoint(Point &pO);
  CString PrintRegion(int *region);
  CString PrintTrigger(Trigger *tR);
  int FindColon(const char *variable);
  int IsAreaUsed(compiler_data &value, int nextpar);
  int ReadTrigger();
  int ReadResponseSet();
  CString GetComment(int strref);
  CString PrintAction(Action *aC);
  int ReadResponse();
  CString PrintObject(Object *oB);
  Object* DecodeObject(int &pos);
  int ParseInt(int &pos);
  void ParseString(int &pos, char* tmp);
  void WriteString(CString tmp);
  int guess_paramcount(Action *aC);
  int guess_paramcount(Trigger *tR);

private:
  int logging;
  int flog, fhandle, fout;
  char *buffer;
  CString line;
  int num_or;
  int bpoi, valid;
  int ObjectFieldsCount;
  int MaxObjectNesting;
  bool HasAdditionalRect;
  bool HasTriggerPoint;
  int ExtraParametersCount;
};

#endif // !defined(AFX_DECOMPILER_H__7F9D8A7C_C93D_4D5A_A8BD_D3E0B454F79B__INCLUDED_)
