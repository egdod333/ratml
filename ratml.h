#ifndef RATML
#define RATML :3
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#define INT_TYPE 0
#define STR_TYPE 1
typedef uint8_t ptype;//a primitive type. see macros.
/* int - a 32 bit integer. in a file, this is represented as a big-endian decimal number in arabic numerals
 * str - a string of characters. in a file, this is represented as an int(which must be the length of the string), then any character, usually ':', then the string itself. there is no support for wide characters.*/
typedef void* rmlblck;//a pointer to a location with a list of bytes representing a block. used in conjunction with a defined block (see filltype() & blcktype)
typedef void* rmlitem;//a pointer to an item in a composite type. usually a primitive
typedef void* rmlcomp;//a pointer to a location of a composite type. used with a defined composite type (see filltype() & comptype)
typedef enum stroption {RML_STR,C_STR} stroption;
typedef struct comptype{
  uint32_t name;
  uint8_t length;
  ptype* types;
} comptype;//defines a composite type comprised of a list of primitive types and a 4-byte "name".
typedef struct blcktype{
  uint32_t name;
  uint8_t length;
  comptype** types;
} blcktype;//defines a list of pointers to composite types and a 4-byte "name".
typedef struct rmltype {
  uint32_t name;
  uint8_t ctypecnt;
  comptype* ctypes;
  uint8_t btypecnt;
  blcktype* btypes;
} rmltype;//defines a list of composite types and blocks referencing them. honestly forgot why it has a name.
typedef struct rmldata {
  rmltype* t;//just one pointer
  uint32_t l;
  uint8_t* d;//list of bytes
} rmldata;//defines a pointer to a type and a data representing an object of that type.
rmltype filltype(FILE* file);
/* Populates a type struct from a file*/
rmldata filldata(rmltype* t,FILE* file,stroption st);
/* Given a type (see filltype), populates a data struct from a 
 * file.*/
rmlitem get(rmldata* t,const char* field,uint8_t ind,stroption st);
/*documentation goes here*/
char* asstr(rmlitem s,stroption st);
/*interpret an item as a string. allocates new memory for it*/
uint32_t asint(rmlitem i);
/*interpret an item as an integer*/
void dump(rmldata* d);
/* Returns an item from data with an index of a composite type 
 * and a cstring structured the same way as in compfromdata()*/
int ratml_init();
#endif