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
// Purpose: ExtraData content
// Authors: Ioan Chera
//

#ifndef ExtraData_hpp
#define ExtraData_hpp

#include <unordered_map>

class Wad;
struct cfg_t;

enum
{
   kExtraDataDoomednum = 5004
};

//
// ExtraData thing
//
struct EDThing
{
   int type;
   unsigned options;
   int tid;
   int args[5];
   int height;
   int special;
};

//
// Line
//
struct EDLine
{
   int special;
   int tag;
   unsigned extflags;
   int args[5];
   double alpha;
   int portalid;
};

//
// Holds ExtraData stuff
//
class ExtraData
{
public:
   bool LoadLump(const Wad &wad, const char *name);

private:
   bool ProcessThings(cfg_t *cfg);
   bool ProcessLines(cfg_t *cfg);

   std::unordered_map<int, EDThing> mThings;
   std::unordered_map<int, EDLine> mLines;
};

#endif /* ExtraData_hpp */
