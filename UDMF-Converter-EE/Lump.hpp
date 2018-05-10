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
// Purpose: Lump data class
// Authors: Ioan Chera
//

#ifndef Lump_hpp
#define Lump_hpp

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "Result.hpp"

enum
{
   LumpNameLength = 8,  // lump name size is limited
};

//
// Lump class
//
class Lump
{
public:
   Lump()
   {
      memset(mName, 0, sizeof(mName));
   }
   explicit Lump(const char name[LumpNameLength + 1])
   {
      strcpy(mName, name);
   }

   Result Load(FILE *f, size_t size);
   const char *Name() const
   {
      return mName;
   }

   const std::vector<uint8_t> &data() const
   {
      return mData;
   }
private:
   char mName[LumpNameLength + 1];  // lump name
   std::vector<uint8_t> mData;      // lump content
};

#endif /* Lump_hpp */
