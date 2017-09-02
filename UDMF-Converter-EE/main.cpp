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
// Purpose: Entry point
// Authors: Ioan Chera
//

#include "Arguments.hpp"
#include "Converter.hpp"
#include "Helpers.hpp"
#include "Wad.hpp"

//
// Entry point
//
int main(int argc, const char * argv[])
{
   Arguments args(argc, argv);
   const char *path = args.GetSingle("file");
   if(!path)
      path = args.GetSingle("wad");
   if(!path)
   {
      fprintf(stderr, "You must provide a -file or -wad parameter with the file path.\n");
      return EXIT_FAILURE;
   }
   const char *map = args.GetSingle("map");
   if(!map)
   {
      fprintf(stderr, "You must provide a -map parameter with the map name (e.g. -map MAP01).\n");
      return EXIT_FAILURE;
   }
   const char *outPath = args.GetSingle("out");
   if(!outPath)
   {
      fprintf(stderr, "You must provide an -out parameter with the output text map file (e.g. -out textmap.txt).\n");
      return EXIT_FAILURE;
   }

   Wad wad;
   Result result = wad.LoadFile(path);
   if(result != Result::OK)
   {
      fprintf(stderr, "Failed loading file '%s'. %s\n", path, ResultMessage(result));
      return EXIT_FAILURE;
   }

   Converter converter;
   result = converter.LoadWad(wad, map);
   if(result != Result::OK)
   {
      fprintf(stderr, "Failed converting WAD map '%s'. %s\n", map, ResultMessage(result));
      return EXIT_FAILURE;
   }

   // TODO: stuff

   return 0;
}
