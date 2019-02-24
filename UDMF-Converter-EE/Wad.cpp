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

#include "Wad.hpp"

//
// Tries to read a file
//
Result Wad::AddFile(const char *path)
{
   FILE *f = fopen(path, "rb");
   if(!f)
      return Result::CannotOpen;
   Result result = Result::OK;
   char headtag[5];
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

   if(fread(headtag, 1, 4, f) != 4)
   {
      result = Result::BadFile;
      goto cleanup;
   }
   headtag[4] = 0;
   if(!strcmp(headtag, "PWAD"))
      type = WadType::Pwad;
   else if(!strcmp(headtag, "IWAD"))
      type = WadType::Iwad;
   else
   {
      result = Result::BadFile;
      goto cleanup;
   }
   if(fread(&numlumps, 4, 1, f) != 1 || fread(&infotableofs, 4, 1, f) != 1 ||
      fseek(f, infotableofs, SEEK_SET) == -1)
   {
      result = Result::BadFile;
      goto cleanup;
   }
   // Don't reserve numlumps: it may be purposefully set huge to lock-up the app
   // Let it grow slowly so that we have a chance to bail out if we reach EOF
   // before it gets too big.
   for(int i = 0; i < numlumps; ++i)
   {
      LumpDirEntry lde;
      if(fread(&lde.filepos, 4, 1, f) != 1 || fread(&lde.size, 4, 1, f) != 1 ||
         fread(lde.name, 1, LumpNameLength, f) != LumpNameLength)
      {
         result = Result::BadFile;
         goto cleanup;
      }
      lde.name[LumpNameLength] = 0;
      directory.push_back(lde);
   }
   lumps.reserve(directory.size());
   for(const LumpDirEntry &lde : directory)
   {
      Lump lump(lde.name);
      if(fseek(f, lde.filepos, SEEK_SET) == -1)
      {
         result = Result::BadFile;
         goto cleanup;
      }
      result = lump.Load(f, lde.size);
      if(result != Result::OK)
         goto cleanup;
      lumps.push_back(std::move(lump));
   }

   rangePath = {
      .range = { static_cast<int>(mLumps.size()), static_cast<int>(lumps.size()) },
      .path = path
   };

   mLumps.insert(mLumps.end(), lumps.begin(), lumps.end());
   mRangePaths.push_back(rangePath);

cleanup:
   if(f)
      fclose(f);
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
