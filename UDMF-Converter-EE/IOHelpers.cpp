//
// UDMF Converter EE
// Copyright (C) 2019 Ioan Chera
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
// Purpose: Helpers for iostream
// Authors: Ioan Chera
//

#include "IOHelpers.hpp"

bool ReadInt(std::istream &is, int &number)
{
   char n[4];
   if(!is.read(n, 4))
      return false;
   number = n[0] | n[1] << 8 | n[2] << 16 | n[3] << 24;
   return true;
}

void WriteInt(intptr_t number, std::ostream &os)
{
   char n[4];
   n[0] = number & 0xff;
   n[1] = number >> 8 & 0xff;
   n[2] = number >> 16 & 0xff;
   n[3] = number >> 24 & 0xff;
   os.write(n, 4);
}

void WriteShort(intptr_t number, std::ostream &os)
{
   char n[2];
   n[0] = number & 0xff;
   n[1] = number >> 8 & 0xff;
   os.write(n, 2);
}
