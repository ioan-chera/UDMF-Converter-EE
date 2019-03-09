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
// Purpose: name-doomednum mapping
// Authors: Ioan Chera
//

#include <stdio.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Helpers.hpp"
#include "ThingMapping.hpp"

void ThingMapping::AddFromFile(const char *path)
{
   std::ifstream f(path);
   if (!f.is_open())
   {
      fprintf(stderr, "Failed opening thingtype file %s\n", path);
      return;
   }
   std::string data, data2;
   char *endptr;
   while(!f.eof())
   {
      f >> data >> data2;
      endptr = nullptr;
      int value = (int)strtol(data2.c_str(), &endptr, 10);
      if(!endptr || (endptr && *endptr) || value <= 0)
      {
         fprintf(stderr, "Invalid doomednum for %s\n", data.c_str());
         continue;
      }
      MakeLowerCase(data);
      mMap[data] = value;
   }
}

int ThingMapping::operator[](const char *name) const
{
   auto it = mMap.find(name);
   return it == mMap.end() ? -1 : it->second;
}
