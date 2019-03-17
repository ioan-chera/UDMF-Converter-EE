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
// Purpose: Main conversion class
// Authors: Ioan Chera
//

#include <fstream>
#include "IOHelpers.hpp"
#include "Wad.hpp"

//
// Tries to read a file
//
Result Wad::AddFile(const char *path)
{
   std::ifstream is(path, std::ios::in | std::ios::binary);
   if(!is.is_open())
      return Result::CannotOpen;

   Result result = Result::OK;
   char headtag[5] = {};
   WadType type;
   int numlumps;
   int infotableofs;
   struct LumpDirEntry
   {
      int filepos, size;
      char name[LumpNameLength + 1];
   };
   std::vector<LumpDirEntry> directory;
   std::vector<Lump> lumps;

   RangePath rangePath;
   if(!is.read(headtag, 4))
      return Result::BadFile;

   headtag[4] = 0;
   if(!strcmp(headtag, "PWAD"))
      type = WadType::Pwad;
   else if(!strcmp(headtag, "IWAD"))
      type = WadType::Iwad;
   else
      return Result::BadFile;

   if(!ReadInt(is, numlumps) || !ReadInt(is, infotableofs) || !is.seekg(infotableofs))
      return Result::BadFile;

   // Don't reserve numlumps: it may be purposefully set huge to lock-up the app
   // Let it grow slowly so that we have a chance to bail out if we reach EOF
   // before it gets too big.
   for(int i = 0; i < numlumps; ++i)
   {
      LumpDirEntry lde = {};

      if(!ReadInt(is, lde.filepos) || !ReadInt(is, lde.size) || !is.read(lde.name, LumpNameLength))
         return Result::BadFile;

      lde.name[LumpNameLength] = 0;
      directory.push_back(lde);
   }
   lumps.reserve(directory.size());
   for(const LumpDirEntry &lde : directory)
   {
      Lump lump(lde.name);
      if(!is.seekg(lde.filepos))
         return Result::BadFile;

      result = lump.Load(is, lde.size);
      if(result != Result::OK)
         return result;
      lumps.push_back(std::move(lump));
   }

   rangePath = {
      .range = { static_cast<int>(mLumps.size()), static_cast<int>(lumps.size()) },
      .path = path
   };

   mLumps.insert(mLumps.end(), lumps.begin(), lumps.end());
   mRangePaths.push_back(rangePath);

   return result;
}

//
// Finds a lump from the wad, searching backwards.
//
const Lump *Wad::FindLump(const char *name, int *index) const
{
   int lumpIndex = static_cast<int>(mLumps.size() - 1);
   for(auto it = mLumps.rbegin(); it != mLumps.rend(); ++it, --lumpIndex)
   {
      if(!strcasecmp(it->Name(), name))
      {
         if(index)
            *index = lumpIndex;
         return &*it;
      }
   }
   return nullptr;
}
