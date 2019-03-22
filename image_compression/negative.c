#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  char buffer[16] = {0};
  int w, h, d, r, g, b;
  fscanf(stdin,"%s",buffer);
  fscanf(stdin,"%d %d %d",&w,&h,&d);
  if(strcmp(buffer,"P3") == 0){
    fprintf(stdout,"P3\n");
    fprintf(stdout,"%d %d\n%d\n",w,h,d);
    for(int i=0; i<w*h; i++){
      fscanf(stdin,"%d %d %d",&r,&g,&b);
      r = 255 - r;
      g = 255 - g;
      b = 255 - b;
      fprintf(stdout,"%d %d %d\n",r,g,b);
    }
  }
  else if(strcmp(buffer,"P6") == 0){
    fprintf(stdout,"P6\n");
    fprintf(stdout,"%d %d\n%d\n",w,h,d);
    getchar(); // get rid of the additional blank
    for(int i=0; i<w*h; i++){
      r = getchar();
      r = 255 - r;
      g = getchar();
      g = 255 - g;
      b = getchar();
      b = 255 - b;
      putchar(r);
      putchar(g);
      putchar(b);
    }
  }
  else{
    fprintf(stderr,"the ppm file should begin with P3 or P6\n");
    exit(1);
  }
}
