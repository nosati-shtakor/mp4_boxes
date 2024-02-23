#include <stdio.h>
#include "../mp4box.h"


void usage(const char*const* argv)
{
  printf("Usage: %s file.mp4 [-png]\n"
         "       -png will create png files (optional parameter)\n",
         argv[0]);
}


int main(int argc, char** argv)
{
  if(argc < 2 || argc > 3)
  {
    usage(argv);
    return 0;
  }

  Mp4Box mp4_box;
  const bool dump_png(argc == 3 ? strcmp(argv[2], "-png") == 0 : false);
  mp4_box.dumpBoxes(argv[1], dump_png);
}