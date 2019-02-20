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
// Purpose: UDMF map items
// Authors: Ioan Chera
//

#include "UDMFItems.hpp"

//
// Gets a UDMF thing from a basic thing
//
UDMFThing::UDMFThing(const Thing &thing) : id(), x(thing.x), y(thing.y), height(),
angle(thing.angle), type(thing.type), flags(UTF_SINGLE | UTF_DM | UTF_COOP), special(), arg{}, health()
{
   // TODO: id, height, type from extraData
   unsigned thflags = thing.flags;
   if(thflags & TF_RESERVED)
      thflags &= TF_EASY | TF_NORMAL | TF_HARD | TF_AMBUSH | TF_NOTSINGLE;
   if(thflags & TF_EASY)
      flags |= UTF_SKILL1 | UTF_SKILL2;
   if(thflags & TF_NORMAL)
      flags |= UTF_SKILL3;
   if(thflags & TF_HARD)
      flags |= UTF_SKILL4 | UTF_SKILL5;
   if(thflags & TF_AMBUSH)
      flags |= UTF_AMBUSH;
   if(thflags & TF_NOTSINGLE)
      flags &= ~UTF_SINGLE;
   if(thflags & TF_NOTDM)
      flags &= ~UTF_DM;
   if(thflags & TF_NOTCOOP)
      flags &= ~UTF_COOP;
   if(thflags & TF_FRIEND)
      flags |= UTF_FRIEND;
   if(thflags & TF_DORMANT)
      flags |= UTF_DORMANT;
}
