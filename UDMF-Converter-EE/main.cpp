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

#include "Arguments.hpp"
#include "Converter.hpp"
#include "DoomLevel.hpp"
#include "Helpers.hpp"
#include "Wad.hpp"
#include "XLEMapInfoParser.hpp"

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

   // Look for EMAPINFO.
   XLEMapInfoParser emapinfo;
   emapinfo.ParseAll(wad);

   // Also look in individual levels
   std::vector<const Lump *> levelLumps = DoomLevel::FindLevelLumps(wad);
   for(const Lump *lump : levelLumps)
   {
      emapinfo.LocalLevel(lump->Name());
      emapinfo.ParseLump(*lump);
   }

   // TODO: parse ExtraData and EDF. Load the maps. Then run the converter.

   return 0;
}
