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

#include <string>
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
// Sector
//
struct EDSector
{
   bool hasflags;
   unsigned flags;
   unsigned flagsadd;
   unsigned flagsrem;
   int damage;
   int damagemask;
   std::string damagetype;
   bool hasdamageflags;
   unsigned damageflags;
   unsigned damageflagsadd;
   unsigned damageflagsrem;
   double floor_xoffs;
   double floor_yoffs;
   double ceiling_xoffs;
   double ceiling_yoffs;
   double floor_xscale;
   double floor_yscale;
   double ceiling_xscale;
   double ceiling_yscale;
   double floorangle;
   double ceilingangle;
   std::string topmap;
   std::string midmap;
   std::string bottommap;
   std::string floorterrain;
   std::string ceilingterrain;
   unsigned f_pflags;
   unsigned c_pflags;
   int f_alpha;
   int c_alpha;
   int f_portalid;
   int c_portalid;
};

//
// Holds ExtraData stuff
//
class ExtraData
{
public:
   bool LoadLump(const Wad &wad, const char *name);
   const EDThing *GetThing(int recordnum) const
   {
      return Get(mThings, recordnum);
   }

private:
   bool ProcessThings(cfg_t *cfg);
   bool ProcessLines(cfg_t *cfg);
   bool ProcessSectors(cfg_t *cfg);

   void Clear();

   template<typename T>
   const T *Get(const std::unordered_map<int, T> &map, int recordnum) const
   {
      auto it = map.find(recordnum);
      if(it != map.end())
         return &it->second;
      return nullptr;
   }

   std::unordered_map<int, EDThing> mThings;
   std::unordered_map<int, EDLine> mLines;
   std::unordered_map<int, EDSector> mSectors;
};

#endif /* ExtraData_hpp */
