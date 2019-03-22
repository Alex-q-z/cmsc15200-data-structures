#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct rgb {
  int r;
  int g;
  int b;
};

int main(){
  // header
  fprintf(stdout,"P3\n");

  // 0:5 header "P152v1"
  for(int i=0; i<6; i++)
    getchar();

  // 6:7 reserved
  for(int i=0; i<2; i++)
    getchar();

  // 8:11 date
  for(int i=0; i<4; i++)
    getchar();

  // 12:13 time
  for(int i=0; i<2; i++)
    getchar();

  // 14:15 width
  int w1 = getchar();
  int w2 = getchar();
  int w = (w1 << 8) + w2;

  // 16:17 height
  int h1 = getchar();
  int h2 = getchar();
  int h = (h1 << 8) + h2;
  fprintf(stdout,"%d %d\n255\n",w,h);
  int num_pixels = w * h;

  // 18:21 color table sizeof
  int cts1 = getchar();
  int cts2 = getchar();
  int cts3 = getchar();
  int cts4 = getchar();
  int cts = (cts1 << 24) + (cts2 << 16) + (cts3 << 8) + cts4;

  // 22
  int byte22 = getchar();
  int ifgray = (byte22 >> 6) & 1;
  fprintf(stderr,"ifgray is: %d\n",ifgray);
  int rflag = (byte22 >> 7) & 1;
  fprintf(stderr,"rflag is: %d\n",rflag);
  int ctflag = (byte22 >> 5) & 1;
  fprintf(stderr,"ctflag is: %d\n",ctflag);

  // 23:25 3 bytes reserved
  for(int i=0; i<3; i++)
    getchar();

  // message string
  int acc = 0;
  char curr_mess = 1;
  while(curr_mess != 0){
    curr_mess = getchar();
    acc++;
  }
  fprintf(stderr,"Length of the message is: %d\n",acc-1);

  // color table content
  struct rgb ct[cts];
  if(!ctflag){
    getchar();
    getchar();
    getchar();
    getchar();
    // four zero bytes
  }
  else{
    for(int i=0; i<cts; i++){
      ct[i].r = getchar();
      ct[i].g = getchar();
      ct[i].b = getchar();
    }
  }

  // number of runs & run lengths & color pixels
  if(rflag){
    // use run-length encoding
    // color image
    int num_run1 = getchar();
    int num_run2 = getchar();
    int num_run3 = getchar();
    int num_run4 = getchar();
    int num_runs = (num_run1 << 24) + (num_run2 << 16) +
                    (num_run3 << 8) + num_run4;

    // store the run lengths in order in runs[]
    int runs[num_runs];
    for(int i=0; i<num_runs; i++){
      int r1 = getchar();
      int r2 = getchar();
      int r3 = getchar();
      int r4 = getchar();
      runs[i] = (r1 << 24) + (r2 << 16) + (r3 << 8) + r4;
    }

    // store the colors / indices
    struct rgb colors[num_runs];
    int indices[num_runs];

    if(ctflag){
      // use color table
      // encoded indices

      // scan and store the indices
      for(int i=0; i<num_runs; i++){
        indices[i] = getchar();
      }

      // convert indices to struct rgbs and store in colors[]
      for(int i=0; i<num_runs; i++){
        colors[i] = ct[indices[i]];
      }
    }

    else if(!ifgray){
      // do not use color table
      // color image
      // encoded rgbs
      for(int i=0; i<num_runs; i++){
        colors[i].r = getchar();
        colors[i].g = getchar();
        colors[i].b = getchar();
      }

    }

    else{
      // do not use color table
      // gray image
      // encoded r(g/b)s
      for(int i=0; i<num_runs; i++){
        int curr_gray = getchar();
        colors[i].r = curr_gray;
        colors[i].g = curr_gray;
        colors[i].b = curr_gray;
      }
    }

    // print the color pixels in colors[]
    for(int i=0; i<num_runs; i++){
      for(int j=0; j<runs[i]; j++){
        fprintf(stdout,"%d %d %d\n",colors[i].r,colors[i].g,colors[i].b);
      }
    }
  }

  else{
    // do not use run-length encoding
    // raw bytes / raw indices
    if(ifgray){
      // do not use color table
      // gray image
      for(int i=0; i<num_pixels; i++){
        int curr_gray = getchar();
        fprintf(stdout,"%d %d %d\n",curr_gray,curr_gray,curr_gray);
      }
    }
    else if(ctflag){
      // use color table
      // color image
      for(int i=0; i<num_pixels; i++){
        int curr_index = getchar();
        fprintf(stdout,"%d %d %d\n",
                ct[curr_index].r,ct[curr_index].g,ct[curr_index].b);
      }
    }
    else{
      // do not use color table
      // color image
      for(int i=0; i<num_pixels; i++){
        int r = getchar();
        int g = getchar();
        int b = getchar();
        fprintf(stdout,"%d %d %d\n",r,g,b);
      }
    }

  }

}
