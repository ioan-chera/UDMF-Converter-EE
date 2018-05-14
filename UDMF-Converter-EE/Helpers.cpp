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

#include "Helpers.hpp"

std::string LowerCase(const char *string)
{
   if(!string)
      return "";
   std::string result;
   size_t length = strlen(string);
   result.resize(length);
   for(size_t i = 0; i < length; ++i)
      result[i] = tolower(string[i]);
   return result;
}

void MakeLowerCase(std::string &string)
{
   for(char &c : string)
      c = tolower(c);
}

std::string UpperCase(const char *string)
{
   if(!string)
      return "";
   std::string result;
   size_t length = strlen(string);
   result.resize(length);
   for(size_t i = 0; i < length; ++i)
      result[i] = toupper(string[i]);
   return result;
}

void MakeUpperCase(std::string &string)
{
   for(char &c : string)
      c = toupper(c);
}

std::string Escape(const std::string &string)
{
   std::string ret;
   ret.reserve(string.length() * 2);
   for(char c : string)
   {
      if(c == '"' || c == '\\')
         ret.push_back('\\');
      ret.push_back(c);
   }
   return ret;
}
