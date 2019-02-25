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
#include "LineSpecialMapping.hpp"

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
// LINEDEF SETUP
//
UDMFLine::UDMFLine(const Linedef &linedef, LinedefConversion &conversion) :
id(linedef.tag),
v{ linedef.v1, linedef.v2 },
flags(),
special(),
arg{},
sidefront(linedef.sidenum[0]),
sideback(linedef.sidenum[1]),
portal(),
alpha(1.0)
{
   unsigned ldflags = linedef.flags;
   if(ldflags & LF_RESERVED)
      ldflags &= 0x1FF;
   if(ldflags & LF_BLOCKING)
      flags |= ULF_BLOCKING;
   if(ldflags & LF_BLOCKMONSTERS)
      flags |= ULF_BLOCKMONSTERS;
   if(ldflags & LF_TWOSIDED)
      flags |= ULF_TWOSIDED;
   if(ldflags & LF_DONTPEGTOP)
      flags |= ULF_DONTPEGTOP;
   if(ldflags & LF_DONTPEGBOTTOM)
      flags |= ULF_DONTPEGBOTTOM;
   if(ldflags & LF_SECRET)
      flags |= ULF_SECRET;
   if(ldflags & LF_BLOCKSOUND)
      flags |= ULF_BLOCKSOUND;
   if(ldflags & LF_HIDDEN)
      flags |= ULF_DONTDRAW;
   if(ldflags & LF_MAPPED)
      flags |= ULF_MAPPED;
   if(ldflags & LF_PASSUSE)
      flags |= ULF_PASSUSE;
   if(ldflags & LF_3DMIDTEX)
      flags |= ULF_MIDTEX3D;
   if(linedef.special)
      HandleDoomSpecial(linedef.special, linedef.tag, conversion);
}

//
// Handles a Doom special, applying conversion
//
void UDMFLine::HandleDoomSpecial(int lnspecial, int tag, LinedefConversion &conversion)
{
   const UdmfSpecialTarget *target = GetUDMFSpecial(lnspecial);
   if(!target)
      fprintf(stderr, "Invalid linedef special %d (see vertices %d and %d)\n", lnspecial,
              v[0], v[1]);
   else
   {
      special = target->special;
      memcpy(arg, target->args, sizeof(arg));
      if(target->flags & PlayerCrosses)
         flags |= ULF_PLAYERCROSS;
      if(target->flags & MonsterCrosses)
         flags |= ULF_MONSTERCROSS;
      if(target->flags & PlayerUses)
         flags |= ULF_PLAYERUSE;
      if(target->flags & MonsterUses)
         flags |= ULF_MONSTERUSE;
      if(target->flags & PlayerShoots)
         flags |= ULF_IMPACT;
      if(target->flags & MonsterShoots)
         flags |= ULF_MONSTERSHOOT;
      if(target->flags & Repeatable)
         flags |= ULF_REPEATSPECIAL;
      if(!(target->flags & NoTag))
      {
         if(target->flags & TagSecond)
            arg[1] = tag;
         else if(target->flags & TagThird)
            arg[2] = tag;
         else if(target->flags & TagFirstSecond)
            arg[0] = arg[1] = tag;
         else
            arg[0] = tag;
      }
      if(target->flags & FirstSide)
         flags |= ULF_FIRSTSIDEONLY;
      if(target->additional)
         (conversion.*target->additional)(lnspecial, tag, *this);
   }
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
   switch (sector.special)
   {
         // TODO: other games than Doom
      case 1:
         special = 65;
         break;
      case 2:
         special = 66;
         break;
      case 3:
         special = 67;
         break;
      case 4:
         special = 68;
         break;
      case 5:
         special = 69;
         break;
      case 7:
         special = 71;
         break;
      case 8:
         special = 72;
         break;
      case 9:
         special = 1024;
         flags |= USF_SECRET;
         break;
      case 10:
         special = 74;
         break;
      case 11:
         special = 75;
         break;
      case 12:
         special = 76;
         break;
      case 13:
         special = 77;
         break;
      case 14:
         special = 78;
         break;
      case 16:
         special = 80;
         break;
      case 17:
         special = 81;
         break;
      default:
         // Generalized
      {
         int dmgflags = sector.special & 96;
         if(dmgflags == 32)
         {
            damageamount = 5;
            damageinterval = 32;
            damagetype = "Slime";
         }
         else if(dmgflags == 64)
         {
            damageamount = 10;
            damageinterval = 32;
            damagetype = "Slime";
         }
         else if(dmgflags == 96)
         {
            damageamount = 20;
            damageinterval = 32;
            damagetype = "Slime";
            leakiness = 5;
         }
         if(sector.special & 128)
         {
            special |= 1024;
            flags |= USF_SECRET;
         }
         if(sector.special & 256)
            special |= 2048;
         if(sector.special & 512)
            special |= 4096;
         if(sector.special & 1024)
            special |= 8192;
         if(sector.special & 2048)
            special |= 16384;
         break;
      }
   }
}

