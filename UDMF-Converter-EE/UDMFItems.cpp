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

#include <string.h>
#include "DoomLevel.hpp"
#include "ExtraData.hpp"
#include "UDMFItems.hpp"

//
// Gets a UDMF thing from a basic thing
//
UDMFThing::UDMFThing(const Thing &thing, const ExtraData &extraData) :
id(),
x(thing.x),
y(thing.y),
height(),
angle(thing.angle),
type(thing.type),
flags(UTF_SINGLE | UTF_DM | UTF_COOP),
special(),
arg{},
health()
{
   // TODO: id, height, type from extraData
   SetUDMFFlagsFromDoomFlags(thing.flags);

   if(type == kExtraDataDoomednum)
   {
      const EDThing *edThing = extraData.GetThing(thing.flags);
      if(!edThing)
      {
         fprintf(stderr, "Warning: missing ExtraData mapthing record %u, skipping thing at position %d %d\n",
                 thing.flags, thing.x, thing.y);
         return;
      }

      type = edThing->type;
      SetUDMFFlagsFromDoomFlags(edThing->options);
      id = edThing->tid;
      memcpy(arg, edThing->args, sizeof(arg));
      height = edThing->height;
      special = edThing->special;
   }
}

//
// Converts Doom flags to UDMF flags
//
void UDMFThing::SetUDMFFlagsFromDoomFlags(unsigned thflags)
{
   flags = UTF_SINGLE | UTF_DM | UTF_COOP;
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

//
// Get the sector now
// We can't get info from ExtraData immediately, we need to have a linedef first
//
UDMFSector::UDMFSector(const Sector &sector) :
heightfloor(sector.floorheight),
heightceiling(sector.ceilingheight),
texturefloor(sector.floorpic),
textureceiling(sector.ceilingpic),
lightlevel(sector.lightlevel),
special(sector.special),
id(sector.tag),
xpanningfloor(),
ypanningfloor(),
xpanningceiling(),
xscalefloor(1),
yscalefloor(1),
xscaleceiling(1),
yscaleceiling(1),
rotationfloor(),
rotationceiling(),
flags(),
friction(-1),
leakiness(),
damageamount(),
damageinterval(),
damagetype("Unknown"),
floorterrain("@flat"),
ceilingterrain("@flat"),
lightfloor(),
lightceiling(),
colormaptop("@default"),
colormapmid("@default"),
colormapbottom("@default"),
scroll_ceil_x(),
scroll_ceil_y(),
scroll_ceil_type("none"),
scroll_floor_x(),
scroll_floor_y(),
scroll_floor_type("none"),
floorid(),
ceilingid(),
attachfloor(),
attachceiling(),
soundsequence(),
portalfloor(),
portalceiling(),
portal_floor_overlaytype("none"),
alphafloor(1),
portal_ceil_overlaytype("none"),
alphaceiling(1)
{
}

//
// UDMF level maker, resulted from input Doom level and extra Data
//
UDMFLevel::UDMFLevel(const DoomLevel &level, const ExtraData &extraData)
{
   mThings.reserve(level.GetThings().size());
   for(const Thing &thing : level.GetThings())
      mThings.emplace_back(thing, extraData);

   mVertices.reserve(level.GetVertices().size());
   for(const Vertex &vertex : level.GetVertices())
      mVertices.emplace_back(vertex);

   mSectors.reserve(level.GetVertices().size());
   for(const Sector &sector : level.GetSectors())
      mSectors.emplace_back(sector);
}
