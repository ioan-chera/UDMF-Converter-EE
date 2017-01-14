//
//  main.cpp
//  UDMF-Converter-EE
//
//  Created by ioan on 09.01.2017.
//  Copyright © 2017 Team Eternity. All rights reserved.
//

#include "z_zone.h"
#include "m_argv.h"
#include "m_collection.h"
#include "w_wad.h"

//
// D_AddFile
//
// Rewritten by Lee Killough
//
// killough 11/98: remove limit on number of files
// haleyjd 05/28/10: added f and baseoffset parameters for subfile support.
//
static void addFile(const char *file, PODCollection<wfileadd_t> &adds)
{
   unsigned int flags;

   wfileadd_t &add = adds.addNew();
   add.filename = file;
   add.li_namespace = lumpinfo_t::ns_global;
   add.requiredFmt = -1;

   // haleyjd 10/27/12: setup flags

   // ioanch: check if it's a directory. Do not allow "subfiles" or special
   // addflags
   struct stat sbuf;
   if(!stat(file, &sbuf) && S_ISDIR(sbuf.st_mode))
      flags = WFA_DIRECTORY_ARCHIVE | WFA_OPENFAILFATAL;
   else
      flags = WFA_ALLOWINEXACTFN | WFA_OPENFAILFATAL | WFA_ALLOWHACKS;

   add.flags = flags;
}

//
// Loads the wads from the command line into the directory
//
static void loadWads(WadDirectory &dir)
{
   int parm = M_CheckParm("-file");
   if(!parm || parm + 1 >= myargc)
   {
      fprintf(stderr, "Please provide the -file with path to wad\n");
      exit(EXIT_FAILURE);
   }

   PODCollection<wfileadd_t> adds;
   for(++parm; parm < myargc; ++parm)
   {
      if(myargv[parm][0] == '-')
         break;
      addFile(myargv[parm], adds);
   }
   adds.addNew(); // add empty entry (sentinel)

   dir.initMultipleFiles(&adds[0]);
}

//
// Entry point
//
int main(int argc, const char * argv[])
{
   myargc = argc;
   myargv = argv;
   WadDirectory dir;

   loadWads(dir);
   int parm = M_CheckParm("-map");
   if(!parm || parm + 1 >= myargc)
   {
      fprintf(stderr, "Please provide the -map with the map name\n");
      exit(EXIT_FAILURE);
   }
   

   return 0;
}
