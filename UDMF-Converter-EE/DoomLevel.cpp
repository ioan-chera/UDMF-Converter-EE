//
// UDMF Converter EE
// Copyright (C) 2018 Ioan Chera
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
// Purpose: Doom format level
// Authors: Ioan Chera
//

#include <algorithm>
#include "DataStreamer.hpp"
#include "DoomLevel.hpp"
#include "Wad.hpp"

//
// Loads a wad
//
bool DoomLevel::LoadWad(const Wad &wad, size_t lumpIndex)
{
   if(lumpIndex + 10 >= wad.Lumps().size())
      return false;
   LoadThings(wad.Lumps()[lumpIndex + 1]);
   LoadLinedefs(wad.Lumps()[lumpIndex + 2]);
   LoadSidedefs(wad.Lumps()[lumpIndex + 3]);
   LoadVertices(wad.Lumps()[lumpIndex + 4]);
   SeparateSegVertices();
   LoadSegs(wad.Lumps()[lumpIndex + 5]);
   LoadSubsectors(wad.Lumps()[lumpIndex + 6]);
   LoadNodes(wad.Lumps()[lumpIndex + 7]);
   LoadSectors(wad.Lumps()[lumpIndex + 8]);
   mReject = wad.Lumps()[lumpIndex + 9].Data();
   LoadBlockmap(wad.Lumps()[lumpIndex + 10]);
   mWad = &wad;
   return true;
}

//
// Locates levels in a wad's lump list
//
std::vector<LumpInfo> DoomLevel::FindLevelLumps(const Wad &wad)
{
   static const char *const doomLumpNames[] =
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

   std::vector<LumpInfo> ret;

   const auto &lumps = wad.Lumps();
   for(auto it = lumps.begin(); it != lumps.end(); ++it)
   {
      bool isLevel = true;
      auto jt = it + 1;
      for(; jt != lumps.end() && jt < it + 11; ++jt)
      {
         if(strcasecmp(jt->Name(), doomLumpNames[jt - it - 1]))
         {
            isLevel = false;
            break;
         }
      }
      if(!isLevel || jt < it + 11 || (jt == it + 11 &&
                                      !strcasecmp(jt->Name(), "BEHAVIOR")))
      {
         continue;   // TODO: add support for Hexen maps
      }
      DoomLevel level;

      LumpInfo info = {};
      info.lump = &*it;
      info.index = static_cast<int>(it - lumps.begin());
      ret.push_back(info);
      it += 10;
   }
   return ret;
}

//
// Loads all things
//
void DoomLevel::LoadThings(const Lump &lump)
{
   mThings.resize(lump.Data().size() / 10);
   DataStreamer stream(lump.Data());
   for(Thing &thing : mThings)
   {
      thing.x = stream.ReadShort();
      thing.y = stream.ReadShort();
      thing.angle = stream.ReadShort();
      thing.type = stream.ReadShort();
      thing.flags = stream.ReadShort();
   }
}

void DoomLevel::LoadLinedefs(const Lump &lump)
{
   mLinedefs.resize(lump.Data().size() / 14);
   DataStreamer stream(lump.Data());
   for(Linedef &linedef : mLinedefs)
   {
      linedef.v1 = stream.ReadShort();
      linedef.v2 = stream.ReadShort();
      linedef.flags = stream.ReadShort();
      linedef.special = stream.ReadShort();
      linedef.tag = stream.ReadShort();
      linedef.sidenum[0] = stream.ReadShort();
      linedef.sidenum[1] = stream.ReadShort();
   }
}

void DoomLevel::LoadSidedefs(const Lump &lump)
{
   mSidedefs.resize(lump.Data().size() / 30);
   DataStreamer stream(lump.Data());
   for(Sidedef &sidedef : mSidedefs)
   {
      sidedef.xoffset = stream.ReadShort();
      sidedef.yoffset = stream.ReadShort();
      sidedef.upperpic = stream.ReadString(8);
      sidedef.lowerpic = stream.ReadString(8);
      sidedef.midpic = stream.ReadString(8);
      sidedef.sector = stream.ReadShort();
   }
}

void DoomLevel::LoadVertices(const Lump &lump)
{
   mVertices.resize(lump.Data().size() / 4);
   DataStreamer stream(lump.Data());
   for(Vertex &vertex : mVertices)
   {
      vertex.x = stream.ReadShort();
      vertex.y = stream.ReadShort();
   }
}

void DoomLevel::SeparateSegVertices()
{
   if(mVertices.empty())
      return;
   size_t index = mVertices.size() - 1;
   mNodeVertices.reserve(mVertices.size());
   for(auto it = mVertices.rbegin(); it != mVertices.rend(); ++it, --index)
   {
      bool found = false;
      for(const Linedef &linedef : mLinedefs)
      {
         if(linedef.v1 == index || linedef.v2 == index)
         {
            found = true;
            break;
         }
      }
      if(!found)
      {
         mNodeVertices.push_back(*it);
         mVertices.pop_back();
      }
   }
   std::reverse(mNodeVertices.begin(), mNodeVertices.end());
}

void DoomLevel::LoadSegs(const Lump &lump)
{
   mSegs.resize(lump.Data().size() / 12);
   DataStreamer stream(lump.Data());
   for(Seg &seg : mSegs)
   {
      seg.startVertex = stream.ReadShort();
      seg.endVertex = stream.ReadShort();
      seg.angle = stream.ReadShort();
      seg.linedef = stream.ReadShort();
      seg.dir = stream.ReadShort();
      seg.offset = stream.ReadShort();
   }
}

void DoomLevel::LoadSubsectors(const Lump &lump)
{
   mSegs.resize(lump.Data().size() / 4);
   DataStreamer stream(lump.Data());
   for(Subsector &subsector : mSubsectors)
   {
      subsector.segcount = stream.ReadShort();
      subsector.startseg = stream.ReadShort();
   }
}

void DoomLevel::LoadNodes(const Lump &lump)
{
   mNodes.resize(lump.Data().size() / 28);
   DataStreamer stream(lump.Data());
   for(Node &node : mNodes)
   {
      node.partx = stream.ReadShort();
      node.party = stream.ReadShort();
      node.dx = stream.ReadShort();
      node.dy = stream.ReadShort();
      node.rightbox[0] = stream.ReadShort();
      node.rightbox[1] = stream.ReadShort();
      node.rightbox[2] = stream.ReadShort();
      node.rightbox[3] = stream.ReadShort();
      node.leftbox[0] = stream.ReadShort();
      node.leftbox[1] = stream.ReadShort();
      node.leftbox[2] = stream.ReadShort();
      node.leftbox[3] = stream.ReadShort();
      node.rightchild = stream.ReadShort();
      node.leftchild = stream.ReadShort();
   }
}

void DoomLevel::LoadSectors(const Lump &lump)
{
   mSectors.resize(lump.Data().size() / 26);
   DataStreamer stream(lump.Data());
   for(Sector &sector : mSectors)
   {
      sector.floorheight = stream.ReadShort();
      sector.ceilingheight = stream.ReadShort();
      sector.floorpic = stream.ReadString(8);
      sector.ceilingpic = stream.ReadString(8);
      sector.lightlevel = stream.ReadShort();
      sector.special = stream.ReadShort();
      sector.tag = stream.ReadShort();
   }
}

void DoomLevel::LoadBlockmap(const Lump &lump)
{
   mBlockmap.resize(lump.Data().size() / 2);
   DataStreamer stream(lump.Data());
   for(int16_t &val : mBlockmap)
      val = stream.ReadShort();
}
