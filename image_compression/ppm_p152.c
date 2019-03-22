#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct rgb {
  int r;
  int g;
  int b;
};

struct parameters {
  FILE *ifp;
  FILE *ofp;
  int ctflag; // if using color table
  int cts; // color table size
  int rflag; // if using run-length encoding
  char *message; // description
  int w; // width
  int h; // height
  int cd; // color depth
};

int ifgrayscale(struct rgb arr[], int num_pix){
  for(int k=0; k<num_pix; k++){
    if(arr[k].r != arr[k].g || arr[k].g != arr[k].b || arr[k].r != arr[k].b)
      return 0;
  }
  return 1;
}

struct rgb *generatect(int cts){
  struct rgb *result = malloc(cts * sizeof(struct rgb));
  int color[cts];
  for(int i=0; i<cts; i++){
    color[i] = 0xFFFFFF * i / (cts - 1);
    //fprintf(stderr,"#%d element in color array is %d\n",i,color[i]);
    //fprintf(stderr,"color is (%d,%d,%d)\n",
    //        (color[i]>>16)&0xFF,(color[i]>>8)&0xFF,color[i]&0xFF);
  }
  for(int i=0; i<cts; i++){
    result[i].r = (color[i] >> 16) & 0xFF;
    result[i].g = (color[i] >> 8) & 0xFF;
    result[i].b = color[i] & 0xFF;
  }
  return result;
}

void show_rgb(struct rgb *input, int l){
  for(int i=0; i<l; i++){
    fprintf(stderr,"Color #%d is (%d,%d,%d)\n",
            i,input[i].r,input[i].g,input[i].b);
  }
}

double sqr(double k){
  return k * k;
}

double eucdistance(struct rgb a1, struct rgb a2){
  double r1_r2 = (double) a1.r - a2.r;
  double g1_g2 = (double) a1.g - a2.g;
  double b1_b2 = (double) a1.b - a2.b;
  double d = sqrt(sqr(r1_r2) + sqr(g1_g2) + sqr(b1_b2));
  return d;
}

int optimal_index(struct rgb *ct, int l, struct rgb target){
  double min_dis = eucdistance(ct[0],target);
  int min_index = 0;
  for(int i=0; i<l; i++){
    if(eucdistance(ct[i],target) < min_dis){
      min_dis = eucdistance(ct[i],target);
      min_index = i;
    }
  }
  return min_index;
}

