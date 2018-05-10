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
#include "m_swap.h"
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
   
   Result result = LoadThings(*mapLumps.things);
   if(result != Result::OK)
      return result;

   result = LoadVertices(*mapLumps.vertices);
   if(result != Result::OK)
      return result;

   result = LoadLinedefs(*mapLumps.linedefs);
   if(result != Result::OK)
      return result;

   result = LoadSidedefs(*mapLumps.sidedefs);
   if(result != Result::OK)
      return result;

   result = LoadSectors(*mapLumps.sectors);
   if(result != Result::OK)
      return result;

   return Result::OK;
}

//
// Called when a set-light-tag special is encountered
//
void Converter::SetLightTag(int special, int tag, int index)
{
   // TODO
}

//
// Called when the set-surface-control is called
//
void Converter::SetSurfaceControl(int special, int tag, int index)
{
   // TODO
}

//
// Called by the attach-to-control specials
//
void Converter::AttachToControl(int special, int tag, int index)
{
   // TODO
}

//
// Handles line type 270
//
void Converter::ResolveLineExtraData(int special, int tag, int index)
{
   // TODO
}

//
// Handles line type 401
//
void Converter::ResolveSectorExtraData(int special, int tag, int index)
{
   // TODO
}

//
// Sets line ID based on special
//
void Converter::SetLineID(int special, int tag, int index)
{

}

//
// Define a portal
//
void Converter::PortalDefine(int special, int tag, int index)
{

}

//
// Define a portal
//
void Converter::QuickLinePortal(int special, int tag, int index)
{

}

//
// Converts a translucent line with a special
//
void Converter::TranslucentLine(int special, int tag, int index)
{

}

//
// Loads things
//
Result Converter::LoadThings(const Lump &things)
{
   if(things.data().size() % 10)
      return Result::BadData;
   size_t numthings = things.data().size() / 10;
   mThings.resize(numthings);
   for(size_t i = 0; i < numthings; ++i)
   {
      Thing &t = mThings[i];
      const int16_t *dat =
      reinterpret_cast<const int16_t *>(&things.data()[10 * i]);

      t.x = SwapShort(dat[0]);
      t.y = SwapShort(dat[1]);
      t.angle = SwapShort(dat[2]);
      t.type = SwapShort(dat[3]);
      t.flags = SwapShort(dat[4]);
   }
   return Result::OK;
}

//
// Loads vertices
//
Result Converter::LoadVertices(const Lump &vertices)
{
   if(vertices.data().size() % 4)
      return Result::BadData;
   size_t numverts = vertices.data().size() / 4;
   mVertices.resize(numverts);
   for(size_t i = 0; i < numverts; ++i)
   {
      Vertex &v = mVertices[i];
      const int16_t *dat = reinterpret_cast<const int16_t *>(&vertices.data()[4 * i]);

      v.x = SwapShort(dat[0]);
      v.y = SwapShort(dat[1]);
   }

   return Result::OK;
}

//
// Loads lines
//
Result Converter::LoadLinedefs(const Lump &linedefs)
{
   if(linedefs.data().size() % 14)
      return Result::BadData;
   size_t numlines = linedefs.data().size() / 14;
   mLinedefs.resize(numlines);
   for(size_t i = 0; i < numlines; ++i)
   {
      Linedef &l = mLinedefs[i];
      const int16_t *dat = reinterpret_cast<const int16_t *>(&linedefs.data()[14 * i]);

      l.v1 = SwapShort(dat[0]);
      l.v2 = SwapShort(dat[1]);
      l.flags = SwapShort(dat[2]);
      l.special = SwapShort(dat[3]);
      l.tag = SwapShort(dat[4]);
      l.sidenum[0] = SwapShort(dat[5]);
      l.sidenum[1] = SwapShort(dat[6]);
   }
   return Result::OK;
}

//
// Loads sides
//
Result Converter::LoadSidedefs(const Lump &sidedefs)
{
   if(sidedefs.data().size() % 30)
      return Result::BadData;
   size_t numsides = sidedefs.data().size() / 30;
   mSidedefs.resize(numsides);
   char lname[9] = { 0 };
   for(size_t i = 0; i < numsides; ++i)
   {
      Sidedef &s = mSidedefs[i];
      const int16_t *dat = reinterpret_cast<const int16_t *>(&sidedefs.data()[30 * i]);

      s.xoffset = SwapShort(dat[0]);
      s.yoffset = SwapShort(dat[1]);
      strncpy(lname, (const char *)(dat + 2), 8);
      s.upperpic = lname;
      strncpy(lname, (const char *)(dat + 6), 8);
      s.lowerpic = lname;
      strncpy(lname, (const char *)(dat + 10), 8);
      s.midpic = lname;
      s.sector = SwapShort(dat[14]);
   }
   return Result::OK;
}

//
// Loads sectors
//
Result Converter::LoadSectors(const Lump &sectors)
{
   if(sectors.data().size() % 26)
      return Result::BadData;
   size_t numsectors = sectors.data().size() / 26;
   mSectors.resize(numsectors);
   char lname[9] = { 0 };
   for(size_t i = 0; i < numsectors; ++i)
   {
      Sector &s = mSectors[i];
      const uint8_t *dat = &sectors.data()[26 * i];
      s.floorheight = SwapShort(*(const int16_t*)dat);
      s.ceilingheight = SwapShort(*(const int16_t*)(dat + 2));
      strncpy(lname, (const char *)(dat + 4), 8);
      s.floorpic = lname;
      strncpy(lname, (const char *)(dat + 12), 8);
      s.ceilingpic = lname;
      s.lightlevel = SwapShort(*(const int16_t*)(dat + 20));
      s.special = SwapShort(*(const int16_t*)(dat + 22));
      s.tag = SwapShort(*(const int16_t*)(dat + 24));
   }

   return Result::OK;
}
