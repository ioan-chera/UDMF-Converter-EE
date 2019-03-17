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

#ifndef IOHelpers_hpp
#define IOHelpers_hpp

#include <stdint.h>
#include <istream>
#include <ostream>

bool ReadInt(std::istream &is, int &number);

void WriteInt(intptr_t number, std::ostream &os);
void WriteShort(intptr_t number, std::ostream &os);

template <typename T>
void WriteData(const std::vector<T> &data, std::ostream &os)
{
   os.write(reinterpret_cast<const char *>(data.data()), data.size() * sizeof(T));
}

#endif /* IOHelpers_hpp */
