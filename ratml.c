#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ratml.h"
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;

ui32 blank;
ui32 end;
ui32 composite;
ui32 block;
ui32 list;

int readint(FILE* file){
  ui32 out=0;
  char a;
  ui8 pwr=0;
  fread(&a,1,1,file);
  while(48<=a&&a<58) {
    ui32 t=(a-48);
    for(ui8 i=0;i<pwr;i++){t*=10;}
    pwr++;
    out+=t;
    fread(&a,1,1,file);
  }
  return out;
}
rmltype filltype(FILE* file){
  ui32 t=0;
  rmltype out={};
  fread(&out.name,1,4,file);
  fseek(file,1,SEEK_CUR);
  while(t!=end){
    if((fread(&t,1,4,file)!=4)||(t==end)){break;}
    if(t==composite){
      fseek(file,1,SEEK_CUR);
      ui8 c=readint(file);
      out.ctypecnt=c;
      out.ctypes=(comptype*)malloc(c*sizeof(comptype));
      for(int i=0;i<c;i++){
        comptype j={};
        do{fread(&j.name,1,4,file);}while(j.name==blank);
        fseek(file,1,SEEK_CUR);
        j.length=readint(file);
        j.types=(ptype*)malloc(sizeof(ptype)*j.length);
        char k;
        for(int l=0;l<j.length;l++){
          fread(&k,1,1,file);
          switch(k){
            case 'i':
            j.types[l]=INT_TYPE;
            break;
            case 's':
            j.types[l]=STR_TYPE;
            break;
          }
          fseek(file,1,SEEK_CUR);
        }
        out.ctypes[i]=j;
      }
    }else if(t==block){
      fseek(file,1,SEEK_CUR);
      ui8 c=readint(file);
      out.btypecnt=c;
      out.btypes=(blcktype*)malloc(c*sizeof(blcktype));
      for(int i=0;i<c;i++){
        blcktype j={};
        do{fread(&j.name,1,4,file);}while(j.name==blank);
        fseek(file,1,SEEK_CUR);
        j.length=readint(file);
        j.types=(comptype**)malloc(j.length*sizeof(comptype**));
        ui32 name;
        for(int k=0;k<j.length;k++){
          do{fread(&name,1,4,file);}while(name==blank);
          ui8 l=0;
          while(l<out.ctypecnt&&out.ctypes[l].name!=name){l++;}
          j.types[k]=&out.ctypes[l];
          fseek(file,1,SEEK_CUR);
        }
        out.btypes[i]=j;
      }
    }else{

    }
  }
  return out;
}
rmldata filldata(rmltype* t,FILE* file,stroption st){
  rmldata out={t,0,NULL};
  ui32 a=0;
  ui32 s=0;
  ui32 n=0;
  while(a!=end){
    do{fread(&a,1,4,file);}while(a==blank);
    for(ui8 i=0;i<max(t->ctypecnt,t->btypecnt);i++){
      if(i<t->ctypecnt&&a==t->ctypes[i].name){
        for(ui8 j=0;j<t->ctypes[i].length;j++){
          fseek(file,1,SEEK_CUR);
          switch(t->ctypes[i].types[j]){
            case INT_TYPE:
            s+=4;
            readint(file);
            fseek(file,-1,SEEK_CUR);
            break;
            case STR_TYPE:{
              ui8 p=readint(file);
              s+=p+1;
              fseek(file,p,SEEK_CUR);
            }
            break;
            default:;
          }
        }
        break;
      }if(i<t->btypecnt){
        if(a==t->btypes[i].name){
          s+=8;
          n+=8;
        }
      }
    }
    fseek(file,1,SEEK_CUR);
  }
  out.d=(ui8*)malloc(s);
  out.l=n/8;
  fseek(file,0,SEEK_SET);
  s=n;
  n=0;
  a=0;
  do{
    do{fread(&a,1,4,file);}while(a==blank);
    for(ui8 i=0;i<max(t->ctypecnt,t->btypecnt);i++){
      if(i<t->ctypecnt&&a==t->ctypes[i].name){
        for(ui8 j=0;j<t->ctypes[i].length;j++){
          fseek(file,1,SEEK_CUR);
          switch(t->ctypes[i].types[j]){
            case INT_TYPE:
            out.d[s]=readint(file);
            s+=4;
            fseek(file,-1,SEEK_CUR);
            break;
            case STR_TYPE:{
              ui8 p=readint(file);
              switch(st){
                case RML_STR:
                  out.d[s]=p;
                  fread(&out.d[s+1],1,p,file);
                  s+=p+1;
                  break;
                case C_STR:
                  out.d[s+p]='\0';
                  fread(&out.d[s],1,p,file);
                  s+=p+1;
                  break;
              }
              break;
            }
            break;
            default:;
          }
        }
        break;
      }else if(i<t->btypecnt&&a==t->btypes[i].name){
        ((ui32*)out.d)[n/4]=a;
        ((ui32*)out.d)[n/4+1]=s;
        n+=8;
      }
    }
    fseek(file,1,SEEK_CUR);
  }while(a!=end);
  return out;
}
rmlitem get(rmldata* d,const char* i,ui8 ind,stroption st){
  ui32 blck=*(ui32*)i;
  ui32 comp=*(ui32*)&i[5];
  blcktype* btype;
  comptype* ctype;
  for(ui8 j=0;j<d->t->btypecnt;j++){
    if(d->t->btypes[j].name==blck){
      btype=&d->t->btypes[j];
      break;
    }
  }
  for(ui8 j=0;j<btype->length;j++){
    if(btype->types[j]->name==comp){
      ctype=btype->types[j];
      break;
    }
  }
  for(ui8 j=0;j<d->l;j++){
    if(*(ui32*)&d->d[8*j]==blck){
      ui8 b=*(ui32*)&d->d[8*j+4];
      for(ui8 k=0;k<btype->length;k++){
        for(ui8 l=0;l<btype->types[k]->length;l++){
          if((l==ind)&&(btype->types[k]->name==comp)){
            return &(d->d[b]);
          }
          switch(btype->types[k]->types[l]){
            case INT_TYPE:b+=4;break;
            case STR_TYPE:{
              if(st==RML_STR){
                b+=d->d[b]+1;
              }else if(st==C_STR){
                for(;d->d[b]!='\0';b++);
              }
              break;
            }
          }
        }
      }
      printf("\n\n\nunimaginable\n");
      return NULL;
      // break;
    }else{
    }
  }
}
void dump(rmldata* d){
  rmltype* t=d->t;
  printf("types\nctypes: %u\nname | primitives\n-----+-----",t->ctypecnt);
  for(ui8 i=0;i<t->ctypecnt;i++){
    printf("\n%.4s |",&t->ctypes[i].name);
    for(ui8 j=0;j<t->ctypes[i].length;j++){
      char c='*';
      switch(d->t->ctypes[i].types[j]){
        case INT_TYPE:c='i';break;
        case STR_TYPE:c='s';break;
      }
      printf(" %c",c);
    }
  }
  printf("\n-----+-----\nbtypes: %u\nname | composites\n-----+-----",t->btypecnt);
  for(ui8 i=0;i<t->btypecnt;i++){
    printf("\n%.4s |",&t->btypes[i].name);
    for(ui8 j=0;j<t->btypes[i].length;j++){
      printf(" %.4s",&((*t->btypes[i].types)[j]));
    }
  }
  printf("\n-----+-----\n\nindices of %u blocks\nbyte loc| name: address\n--------+-----\n",d->l);
  for(ui32 i=0;i<(d->l*8);i+=8){
      printf("%03.3u-%03.3u | %.4s: %u\n",i,i+7,&d->d[i],d->d[i+4]);
  }
  printf("--------+-----\nblock contents\n--------+-----");
  for(ui8 j=0;j<(d->l);j++){
    for(ui8 k=0;k<t->btypecnt;k++){
      if(*(ui32*)&(d->d)[8*j]==t->btypes[k].name){
        ui8 i=0;
        ui8 o=d->d[8*j+4];
        printf("\n%03.3u-... | %.4s",o,&t->btypes[k].name);
        for(ui8 l=0;l<t->btypes[k].length;l++){
          for(ui8 m=0;m<(*t->btypes[k].types[l]).length;m++){
            switch((*t->btypes[k].types[l]).types[m]){
              case INT_TYPE:{
                printf("\n%03.3u-%03.3u | %u",i+o,i+o+3,*(ui32*)&d->d[o+i]);
                i+=4;
                break;
              }
              case STR_TYPE:{
                ui8 n=(d->d)[o+i];
                printf("\n%03.3u-%03.3u | %u:%.*s",i+o,i+o+n,n,n,&d->d[o+1+i]);
                i+=n+1;
                break;
              }
            }
          }
        }
        break;
      }
    }
  }
  printf("\n--------+-----\n");
}
char* asstr(rmlitem s,stroption st){
  char* out;
  if(st==RML_STR){
    ui8 l=*(ui8*)s;
    out=malloc(l+1);
    memcpy(out,&((ui8*)s)[1],l);
    out[l]='\0';
  }else if(st==C_STR){
    
  }
  return out;
}
uint32_t asint(rmlitem i){
  return *(uint32_t*)i;
}
int ratml_init() {
  blank=*(ui32*)"    ";
  end=*(ui32*)"!!!!";
  composite=*(ui32*)"comp";
  block=*(ui32*)"blck";
  list=*(ui32*)"list";
  printf("ratml\n");
  return 0;
}