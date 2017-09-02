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
// Purpose: Main conversion class
// Authors: Ioan Chera
//

#include "Converter.hpp"
#include "Helpers.hpp"
#include "Wad.hpp"

//
// Holds the map lump addresses
//
struct MapLumps
{
   const Lump *things;
   const Lump *linedefs;
   const Lump *sidedefs;
   const Lump *vertices;
   const Lump *sectors;
};

//
// Checks that a given lump is for levels
//
static bool CheckLumpIsLevel(const std::vector<Lump> &lumps, const Lump &lump,
                             MapLumps &mapLumps)
{
   static const char *const lumpNames[] =
   {
      "THINGS",
      "LINEDEFS",
      "SIDEDEFS",
      "VERTEXES",
      "SEGS",
      "SSECTORS",
      "NODES",
      "SECTORS",
      "REJECT",
      "BLOCKMAP"
   };

   mapLumps = {};

   ptrdiff_t index = &lump - &lumps[0];
   if(index + lengthof(lumpNames) >= lumps.size())
      return false;
   for(size_t i = 0; i < lengthof(lumpNames); ++i)
   {
      const Lump &dataLump = lumps[index + i + 1];
      if(strcasecmp(dataLump.Name(), lumpNames[i]))
         return false;
      else if(!strcasecmp(lumpNames[i], "THINGS"))
         mapLumps.things = &dataLump;
      else if(!strcasecmp(lumpNames[i], "LINEDEFS"))
         mapLumps.linedefs = &dataLump;
      else if(!strcasecmp(lumpNames[i], "SIDEDEFS"))
         mapLumps.sidedefs = &dataLump;
      else if(!strcasecmp(lumpNames[i], "VERTEXES"))
         mapLumps.vertices = &dataLump;
      else if(!strcasecmp(lumpNames[i], "SECTORS"))
         mapLumps.sectors = &dataLump;
   }
   return true;
}

//
// Tries loading from WAD
//
Result Converter::LoadWad(const Wad &wad, const char *mapName)
{
   const std::vector<Lump> &lumps = wad.Lumps();
   MapLumps mapLumps;
   bool found = false;
   for(const Lump &lump : lumps)
   {
      if(!strcasecmp(lump.Name(), mapName) && CheckLumpIsLevel(lumps, lump,
                                                               mapLumps))
      {
         found = true;
         break;
      }
   }
   if(!found)
      return Result::LevelNotFound;
   
   return Result::OK;
}
