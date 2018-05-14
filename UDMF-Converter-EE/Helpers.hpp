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
// Purpose: Standard library helpers
// Authors: Ioan Chera
//

#ifndef Helpers_hpp
#define Helpers_hpp

#include <string>

#define lengthof(x) (sizeof(x) / sizeof(*(x)))

std::string LowerCase(const char *string);
void MakeLowerCase(std::string &string);
std::string UpperCase(const char *string);
void MakeUpperCase(std::string &string);

std::string Escape(const std::string &string);

template<typename T>
inline static bool NullOrEmpty(const T *vector)
{
   return !vector || vector->empty();
}

#endif /* Helpers_hpp */
