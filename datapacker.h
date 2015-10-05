#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

// macros for packing floats and doubles:
#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/*
** pack754() -- pack a floating point number into IEEE-754 format
*/ 
unsigned long long int pack754(long double, unsigned, unsigned);
/*
** unpack754() -- unpack a floating point number from IEEE-754 format
*/ 
long double unpack754(unsigned long long int, unsigned, unsigned);

/*
** packi16() -- store a 16-bit int into a char buffer (like htons())
*/ 
void packi16(unsigned char*, unsigned int);
/*
** packi32() -- store a 32-bit int into a char buffer (like htonl())
*/ 
void packi32(unsigned char*, unsigned long int);
/*
** packi64() -- store a 64-bit int into a char buffer (like htonl())
*/ 
void packi64(unsigned char*, unsigned long long int);
/*
** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
*/ 
int unpacki16(unsigned char*);
/*
** unpacku16() -- unpack a 16-bit unsigned from a char buffer (like ntohs())
*/ 
unsigned int unpacku16(unsigned char*);
/*
** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
*/ 
long int unpacki32(unsigned char*);
/*
** unpacku32() -- unpack a 32-bit unsigned from a char buffer (like ntohl())
*/ 
unsigned long int unpacku32(unsigned char*);
/*
** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
*/ 
long long int unpacki64(unsigned char*);
/*
** unpacku64() -- unpack a 64-bit unsigned from a char buffer (like ntohl())
*/ 
unsigned long long int unpacku64(unsigned char*);
/*
** pack() -- store data dictated by the format string in the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C         
**     16 |   h        H         f
**     32 |   l        L         d
**     64 |   q        Q         g
**      - |                               s
**
**  (16-bit unsigned length is automatically prepended to strings)
*/ 

unsigned int pack(unsigned char*, char*, ...);

void unpack(unsigned char*, char*, ...);

