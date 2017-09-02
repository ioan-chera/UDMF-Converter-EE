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

#include "Lump.hpp"

enum
{
   ReadSize = 4096,  // Read at most this size
};

//
// Tries loading lump from a stream
//
Result Lump::Load(FILE *f, size_t size)
{
   std::vector<uint8_t> data;
   // Protect against excessive size by being able to quit on EOF
   size_t toRead;
   uint8_t block[ReadSize];
   for(size_t i = 0; i < size; i += toRead)
   {
      toRead = ReadSize;
      if(size - i < toRead)
         toRead = size - i;
      if(fread(block, 1, toRead, f) < toRead)
         return Result::BadFile;
      for(size_t j = 0; j < toRead; ++j)
         data.push_back(block[j]);
   }

   mData = std::move(data);
   return Result::OK;
}
