#ifndef _PDATATYPE_H_
#define _PDATATYPE_H_

typedef unsigned char       uint8;
typedef char                int8;
typedef unsigned short      uint16;
typedef short               int16;
typedef unsigned int        uint32;  // 适用于ILP32、LP64、LLP64
typedef int                 int32;   // 适用于ILP32、LP64、LLP64  
typedef float               float32;
typedef double              double64;
typedef unsigned short      CRC_t;
typedef unsigned long long  uint64;
typedef long long           int64;
typedef char                strnum;


#define  BOOL8 char

typedef enum tag_emDType
{
    DTYPE_UINT8     = 1,
    DTYPE_INT8      = 2,
    DTYPE_UINT16    = 3,
    DTYPE_INT16     = 4,
    DTYPE_UINT32    = 5,
    DTYPE_INT32     = 6,
    DTYPE_FLOAT     = 7, 
    DTYPE_DOUBLE    = 8,
    DTYPE_CHAR      = 9,
    DTYPE_CRC       = 10,
    DTYPE_UINT64    = 11,
    DTYPE_INT64     = 12,
    DTYPE_STRNUM    = 13
}emDType;


#if !defined(VOID)
	typedef void VOID;
#endif
#ifndef FALSE
#define	FALSE				0x00
#define	TRUE				0x01
#endif

#ifndef  INVALID
#define  INVALID      -100000
#endif

#ifndef PI
#define PI              acos(-1.0)
#endif

#define dmax(a,b)    (((a) > (b)) ? (a) : (b))
#define dmin(a,b)    (((a) < (b)) ? (a) : (b))

#define D_EPS 1e-6 // 最小精度

void nameOfDataType(char *pcdataType, int dataType);
int sizeOfDataType(int dataType);
void valueStr(char *valuestr, double value, int precision, BOOL8 usePrecision);


#endif // _PDATATYPE_H_

