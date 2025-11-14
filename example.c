#include <ratml.h>
struct data {
  char a[5];
  char b[11];
  int c;
  int d;
  int e;
  char f[8];
  char g[8];
  char h[5];
  int i;
  int j;
  int k;
  char l[6];
  int m;
  char n[26];
  char o[4];
  int p;
};
int main(){
  ratml_init();
  // printf("%u,%u\n",RML_STR,C_STR);
  rmltype type=filltype(fopen("./example/type","r"));//to use ratml you first define a 'type', similar to a c struct but it's managed at runtime
  puts("got type!");
  rmldata data=filldata(&type,fopen("./example/data","r"),C_STR);//then, you can fill it in with a different file, like initializing a c struct.
  puts("got data!");
  struct data st=*(struct data*)&(((char*)data.d)[24]);
  printf("%s %s %u.%u.%u\n%s %s %s %s\n",st.a,st.b,st.c,st.d,st.e,get(&data,"blk2.0.cmp3.0.cmp1.0"),st.h,get(&data,"blk2.0.cmp2.1"),st.f);
  return 0;
}