//
// UDMF level maker, resulted from input Doom level and extra Data
//
UDMFLevel::UDMFLevel(const DoomLevel &level, const ExtraData &extraData) : mExtraData(extraData)
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

   mSides.reserve(level.GetSidedefs().size());
   for(const Sidedef &sidedef : level.GetSidedefs())
      mSides.emplace_back(sidedef);

   mLines.reserve(level.GetLinedefs().size());
   for(const Linedef &linedef : level.GetLinedefs())
      mLines.emplace_back(linedef, *this);
}

//
// Interface methods
//
void UDMFLevel::SetLightTag(int special, int tag, UDMFLine &line)
{
   switch (line.special)
   {
      case Door_LockedRaise:
         line.arg[4] = tag;
         break;
      case Door_Open:
         line.arg[2] = tag;
         break;
      case Door_Raise:
         line.arg[3] = tag;
         break;
      default:
         break;
   }
}
void UDMFLevel::SetSurfaceControl(int special, int tag, UDMFLine &line)
{
   UDMFSector *sector = GetFrontSector(line);
   if(!sector)
      return;

   switch (special)
   {
      case EV_STATIC_ATTACH_SET_CEILING_CONTROL:
         sector->ceilingid = tag;
         break;
      case EV_STATIC_ATTACH_SET_FLOOR_CONTROL:
         sector->floorid = tag;
         break;
      default:
         break;
   }
}
void UDMFLevel::AttachToControl(int special, int tag, UDMFLine &line)
{
   UDMFSector *sector = GetFrontSector(line);
   if(!sector)
      return;

   switch (special)
   {
      case EV_STATIC_ATTACH_CEILING_TO_CONTROL:
         sector->attachceiling = tag;
         break;
      case EV_STATIC_ATTACH_FLOOR_TO_CONTROL:
         sector->attachfloor = tag;
         break;
      case EV_STATIC_ATTACH_MIRROR_CEILING:
         sector->attachceiling = -tag;
         break;
      case EV_STATIC_ATTACH_MIRROR_FLOOR:
         sector->attachfloor = -tag;
         break;
      default:
         break;
   }
}
void UDMFLevel::ResolveLineExtraData(int special, int tag, UDMFLine &line)
{
   const EDLine *edLine = mExtraData.GetLine(tag);
   if(!edLine)
   {
      fprintf(stderr, "Unknown linedef recordnum %d (from vertices %d-%d)\n", tag, line.v[0],
              line.v[1]);
      return;
   }
   line.id = edLine->tag;
   if(edLine->special & FLAG_DOOM_SPECIAL)
   {
      // Here we go again
      int ldspecial = edLine->special & ~FLAG_DOOM_SPECIAL;
      if(ldspecial == special)
      {
         fprintf(stderr, "Illegal recursive ExtraData line special at recordnum %d (vertices %d-%d)\n",
                 tag, line.v[0], line.v[1]);
      }
      else
         line.HandleDoomSpecial(ldspecial, edLine->tag, *this);
   }
   else if(edLine->special == Line_SetIdentification)
      line.id = edLine->args[0];
   else
   {
      line.special = edLine->special;
      memcpy(line.arg, edLine->args, sizeof(line.arg));

      if(edLine->extflags & EX_ML_CROSS && edLine->extflags & EX_ML_PLAYER)
         line.flags |= ULF_PLAYERCROSS;
      if(edLine->extflags & EX_ML_CROSS && edLine->extflags & EX_ML_MONSTER)
         line.flags |= ULF_MONSTERCROSS;
      if(edLine->extflags & EX_ML_CROSS && edLine->extflags & EX_ML_MISSILE)
         line.flags |= ULF_MISSILECROSS;
      if(edLine->extflags & EX_ML_CROSS && edLine->extflags & EX_ML_POLYOBJECT)
         line.flags |= ULF_POLYCROSS;
      if(edLine->extflags & EX_ML_USE && edLine->extflags & EX_ML_PLAYER)
         line.flags |= ULF_PLAYERUSE;
      if(edLine->extflags & EX_ML_USE && edLine->extflags & EX_ML_MONSTER)
         line.flags |= ULF_MONSTERUSE;
      if(edLine->extflags & EX_ML_IMPACT && edLine->extflags & EX_ML_PLAYER)
         line.flags |= ULF_IMPACT;
      if(edLine->extflags & EX_ML_IMPACT && edLine->extflags & EX_ML_MONSTER)
         line.flags |= ULF_MONSTERSHOOT;
      if(edLine->extflags & EX_ML_PUSH && edLine->extflags & EX_ML_PLAYER)
         line.flags |= ULF_PLAYERPUSH;
      if(edLine->extflags & EX_ML_PUSH && edLine->extflags & EX_ML_MONSTER)
         line.flags |= ULF_MONSTERPUSH;
      if(edLine->extflags & EX_ML_REPEAT)
         line.flags |= ULF_REPEATSPECIAL;
      if(edLine->extflags & EX_ML_1SONLY)
         line.flags |= ULF_FIRSTSIDEONLY;
   }

   if((line.alpha = edLine->alpha) < 1)
      line.renderstyle = "translucent";

   if(edLine->extflags & EX_ML_ADDITIVE)
      line.renderstyle = "add";
   if(edLine->extflags & EX_ML_BLOCKALL)
      line.flags |= ULF_BLOCKEVERYTHING;
   if(edLine->extflags & EX_ML_ZONEBOUNDARY)
      line.flags |= ULF_ZONEBOUNDARY;
   if(edLine->extflags & EX_ML_CLIPMIDTEX)
      line.flags |= ULF_CLIPMIDTEX;
   if(edLine->extflags & EX_ML_3DMTPASSPROJ)
      line.flags |= ULF_MIDTEX3DIMPASSIBLE;
   if(edLine->extflags & EX_ML_LOWERPORTAL)
      line.flags |= ULF_LOWERPORTAL;
   if(edLine->extflags & EX_ML_UPPERPORTAL)
      line.flags |= ULF_UPPERPORTAL;

   line.portal = edLine->portalid;
}
void UDMFLevel::ResolveSectorExtraData(int special, int tag, UDMFLine &line)
{
   UDMFSector *sector = GetFrontSector(line);
   if(!sector)
      return;

   const EDSector *edSector = mExtraData.GetSector(tag);
   if(!edSector)
   {
      fprintf(stderr, "Missing ExtraData sector %d (from map sector %d)\n", tag,
              (int)(sector - &mSectors[0]));
      return;
   }

   unsigned flags = 0;
   if(edSector->hasflags)
      flags = edSector->flags;
   flags |= edSector->flagsadd;
   flags &= ~edSector->flagsrem;

   if(flags & SECF_SECRET)
      sector->flags |= USF_SECRET;
   if(flags & SECF_FRICTION)
      sector->special |= 2048;
   if(flags & SECF_PUSH)
      sector->special |= 4096;
   if(flags & SECF_KILLSOUND)
      sector->special |= 8192;
   if(flags & SECF_KILLMOVESOUND)
      sector->special |= 16384;
   if(flags & SECF_PHASEDLIGHT)
      sector->flags |= USF_PHASEDLIGHT;
   if(flags & SECF_LIGHTSEQUENCE)
      sector->flags |= USF_LIGHTSEQUENCE;
   if(flags & SECF_LIGHTSEQALT)
      sector->flags |= USF_LIGHTSEQALT;

   flags = edSector->flagsrem;
   // Make sure to also remove flags
   if(flags & SECF_SECRET)
   {
      sector->flags &= ~USF_SECRET;
      sector->special &= ~1024;
   }
   if(flags & SECF_FRICTION)
   {
      sector->special &= ~2048;
      sector->friction = -1;
   }
   if(flags & SECF_PUSH)
      sector->special &= ~4096;
   if(flags & SECF_KILLSOUND)
      sector->special &= ~8192;
   if(flags & SECF_KILLMOVESOUND)
      sector->special &= ~16384;
   if(flags & SECF_PHASEDLIGHT)
      sector->flags &= ~USF_PHASEDLIGHT;
   if(flags & SECF_LIGHTSEQUENCE)
      sector->flags &= ~USF_LIGHTSEQUENCE;
   if(flags & SECF_LIGHTSEQALT)
      sector->flags &= ~USF_LIGHTSEQALT;

   sector->damageamount = edSector->damage;
   sector->damageinterval = edSector->damagemask;
   sector->damagetype = edSector->damagetype;

   flags = 0;
   if(edSector->hasdamageflags)
      flags = edSector->damageflags;
   flags |= edSector->damageflagsadd;
   flags &= ~edSector->damageflagsrem;

   if(flags & SDMG_LEAKYSUIT)
      sector->leakiness = 5;
   if(flags & SDMG_IGNORESUIT)
      sector->leakiness = 256;
   if(flags & SDMG_ENDGODMODE)
      sector->flags |= USF_DAMAGE_ENDGODMODE;
   if(flags & SDMG_EXITLEVEL)
      sector->flags |= USF_DAMAGE_EXITLEVEL;
   if(flags & SDMG_TERRAINHIT)
      sector->flags |= USF_DAMAGETERRAINEFFECT;

   sector->xpanningfloor = edSector->floor_xoffs;
   sector->ypanningfloor = edSector->floor_yoffs;
   sector->xpanningceiling = edSector->ceiling_xoffs;
   sector->ypanningceiling = edSector->ceiling_yoffs;
   sector->xscalefloor = edSector->floor_xscale;
   sector->yscalefloor = edSector->floor_yscale;
   sector->xscaleceiling = edSector->ceiling_xscale;
   sector->yscaleceiling = edSector->ceiling_yscale;
   sector->rotationfloor = edSector->floorangle;
   sector->rotationceiling = edSector->ceilingangle;
   sector->colormaptop = edSector->topmap;
   sector->colormapmid = edSector->midmap;
   sector->colormapbottom = edSector->bottommap;
   sector->floorterrain = edSector->floorterrain;
   sector->ceilingterrain = edSector->ceilingterrain;

   if(edSector->f_pflags & PF_DISABLED)
      sector->flags |= USF_PORTAL_FLOOR_DISABLED;
   if(edSector->f_pflags & PF_NORENDER)
      sector->flags |= USF_PORTAL_FLOOR_NORENDER;
   if(edSector->f_pflags & PF_NOPASS)
      sector->flags |= USF_PORTAL_FLOOR_NOPASS;
   if(edSector->f_pflags & PF_BLOCKSOUND)
      sector->flags |= USF_PORTAL_FLOOR_BLOCKSOUND;
   if(edSector->f_pflags & PS_OVERLAY)
   {
      sector->portal_floor_overlaytype = "translucent";
      if(edSector->f_pflags & PS_ADDITIVE)
         sector->portal_floor_overlaytype = "additive";
   }
   if(edSector->f_pflags & PS_USEGLOBALTEX)
      sector->flags |= USF_PORTAL_FLOOR_USEGLOBALTEX;
   if(edSector->f_pflags & PF_ATTACHEDPORTAL)
      sector->flags |= USF_PORTAL_FLOOR_ATTACHED;

   if(edSector->c_pflags & PF_DISABLED)
      sector->flags |= USF_PORTAL_CEIL_DISABLED;
   if(edSector->c_pflags & PF_NORENDER)
      sector->flags |= USF_PORTAL_CEIL_NORENDER;
   if(edSector->c_pflags & PF_NOPASS)
      sector->flags |= USF_PORTAL_CEIL_NOPASS;
   if(edSector->c_pflags & PF_BLOCKSOUND)
      sector->flags |= USF_PORTAL_CEIL_BLOCKSOUND;
   if(edSector->c_pflags & PS_OVERLAY)
   {
      sector->portal_ceil_overlaytype = "translucent";
      if(edSector->c_pflags & PS_ADDITIVE)
         sector->portal_ceil_overlaytype = "additive";
   }
   if(edSector->c_pflags & PS_USEGLOBALTEX)
      sector->flags |= USF_PORTAL_CEIL_USEGLOBALTEX;
   if(edSector->c_pflags & PF_ATTACHEDPORTAL)
      sector->flags |= USF_PORTAL_CEIL_ATTACHED;

   sector->alphafloor = edSector->f_alpha / 255.0;
   if(sector->alphafloor < 0)
      sector->alphafloor = 0;
   else if(sector->alphafloor > 1)
      sector->alphafloor = 1;
   sector->alphaceiling = edSector->c_alpha / 255.0;
   if(sector->alphaceiling < 0)
      sector->alphaceiling = 0;
   else if(sector->alphaceiling > 1)
      sector->alphaceiling = 1;

   sector->portalfloor = edSector->f_portalid;
   sector->portalceiling = edSector->c_portalid;
}
void UDMFLevel::SetLineID(int special, int tag, UDMFLine &line)
{
   // TODO
}
void UDMFLevel::PortalDefine(int special, int tag, UDMFLine &line)
{

}
void UDMFLevel::QuickLinePortal(int special, int tag, UDMFLine &line)
{
   // Classic mode: same tag, different specials
   // UDMF mode: same special, same tag
   line.special = Line_QuickPortal;
   line.id = tag;
}
void UDMFLevel::TranslucentLine(int special, int tag, UDMFLine &line)
{

}

//
// Gets a linedef's front sector, if available
//
UDMFSector *UDMFLevel::GetFrontSector(const UDMFLine &line) 
{
   int frontside = line.sidefront;
   if(frontside < 0 || frontside >= mSides.size())
      return nullptr;
   const UDMFSide &side = mSides[frontside];
   int secnum = side.sector;
   if(secnum < 0 || secnum >= mSectors.size())
      return nullptr;
   return &mSectors[secnum];
}
