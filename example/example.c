#include <ratml.h>
struct rmlstrstruct {
  //blk1
  //cmp1
  uint8_t a;
  char b[4];
  uint8_t c;
  char d[10];
  uint32_t e;
  uint32_t f;
  //cmp2
  uint8_t g;
  char h[7];
  //blk2
  //cmp2
  uint8_t i;
  char j[14];
};
struct cstrstruct {
  //blk1
  //cmp1
  char b[5];
  char d[11];
  uint32_t e;
  uint32_t f;
  //cmp2
  char h[8];
  //blk2
  //cmp2
  char j[15];
};
int main(){
  ratml_init();
  rmltype type=filltype(fopen("./example/type","r"));//to use ratml you first define a 'type', similar to a c struct but it's managed at runtime
  rmldata data=filldata(&type,fopen("./example/data","r"),RML_STR);//then, you can fill it in with a different file, like initializing a c struct.
  rmlitem item=get(&data,"blk1.cmp1",0,RML_STR);//you can get items out of data because inside of an rmldata thing is a reference to its type
  puts(asstr(item,RML_STR));//there's also helper methods for the formatting of ratml items, because especially the strings are a little bit silly
  struct rmlstrstruct tostruct=*(struct rmlstrstruct*)&data.d[data.l*8];//if you know what your data looks like (INCLUDING STRING SIZES) you can even define a struct to map it to!
  printf("%.*s %u.%u\n",tostruct.c,tostruct.d,tostruct.e,tostruct.f);//however, if you take a look at the struct, the strings are all weird
  data=filldata(&type,fopen("./example/data","r"),C_STR);//to use them as cstrings instead of ratml strings you can use the C_STR option, which stores them differently!
  struct cstrstruct tocstrstruct=*(struct cstrstruct*)&data.d[data.l*8];
  printf("%s %s\n",tocstrstruct.h,tocstrstruct.j);//then, you can treat them like normal! they don't like to get changed, though, so watch out and use asstr() if you want to get a copy to work with
  return 0;
}