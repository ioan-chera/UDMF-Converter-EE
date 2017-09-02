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
// Purpose: Command-line arguments
// Authors: Ioan Chera
//

#include "Arguments.hpp"
#include "Helpers.hpp"

//
// Loads arguments from input
//
Arguments::Arguments(int argc, const char *argv[])
{
   std::string key;
   for(int i = 1; i <= argc; ++i)
   {
      if(argv[i][0] == '-')
      {
         key = argv[i];
         key.erase(0, 1);
         MakeLowerCase(key);
         mArguments[key].reserve(1);   // just make it exist
      }
      else
         mArguments[key].push_back(argv[i]);
   }
}

//
// Gets the argument from key
//
const std::vector<const char *> *Arguments::Get(const char *key) const
{
   std::string formattedKey(key);
   MakeLowerCase(formattedKey);
   auto value = mArguments.find(formattedKey);
   if(value != mArguments.cend())
      return &value->second;
   return nullptr;
}

const char *Arguments::GetSingle(const char *key) const
{
   const std::vector<const char *> *values = Get(key);
   if(NullOrEmpty(values))
      return nullptr;
   return values->front();
}
