//
//  main.cpp
//  UDMF-Converter-EE
//
//  Created by ioan on 09.01.2017.
//  Copyright © 2017 Team Eternity. All rights reserved.
//

#include "z_zone.h"
#include "z_auto.h"
#include "m_argv.h"
#include "m_binary.h"
#include "m_collection.h"
#include "m_qstr.h"
#include "w_wad.h"

// Lump order in a map WAD: each map needs a couple of lumps
// to provide a complete scene geometry description.
enum
{
   ML_LABEL,             // A separator, name, ExMx or MAPxx
   ML_THINGS,            // Monsters, items..
   ML_LINEDEFS,          // LineDefs, from editing
   ML_SIDEDEFS,          // SideDefs, from editing
   ML_VERTEXES,          // Vertices, edited and BSP splits generated
   ML_SEGS,              // LineSegs, from LineDefs split by BSP
   ML_SSECTORS,          // SubSectors, list of LineSegs
   ML_NODES,             // BSP nodes
   ML_SECTORS,           // Sectors, from editing
   ML_REJECT,            // LUT, sector-sector visibility
   ML_BLOCKMAP,          // LUT, motion clipping, walls/grid element
   ML_BEHAVIOR,          // haleyjd 10/03/05: behavior, used to id hexen maps

   // PSX
   ML_LEAFS = ML_BEHAVIOR, // haleyjd 12/12/13: for identifying console map formats

   // Doom 64
   ML_LIGHTS,
   ML_MACROS
};

// haleyjd 10/03/05: let P_CheckLevel determine the map format
enum
{
   LEVEL_FORMAT_INVALID,
   LEVEL_FORMAT_DOOM,
   LEVEL_FORMAT_HEXEN,
   LEVEL_FORMAT_PSX,
   LEVEL_FORMAT_DOOM64,
   // LEVEL_FORMAT_UDMF_[NAMESPACE] is the format for UDMF namespaces that need their own definition
   LEVEL_FORMAT_UDMF_ETERNITY,
};

//
// Map lumps table
//
// For Doom- and Hexen-format maps.
//
static const char *levellumps[] =
{
   "label",    // ML_LABEL,    A separator, name, ExMx or MAPxx
   "THINGS",   // ML_THINGS,   Monsters, items..
   "LINEDEFS", // ML_LINEDEFS, LineDefs, from editing
   "SIDEDEFS", // ML_SIDEDEFS, SideDefs, from editing
   "VERTEXES", // ML_VERTEXES, Vertices, edited and BSP splits generated
   "SEGS",     // ML_SEGS,     LineSegs, from LineDefs split by BSP
   "SSECTORS", // ML_SSECTORS, SubSectors, list of LineSegs
   "NODES",    // ML_NODES,    BSP nodes
   "SECTORS",  // ML_SECTORS,  Sectors, from editing
   "REJECT",   // ML_REJECT,   LUT, sector-sector visibility
   "BLOCKMAP", // ML_BLOCKMAP  LUT, motion clipping, walls/grid element
   "BEHAVIOR"  // ML_BEHAVIOR  haleyjd: ACS bytecode; used to id hexen maps
};

//
// Lumps that are used in console map formats
//
static const char *consolelumps[] =
{
   "LEAFS",
   "LIGHTS",
   "MACROS"
};

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
      I_Error("Please provide the -file with path to wad\n");

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
// Picks the map from the command line
//
static int pickMap(const WadDirectory &dir, qstring &name)
{
   int parm = M_CheckParm("-map");
   if(!parm || parm + 1 >= myargc)
      I_Error("Please provide the -map with the map name\n");
   int mapnum = dir.checkNumForName(myargv[parm + 1]);
   if(mapnum == -1)
   {
      I_Error("Couldn't find map %s\n", myargv[parm + 1]);
   }
   name = myargv[parm + 1];
   return mapnum;
}

//
// P_checkConsoleFormat
//
// haleyjd 12/12/13: Check for supported console map formats
//
static int P_checkConsoleFormat(const WadDirectory &dir, int lumpnum)
{
   int          numlumps = dir.getNumLumps();
   lumpinfo_t **lumpinfo = dir.getLumpInfo();

   for(int i = ML_LEAFS; i <= ML_MACROS; i++)
   {
      int ln = lumpnum + i;
      if(ln >= numlumps ||     // past the last lump?
         strncmp(lumpinfo[ln]->name, consolelumps[i - ML_LEAFS], 8))
      {
         if(i == ML_LIGHTS)
            return LEVEL_FORMAT_PSX; // PSX
         else
            return LEVEL_FORMAT_INVALID; // invalid map
      }
   }

   // If we got here, dealing with Doom 64 format. (TODO: Not supported ... yet?)
   return LEVEL_FORMAT_INVALID; //LEVEL_FORMAT_DOOM64;
}

