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

#ifndef Converter_hpp
#define Converter_hpp

#include <vector>
#include "MapItems.h"
#include "Result.hpp"

class Lump;
class Wad;

class Converter
{
public:
   Result LoadWad(const Wad &wad, const char *mapName);

   void SetLightTag(int special, int tag, int index);
   void SetSurfaceControl(int special, int tag, int index);
   void AttachToControl(int special, int tag, int index);
   void ResolveLineExtraData(int special, int tag, int index);
   void ResolveSectorExtraData(int special, int tag, int index);
   void SetLineID(int special, int tag, int index);
   void PortalDefine(int special, int tag, int index);
   void QuickLinePortal(int special, int tag, int index);
   void TranslucentLine(int special, int tag, int index);

private:
   Result LoadThings(const Lump &things);
   Result LoadVertices(const Lump &vertices);
   Result LoadLinedefs(const Lump &linedefs);
   Result LoadSidedefs(const Lump &sidedefs);
   Result LoadSectors(const Lump &sectors);

   std::vector<Thing> mThings;
   std::vector<Vertex> mVertices;
   std::vector<Linedef> mLinedefs;
   std::vector<Sidedef> mSidedefs;
   std::vector<Sector> mSectors;
};

#endif /* Converter_hpp */
