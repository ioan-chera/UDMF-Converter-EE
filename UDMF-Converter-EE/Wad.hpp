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
// Purpose: Wad container class
// Authors: Ioan Chera
//

#ifndef Wad_hpp
#define Wad_hpp

#include <string>
#include "Lump.hpp"
#include "Range.h"
#include "Result.hpp"

//
// Wad kind
//
enum class WadType
{
   Iwad, // internal WAD
   Pwad  // patch WAD
};

//
// Gets the path of a range
//
struct RangePath
{
   Range range;
   std::string path;
};

//
// Wad class
//
class Wad
{
public:
   Wad()
   {
   }

   Result AddFile(const char *path);

   const std::vector<Lump> &Lumps() const
   {
      return mLumps;
   }

   const Lump *FindLump(const char *name, int *index = nullptr) const;
   
private:
   std::vector<Lump> mLumps;
   std::vector<RangePath> mRangePaths;
};

#endif /* Wad_hpp */