void print_branch(struct parameters *parms, struct rgb *arr){

  // first of all, scan and store all color pixels
  // in an array of struct rgb
  //int r, g, b;
  //int num_pixels = parms->w * parms->h;
  //struct rgb arr[num_pixels];
  //for(int i=0; i<num_pixels; i++){
  //  fscanf(parms->ifp,"%d %d %d",&r,&g,&b);
  //  arr[i].r = r;
  //  arr[i].g = g;
  //  arr[i].b = b;
  //}

  int num_pixels = parms->w * parms->h;

  // 0:5 header
  fprintf(parms->ofp,"P152v1");

  // 6:7 reserved
  putc(0,parms->ofp);
  putc(0,parms->ofp);

  // 8:11 Date
  time_t rawtime;
  struct tm *info;
  time(&rawtime);
  info = localtime( &rawtime );
  int year = info->tm_year + 1900;
  int month = info->tm_mon + 1;
  int day = info->tm_mday;
  //fprintf(stdout,"%d %d %d\n",year,month,day);
  int n = year*10000 + month*100 + day;
  char y1 = (n >> 24) & 0xFF;
  putc(y1,parms->ofp);
  char y2 = (n >> 16) & 0xFF;
  putc(y2,parms->ofp);
  char mo = (n >> 8) & 0xFF;
  putc(mo,parms->ofp);
  char da = n & 0xFF;
  putc(da,parms->ofp);

  // 12:13 Time
  int hour = info->tm_hour;
  int minutes = info->tm_min;
  int m = hour*100 + minutes;
  //printf("%d\n",m);
  char hr = (m >> 8) & 0xFF;
  putc(hr,parms->ofp);
  char minu = m & 0xFF;
  putc(minu,parms->ofp);

  // 14:15 width
  char w1 = (parms->w >> 8) & 0xFF;
  putc(w1,parms->ofp);
  char w2 = parms->w & 0xFF;
  putc(w2,parms->ofp);

  // 16:17 height
  char h1 = (parms->h >> 8) & 0xFF;
  putc(h1,parms->ofp);
  char h2 = parms->h & 0xFF;
  putc(h2,parms->ofp);

  // 18:21 color table size
  int ifgray = ifgrayscale(arr,num_pixels);
  if(ifgray){
    putc(0,parms->ofp);
    putc(0,parms->ofp);
    putc(0,parms->ofp);
    putc(0,parms->ofp);
    fprintf(stderr,"Color table is not available to grayscale images\n");
  }
  else{
    char cts1 = (parms->cts >> 24) & 0xFF;
    putc(cts1,parms->ofp);
    char cts2 = (parms->cts >> 16) & 0xFF;
    putc(cts2,parms->ofp);
    char cts3 = (parms->cts >> 8) & 0xFF;
    putc(cts3,parms->ofp);
    char cts4 = parms->cts & 0xFF;
    putc(cts4,parms->ofp);
    fprintf(stderr,"Size of color table is: %d\n",parms->cts);
  }

  // 22
  // bit 1: if run-length encoded
  // bit 2: if grayscale image
  // bit 3: if color table is used
  //fprintf(stderr,"If the image is grayscale: %d\n",ifgray);
  if(parms->ctflag == 1 && ifgray == 1)
    parms->ctflag = 0;
  int byte22 = 128 * parms->rflag + 64 * ifgray + 32 * parms->ctflag;
  putc(byte22,parms->ofp);

  // 23:25 reserved
  putc(0,parms->ofp);
  putc(0,parms->ofp);
  putc(0,parms->ofp);

  // string describing the image
  if(strcmp(parms->message,"") != 0)
    fprintf(parms->ofp,"%s",parms->message);
  putc(0,parms->ofp);

  // color table content
  // note: If an image is grayscale, do not use a color table,
  // even if a color table has been indicated.
  int *indices = malloc(num_pixels * sizeof(int)); // all pixels expressed in indices

  if(parms->ctflag == 0 || ifgray == 1){
    putc(0,parms->ofp);
    putc(0,parms->ofp);
    putc(0,parms->ofp);
    putc(0,parms->ofp);
  }
  else{ // need color table
    // 1. generate a color table
    struct rgb *ct = generatect(parms->cts);
    // Note: this func is tested and correct

    // 2. print the color table
    for(int i=0; i<parms->cts; i++){
      putc(ct[i].r,parms->ofp);
      putc(ct[i].g,parms->ofp);
      putc(ct[i].b,parms->ofp);
    }
    // 3. convert all pixels in image to optimal indices
    for(int i=0; i<num_pixels; i++){
      indices[i] = optimal_index(ct,parms->cts,arr[i]);
    }

    free(ct);

    /*
    // test of optimal_index here
    struct rgb *test_ct = generatect(4);
    show_rgb(test_ct,4);
    struct rgb test_target;
    test_target.r = 165;
    test_target.g = 185;
    test_target.b = 145;
    int test_index = optimal_index(test_ct,4,test_target);
    fprintf(stderr,"Optimal index for the test pixel is: %d\n",test_index);
    */

  }

  // color pixels: run-length encoding or raw bytes or indices
  if(parms->rflag == 1 && parms->ctflag == 0){
    // situation 1:
    // use run-length encoding
    // do not use color table
    // note: both color & gray image included

    // if passing the test: yes (preliminary)

    // 1. generate the runs and colors array
    int index = 0, curr_num = 1;
    int *runs = malloc(num_pixels * sizeof(int));
    for(int i=0; i<num_pixels; i++){
      runs[i] = 0;
    }


    // WARNING: THIS ARRAY OVERFLOWS if not using malloc
    // remember to free this after !!!!!
    struct rgb *colors = malloc(num_pixels * sizeof(struct rgb));

    for(int j=0; j<num_pixels; j++){
      if(arr[j].r != arr[j+1].r ||
         arr[j].g != arr[j+1].g ||
         arr[j].b != arr[j+1].b) {
        runs[index] = curr_num;
        colors[index].r = arr[j].r;
        colors[index].g = arr[j].g;
        colors[index].b = arr[j].b;
        index++;
        curr_num = 1;
      }else{
        curr_num++;
      }
    }

    // 2. "prints" out the result of runs and colors
    // 2.1 calculate and "prints" the number of runs
    int run_nums = 0;
    while(runs[run_nums] != 0){
      run_nums++;
    }
    fprintf(stderr,"run_nums = %d\n",run_nums);
    putc((run_nums >> 24) & 0xFF,parms->ofp);
    //fprintf(stderr,"1st byte for run_nums: %d\n",(run_nums >> 24) & 0xFF);
    putc((run_nums >> 16) & 0xFF,parms->ofp);
    //fprintf(stderr,"2nd byte for run_nums: %d\n",(run_nums >> 16) & 0xFF);
    putc((run_nums >> 8) & 0xFF,parms->ofp);
    //fprintf(stderr,"3rd byte for run_nums: %d\n",(run_nums >> 8) & 0xFF);
    putc(run_nums & 0xFF,parms->ofp);
    //fprintf(stderr,"4th byte for run_nums: %d\n",run_nums & 0xFF);
    //fprintf(stderr,"Value of run_nums: %d\n",run_nums);

    // 2.2 "prints" the run lengths in order
    for(int k=0; k<run_nums; k++){
      putc((runs[k] >> 24) & 0xFF,parms->ofp);
      putc((runs[k] >> 16) & 0xFF,parms->ofp);
      putc((runs[k] >> 8) & 0xFF,parms->ofp);
      putc(runs[k] & 0xFF,parms->ofp);
      //fprintf(stderr,"content of runs array:\n");
      //fprintf(stderr,"%d\n",runs[k]);
    }

    // 2.3 "prints" the colors in order based on ifgray
    if(ifgray == 1){
      // gray images
      for(int k=0; k<run_nums; k++){
        putc(colors[k].r,parms->ofp);
      }
    }else{
      // color images
      for(int k=0; k<run_nums; k++){
        putc(colors[k].r,parms->ofp);
        putc(colors[k].g,parms->ofp);
        putc(colors[k].b,parms->ofp);
        //fprintf(stderr,"content of colors array:\n");
        //fprintf(stderr,"%d %d %d\n",colors[k].r,colors[k].g,colors[k].b);
      }
    }

    free(runs);
    free(colors);
  }

  else if(parms->rflag == 0 && parms->ctflag == 0){
    // situation 2: print raw bytes
    // do not use run-length encoding
    // do not use color table
    // both color & gray image included

    // if passing the test: yes (preliminary)

    if(ifgray == 1){
      // gray images
      for(int i=0; i<num_pixels; i++){
        putc(arr[i].r,parms->ofp);
      }
    }else{
      // color images
      for(int i=0; i<num_pixels; i++){
        putc(arr[i].r,parms->ofp);
        putc(arr[i].g,parms->ofp);
        putc(arr[i].b,parms->ofp);
        //fprintf(stderr,"content of colors array:\n");
        //fprintf(stderr,"%d %d %d\n",colors[k].r,colors[k].g,colors[k].b);
      }
    }

  }

  else if(parms->rflag == 0 && parms->ctflag == 1 && ifgray == 0){
    // situation 3
    // do not use run-length encoding
    // use color table
    // raw indices

    // if passing the test: yes (preliminary)
    //int countt = 0;
    for(int i=0; i<num_pixels; i++){
      putc(indices[i],parms->ofp);
      //countt++;
    }
    //fprintf(stderr,"The final num of indices printed is %d\n",countt);
  }

  else if(parms->rflag == 1 && parms->ctflag == 1 && ifgray == 0){
    // situation 4
    // use run-length encoding
    // use color table
    // note: the essence is run-length encoding of indices

    // if passing the test: yes (preliminary)

    // 1. generate the encoded_runs and encoded_indices array
    int index = 0, curr_num = 1;
    int *encoded_runs = malloc(num_pixels * sizeof(int));
    for(int i=0; i<num_pixels; i++){
      encoded_runs[i] = 0;
    }
    int *encoded_indices = malloc(num_pixels * sizeof(int));
    for(int j=0; j<num_pixels; j++){
      if(indices[j] != indices[j+1]) {
        encoded_runs[index] = curr_num;
        encoded_indices[index] = indices[j];
        index++;
        curr_num = 1;
      }else{
        curr_num++;
      }
    }

    // 2. "prints" out the result of encoded_runs and encoded_indices
    // 2.1 calculate and "prints" the number of runs
    int run_nums = 0;
    while(encoded_runs[run_nums] != 0){
      run_nums++;
    }
    putc((run_nums >> 24) & 0xFF,parms->ofp);
    //fprintf(stderr,"1st byte for run_nums: %d\n",(run_nums >> 24) & 0xFF);
    putc((run_nums >> 16) & 0xFF,parms->ofp);
    //fprintf(stderr,"2nd byte for run_nums: %d\n",(run_nums >> 16) & 0xFF);
    putc((run_nums >> 8) & 0xFF,parms->ofp);
    //fprintf(stderr,"3rd byte for run_nums: %d\n",(run_nums >> 8) & 0xFF);
    putc(run_nums & 0xFF,parms->ofp);
    //fprintf(stderr,"4th byte for run_nums: %d\n",run_nums & 0xFF);
    //fprintf(stderr,"Value of run_nums: %d\n",run_nums);

    // 2.2 "prints" the run lengths (stored in encoded_runs) in order
    for(int k=0; k<run_nums; k++){
      putc((encoded_runs[k] >> 24) & 0xFF,parms->ofp);
      putc((encoded_runs[k] >> 16) & 0xFF,parms->ofp);
      putc((encoded_runs[k] >> 8) & 0xFF,parms->ofp);
      putc(encoded_runs[k] & 0xFF,parms->ofp);
      //fprintf(stderr,"content of runs array:\n");
      //fprintf(stderr,"%d\n",runs[k]);
    }

    // 2.3 "prints" the indices (stored in encoded_indices) in order
    // color images
    for(int k=0; k<run_nums; k++){
      putc(encoded_indices[k],parms->ofp);
      //fprintf(stderr,"content of colors array:\n");
      //fprintf(stderr,"%d %d %d\n",colors[k].r,colors[k].g,colors[k].b);
    }

    free(encoded_runs);
    free(encoded_indices);
  } // end of major situations

  else{
    fprintf(stderr,"illegal combination\n");
  }

  free(indices);

}

