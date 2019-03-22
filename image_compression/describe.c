#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  fprintf(stdout,"Description of the p152 file\n");

  // 0:5 
  for(int i=0; i<6; i++)
    getchar();

  // 6:7 reserved
  for(int i=0; i<2; i++)
    getchar();

  // 8:11 date
  int d1 = getchar();
  int d2 = getchar();
  int d3 = getchar();
  int d4 = getchar();
  int date = (d1 << 24) + (d2 << 16) + (d3 << 8) + d4;
  int year = date / 10000;
  int month = (date - year * 10000) / 100;
  int day = date - year * 10000 - month * 100;
  fprintf(stdout,"Date formed: %d/",year);
  if(month < 10)
    fprintf(stdout,"0%d/",month);
  else
    fprintf(stdout,"%d/",month);
  if(day < 10)
    fprintf(stdout,"0%d\n",day);
  else
    fprintf(stdout,"%d\n",day);
  //fprintf(stdout,"Date formed(YYYYMMDD): %d\n",date);

  // 12:13 time
  int t1 = getchar();
  int t2 = getchar();
  int time = (t1 << 8) + t2;
  int hour = time / 100;
  int minutes = time - hour * 100;
  if(hour < 10)
    fprintf(stdout,"Time formed: 0%d:",hour);
  else
    fprintf(stdout,"Time formed: %d:",hour);
  if(minutes < 10)
    fprintf(stdout,"0%d\n",minutes);
  else
    fprintf(stdout,"%d\n",minutes);
  //fprintf(stdout,"Time formed(HHMM): %d\n",time);

  // 14:15 width
  int w1 = getchar();
  int w2 = getchar();
  int w = (w1 << 8) + w2;
  fprintf(stdout,"Width of the image: %d\n",w);

  // 16:17 height
  int h1 = getchar();
  int h2 = getchar();
  int h = (h1 << 8) + h2;
  fprintf(stdout,"Height of the image: %d\n",h);

  // 18:21 size of color table
  int cts1 = getchar();
  int cts2 = getchar();
  int cts3 = getchar();
  int cts4 = getchar();
  int cts = (cts1 << 24) + (cts2 << 16) + (cts3 << 8) + cts4;

  // 22
  int byte22 = getchar();
  int ifrunlength = (byte22 >> 7) &1;
  int ifgray = (byte22 >> 6) & 1;
  int ifcolortable = (byte22 >> 5) & 1;

  // 23:25 3 bytes reserved
  for(int i=0; i<3; i++)
    getchar();

  // message
  int acc = 0;
  char message[128];
  char curr_mess = 1;
  while(curr_mess != 0){
    curr_mess = getchar();
    message[acc] = curr_mess;
    acc++;
  }
  if(strcmp(message,"") == 0){
    fprintf(stdout,"The file does not contain any string description.\n");
  }
  else
    fprintf(stdout,"String description: %s\n",message);

  fprintf(stdout,"whether or not the image uses run-length encoding: ");
  if(ifrunlength)
    fprintf(stdout,"yes\n");
  else
    fprintf(stdout,"no\n");

  fprintf(stdout,"whether or not the image is grayscale: ");
  if(ifgray)
    fprintf(stdout,"yes\n");
  else
    fprintf(stdout,"no\n");

  fprintf(stdout,"whether or not the image uses a color table: ");
  if(ifcolortable){
    fprintf(stdout,"yes\n");
    fprintf(stdout,"the size of the color table: %d\n",cts);
  }
  else 
    fprintf(stdout,"no\n");

  return 0;
}
