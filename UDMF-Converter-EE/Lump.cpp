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

//
// Tries loading lump from a stream
//
Result Lump::Load(std::istream &is, size_t size)
{
   std::vector<uint8_t> data;
   data.resize(size);

   if(!is.read(reinterpret_cast<char *>(data.data()), size))
      return Result::BadFile;

   mData = std::move(data);
   return Result::OK;
}