//
// Check the level now
//
int checkLevel(const WadDirectory &dir, int lumpnum)
{
   int numlumps = dir.getNumLumps();
   lumpinfo_t **lumpinfo = dir.getLumpInfo();

   for(int i = ML_THINGS; i <= ML_BEHAVIOR; i++)
   {
      int ln = lumpnum + i;
      if(ln >= numlumps ||     // past the last lump?
         strncmp(lumpinfo[ln]->name, levellumps[i], 8))
      {
         // If "BEHAVIOR" wasn't found, we assume we are dealing with
         // a DOOM-format map, and it is not an error; any other missing
         // lump means the map is invalid.

         if(i == ML_BEHAVIOR)
         {
            // If the current lump is named LEAFS, it's a console map
            if(ln < numlumps && !strncmp(lumpinfo[ln]->name, "LEAFS", 8))
               return P_checkConsoleFormat(dir, lumpnum);
            else
               return LEVEL_FORMAT_DOOM;
         }
         else
            return LEVEL_FORMAT_INVALID;
      }
   }

   // if we got here, we're dealing with a Hexen-format map
   return LEVEL_FORMAT_HEXEN;
}

//
// Handle PSX map
//
static void convertPSXMap(const WadDirectory &dir, int lumpnum)
{
   I_Error("PSX conversion not implemented yet!\n");
}

//
// Handle Doom format map (may have ExtraData)
//
static void convertDoomMap(const WadDirectory &dir, int lumpnum, FILE *f)
{
   // We already know the lumps are valid. Read them in the order they are
   // called in the game.
   {
      ZAutoBuffer buffer;
      int num = lumpnum + ML_VERTEXES;
      dir.cacheLumpAuto(num, buffer);
      auto data = buffer.getAs<const byte *>();
      int numvertices = dir.lumpLength(num) / 4;
      for(int i = 0; i < numvertices; ++i)
      {
         int16_t x = GetBinaryWord(&data);
         int16_t y = GetBinaryWord(&data);
         fprintf(f, "vertex{x=%d;y=%d;}", x, y);
      }
   }
   {
      ZAutoBuffer buffer;
      int num = lumpnum + ML_SECTORS;
      dir.cacheLumpAuto(num, buffer);
      auto data = buffer.getAs<const byte *>();
      int numsectors = dir.lumpLength(num) / 26;

   }
}

//
// Handle Hexen format map
//
static void convertHexenMap(const WadDirectory &dir, int lumpnum)
{
   I_Error("Hexen format conversion not implemented yet!\n");
}

//
// Handle Doom 64 format map
//
static void convertDoom64Map(const WadDirectory &dir, int lumpnum)
{
   I_Error("Doom format conversion not implemented yet!\n");
}

//
// Does much of what P_SetupLevel does in Eternity
//
static void setupLevel(const WadDirectory &dir, int lumpnum, const qstring &name,
                       FILE *f)
{

   int format = checkLevel(dir, lumpnum);
   switch (format) {
      case LEVEL_FORMAT_INVALID:
         I_Error("Invalid level %s\n", name.constPtr());
      case LEVEL_FORMAT_PSX:
         printf("%s is for PSX\n", name.constPtr());
         convertPSXMap(dir, lumpnum);
         break;
      case LEVEL_FORMAT_DOOM:
         printf("%s is of Doom format\n", name.constPtr());
         convertDoomMap(dir, lumpnum, f);
         break;
      case LEVEL_FORMAT_HEXEN:
         printf("%s is of Hexen format\n", name.constPtr());
         convertHexenMap(dir, lumpnum);
         break;
      case LEVEL_FORMAT_DOOM64:
         printf("%s is of Doom 64 format\n", name.constPtr());
         convertDoom64Map(dir, lumpnum);
         break;
      default:
         I_Error("Invalid format %d for level %s\n", format, name.constPtr());
   }
   fclose(f);
}

static FILE *getOutput()
{
   int parm = M_CheckParm("-out");
   if(!parm || parm + 1 >= myargc)
      I_Error("Please provide the -out with the TEXTMAP file to write\n");
   const char *outfile = myargv[parm + 1];
   FILE *f = fopen(outfile, "wt");
   if(!f)
      I_Error("Failed opening the TEXTMAP output file (error %d)\n", errno);
   fprintf(f, "namespace=\"eternity\";");
   return f;
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
   qstring name;
   int lumpnum = pickMap(dir, name);

   setupLevel(dir, lumpnum, name, getOutput());
   return 0;
}