int main(int argc, char *argv[]){
  struct parameters parms;
  parms.ifp = stdin;
  parms.ofp = stdout;
  char *inf = NULL; // read from
  char *outf = NULL; // write to
  parms.ctflag = 0; // if using color table
  parms.cts = 0; // color table size
  parms.rflag = 0; // if using run-length encoding

  parms.message = ""; // description
  // read in and store all command line operations
  for(int i=1; i<argc; i++){
    if(strcmp(argv[i],"-r") == 0)
      parms.rflag = 1;
    else if(strcmp(argv[i],"-i") == 0){
      inf = argv[i+1];
      parms.ifp = fopen(inf,"r"); // redirect ifp
      i++;
    }
    else if(strcmp(argv[i],"-o") == 0){
      outf = argv[i+1];
      parms.ofp = fopen(outf,"w"); // redirect ofp
      i++;
    }
    else if(strcmp(argv[i],"-t") == 0){
      parms.ctflag = 1;
      parms.cts = atoi(argv[i+1]);
      if(parms.cts == 0){
        fprintf(stderr,"Size of the color table should not be 0.\n");
        exit(1);
      }
      i++;
    }
    else if(i == 1){
      parms.message = argv[i];
    }
    else{
      fprintf(stderr,"Unknown command.\n");
      exit(1);
    }
  }
  //fprintf(stderr,"r is %d, i is %s, o is %s, t is %d\n",
  //	  rflag, inf, outf, cts);

  // decide if the ppm file is P3 or P6
  // scan and store w, h, cd
  char buffer[16] = {0};
  fscanf(parms.ifp,"%s",buffer);
  fscanf(parms.ifp,"%d %d %d",&parms.w,&parms.h,&parms.cd);

  // scan pixles and switch to print_branch
  if(strcmp("P3",buffer) == 0){
    // first of all, scan and store all color pixels
    // in an array of struct rgb
    int r, g, b;
    int num_pixels = parms.w * parms.h;
    struct rgb arr[num_pixels];
    for(int i=0; i<num_pixels; i++){
      fscanf(parms.ifp,"%d %d %d",&r,&g,&b);
      arr[i].r = r;
      arr[i].g = g;
      arr[i].b = b;
    }
    print_branch(&parms,arr);
  }
  else if(strcmp("P6",buffer) == 0){
    // first of all, scan and store all color pixels
    // in an array of struct rgb
    getc(parms.ifp); // get rid of the additional blank
    int num_pixels = parms.w * parms.h;
    struct rgb arr[num_pixels];
    for(int i=0; i<num_pixels; i++){
      arr[i].r = getc(parms.ifp);
      arr[i].g = getc(parms.ifp);
      arr[i].b = getc(parms.ifp);
    }
    print_branch(&parms,arr);
  }
  else{
    fprintf(stderr,"the ppm file should begin with 'P3' or 'P6'");
    exit(1);
  }

  fclose(parms.ifp);
  return 0;
}
