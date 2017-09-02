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
// Purpose: Operation result
// Authors: Ioan Chera
//

#include "Result.hpp"

//
// Returns a constant text based on result
//
const char *ResultMessage(Result result)
{
   switch(result)
   {
      case Result::OK:
         return "Operation completed successfully.";
      case Result::BadFile:
         return "Invalid or corrupted file.";
      case Result::CannotOpen:
         return "Cannot open or file not found.";
      case Result::LevelNotFound:
         return "WAD file doesn't have the requested level.";
      default:
         return "Unknown error.";
   }
}
