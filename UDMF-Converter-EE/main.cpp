//
// UDMF Converter EE
// Copyright (C) 2017 Ioan Chera
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
// Additional terms and conditions compatible with the GPLv3 apply. See the
// file COPYING-EE for details.
//
// Purpose: Entry point
// Authors: Ioan Chera
//

#include <sstream>
#include "Arguments.hpp"
#include "DoomLevel.hpp"
#include "ExtraData.hpp"
#include "Helpers.hpp"
#include "LineSpecialMapping.hpp"
#include "ThingMapping.hpp"
#include "UDMFItems.hpp"
#include "Wad.hpp"
#include "XLEMapInfoParser.hpp"
#include "ZNodes.hpp"

//
// Entry point
//
int main(int argc, const char * argv[])
{
   Arguments args(argc, argv);
   const std::vector<const char *> *paths = args.Get("file");
   if(!paths)
   {
      fprintf(stderr, "No wad files specified. Use -file followed by paths to wads.\n");
      return EXIT_FAILURE;
   }

   const char *outPath = args.GetSingle("out");
   if(!outPath)
   {
      fprintf(stderr, "You must provide an -out parameter with the output wad file with converted levels.\n");
      return EXIT_FAILURE;
   }

   Wad wad;
   Result result;
   for(const char *path : *paths)
   {
      result = wad.AddFile(path);
      if(result != Result::OK)
      {
         fprintf(stderr, "Failed loading file '%s'. %s\n", path, ResultMessage(result));
         return EXIT_FAILURE;
      }
   }

   const std::vector<const char *> *thinglists = args.Get("things");
   ThingMapping thingnames;
   if (thinglists)
      for (const char *list : *thinglists)
         thingnames.AddFromFile(list);

   // Initialize line mapping
   InitLineMapping();
   InitExtraDataMappings();

   // Look for EMAPINFO.
   XLEMapInfoParser emapinfo;
   emapinfo.ParseAll(wad);

   // Also look in individual levels
   std::vector<LumpInfo> levelLumps = DoomLevel::FindLevelLumps(wad);
   for(const LumpInfo &info : levelLumps)
   {
      emapinfo.SetLocalLevel(info.lump->Name());
      emapinfo.ParseLump(*info.lump);
   }

   // Convert the maps
   Wad outWad;
   for(const LumpInfo &info : levelLumps)
   {
      const char *name = info.lump->Name();
      const LevelInfo *levelInfo = emapinfo.Get(name);
      ExtraData extraData(thingnames);
      if(levelInfo)
      {
         auto it = levelInfo->find("extradata");
         if(it != levelInfo->end())
         {
            if(!extraData.LoadLump(wad, it->second.c_str()))
            {
               fprintf(stderr, "Warning: failed loading ExtraData %s for %s\n", it->second.c_str(),
                       name);
            }
            // Delete the ExtraData reference: it will be undesired in UDMF
            emapinfo.Erase(name, "extradata");
         }
      }

      DoomLevel level;
      if(!level.LoadWad(wad, info.index))
      {
         fprintf(stderr, "Failed loading level %s\n", name);
         continue;
      }
      printf("Loaded level %s\n", name);

      // Now we have both the level and its ExtraData loaded. Let's see how we convert it now
      UDMFLevel udmfLevel(level, extraData);

      // Create a wad with the new level lumps
      outWad.AddLump(Lump(name));   // marker
      std::ostringstream oss;
      oss << udmfLevel;
      outWad.AddLump(Lump("TEXTMAP", oss.str()));
      oss.clear();
      oss.seekp(0);
      WriteZNodes(level, oss);
      outWad.AddLump(Lump("ZNODES", oss.str()));
      // Also add reject and blockmap
      outWad.AddLump(Lump("REJECT", level.GetReject()));
      outWad.AddLump(Lump("BLOCKMAP", level.GetBlockmap()));
      outWad.AddLump(Lump("ENDMAP"));
   }

   result = wad.WriteFile(outPath);
   if(result != Result::OK)
   {
      fprintf(stderr, "Failed writing file '%s'. %s\n", outPath, ResultMessage(result));
      return EXIT_FAILURE;
   }

   return 0;
}
