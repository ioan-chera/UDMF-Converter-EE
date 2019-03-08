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

#ifndef DoomLevel_hpp
#define DoomLevel_hpp

#include <vector>
#include "MapItems.h"

class Lump;
class Wad;

//
// Holds a pointer to a lump and associated data
//
struct LumpInfo
{
   const Lump *lump;
   int index;
};

//
// Doom level
//
class DoomLevel
{
public:
   bool LoadWad(const Wad &wad, size_t lumpIndex);
   static std::vector<LumpInfo> FindLevelLumps(const Wad &wad);

   const std::vector<Thing> &GetThings() const
   {
      return mThings;
   }
   const std::vector<Vertex> &GetVertices() const
   {
      return mVertices;
   }
   const std::vector<Sector> &GetSectors() const
   {
      return mSectors;
   }
   const std::vector<Sidedef> &GetSidedefs() const
   {
      return mSidedefs;
   }
   const std::vector<Linedef> &GetLinedefs() const
   {
      return mLinedefs;
   }

   const Wad *GetWad() const
   {
      return mWad;
   }

private:
   void LoadThings(const Lump &lump);
   void LoadLinedefs(const Lump &lump);
   void LoadSidedefs(const Lump &lump);
   void LoadVertices(const Lump &lump);
   void SeparateSegVertices();
   void LoadSegs(const Lump &lump);
   void LoadSubsectors(const Lump &lump);
   void LoadNodes(const Lump &lump);
   void LoadSectors(const Lump &lump);
   void LoadBlockmap(const Lump &lump);

   std::vector<Thing> mThings;
   std::vector<Linedef> mLinedefs;
   std::vector<Sidedef> mSidedefs;
   std::vector<Vertex> mVertices;
   std::vector<Vertex> mNodeVertices;
   std::vector<Seg> mSegs;
   std::vector<Subsector> mSubsectors;
   std::vector<Node> mNodes;
   std::vector<Sector> mSectors;
   std::vector<uint8_t> mReject;
   std::vector<int16_t> mBlockmap;

   const Wad *mWad = nullptr;
};

#endif /* DoomLevel_hpp */
