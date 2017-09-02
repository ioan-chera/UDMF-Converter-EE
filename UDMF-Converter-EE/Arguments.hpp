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

#ifndef Arguments_hpp
#define Arguments_hpp

#include <string>
#include <unordered_map>
#include <vector>
#include "Result.hpp"

class Arguments
{
public:
   Arguments(int argc, const char *argv[]);
   const std::vector<const char *> *Get(const char *key) const;
   const char *GetSingle(const char *key) const;
private:
   std::unordered_map<std::string, std::vector<const char *>> mArguments;
};

#endif /* Arguments_hpp */
