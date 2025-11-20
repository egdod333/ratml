#include <ratml.h>
#include <stdlib.h>
int main(){
  ratml_init();
  // printf("%u,%u\n",RML_STR,C_STR);
  rmltype type=filltype(fopen("./example/type","r"));//to use ratml you first define a 'type', similar to a c struct but it's managed at runtime
  puts("got type!");
  rmldata data=filldata(&type,fopen("./example/data","r"),C_STR);//then, you can fill it in with a different file, like initializing a c struct.
  puts("got data!");
  freedata(&data);
  puts("all gone now!");
  return 0;
}