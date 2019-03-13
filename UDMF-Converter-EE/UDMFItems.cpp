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
#include "Helpers.hpp"
#include "LineSpecialMapping.hpp"
#include "Wad.hpp"

static void Print(FILE *stream, const char *name, int value, int def = 0)
{
   if(value != def)
      fprintf(stream, "%s=%d;\n", name, value);
}
static void Print(FILE *stream, const char *name, double value, double def = 0)
{
   if(value != def)
      fprintf(stream, "%s=%.16g;\n", name, value);
}
static void Print(FILE *stream, const char *name, bool value)
{
   if(value)
      fprintf(stream, "%s=true;\n", name);
}
static void Print(FILE *stream, const char *name, const std::string &value, const char *def = "")
{
   if(value != def)
      fprintf(stream, "%s=\"%s\";\n", name, Escape(value).c_str());
}
static void PrintFlag(FILE *stream, const char *name, unsigned flags, unsigned flag)
{
   if(flags & flag)
      fprintf(stream, "%s=true;\n", name);
}

void UDMFVertex::WriteToStream(FILE *f, int index) const
{
   fprintf(f, "vertex // %d\n{\n", index);
   Print(f, "x", x, NAN);
   Print(f, "y", y, NAN);
   fprintf(f, "}\n");
}

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
   flags = UTF_SINGLE | UTF_DM | UTF_COOP | UTF_CLASS1 | UTF_CLASS2 | UTF_CLASS3;
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

void UDMFThing::WriteToStream(FILE *f, int index) const
{
   fprintf(f, "thing // %d\n{\n", index);
   Print(f, "id", id);
   Print(f, "x", x, NAN);
   Print(f, "y", y, NAN);
   Print(f, "height", height);
   Print(f, "angle", angle);
   Print(f, "type", type, INT_MIN);
   Print(f, "special", special);
   Print(f, "arg0", arg[0]);
   Print(f, "arg1", arg[1]);
   Print(f, "arg2", arg[2]);
   Print(f, "arg3", arg[3]);
   Print(f, "arg4", arg[4]);
   Print(f, "health", health);

   PrintFlag(f, "skill1", flags, UTF_SKILL1);
   PrintFlag(f, "skill2", flags, UTF_SKILL2);
   PrintFlag(f, "skill3", flags, UTF_SKILL3);
   PrintFlag(f, "skill4", flags, UTF_SKILL4);
   PrintFlag(f, "skill5", flags, UTF_SKILL5);
   PrintFlag(f, "ambush", flags, UTF_AMBUSH);
   PrintFlag(f, "single", flags, UTF_SINGLE);
   PrintFlag(f, "dm", flags, UTF_DM);
   PrintFlag(f, "coop", flags, UTF_COOP);
   PrintFlag(f, "friend", flags, UTF_FRIEND);
   PrintFlag(f, "dormant", flags, UTF_DORMANT);
   PrintFlag(f, "class1", flags, UTF_CLASS1);
   PrintFlag(f, "class2", flags, UTF_CLASS2);
   PrintFlag(f, "class3", flags, UTF_CLASS3);
   PrintFlag(f, "standing", flags, UTF_STANDING);
   PrintFlag(f, "strifeally", flags, UTF_STRIFEALLY);
   PrintFlag(f, "translucent", flags, UTF_TRANSLUCENT);
   PrintFlag(f, "invisible", flags, UTF_INVISIBLE);
   fprintf(f, "}\n");
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
// Writes a line to stream
//
void UDMFLine::WriteToStream(FILE *f, int index) const
{
   fprintf(f, "linedef // %d\n{\n", index);
   Print(f, "id", id);  // NOTE: use default of 0
   Print(f, "v1", v[0], INT_MIN);
   Print(f, "v2", v[1], INT_MIN);
   Print(f, "special", special);
   Print(f, "arg0", arg[0]);
   Print(f, "arg1", arg[1]);
   Print(f, "arg2", arg[2]);
   Print(f, "arg3", arg[3]);
   Print(f, "arg4", arg[4]);
   Print(f, "sidefront", sidefront, INT_MIN);
   Print(f, "sideback", sideback, -1);

   Print(f, "portal", portal);
   Print(f, "alpha", alpha, 1.0);
   Print(f, "renderstyle", renderstyle, "");

   PrintFlag(f, "blocking", flags, ULF_BLOCKING);
   PrintFlag(f, "blockmonsters", flags, ULF_BLOCKMONSTERS);
   PrintFlag(f, "twosided", flags, ULF_TWOSIDED);
   PrintFlag(f, "dontpegtop", flags, ULF_DONTPEGTOP);
   PrintFlag(f, "dontpegbottom", flags, ULF_DONTPEGBOTTOM);
   PrintFlag(f, "secret", flags, ULF_SECRET);
   PrintFlag(f, "blocksound", flags, ULF_BLOCKSOUND);
   PrintFlag(f, "dontdraw", flags, ULF_DONTDRAW);
   PrintFlag(f, "mapped", flags, ULF_MAPPED);
   PrintFlag(f, "passuse", flags, ULF_PASSUSE);
   PrintFlag(f, "translucent", flags, ULF_TRANSLUCENT);
   PrintFlag(f, "jumpover", flags, ULF_JUMPOVER);
   PrintFlag(f, "blockfloaters", flags, ULF_BLOCKFLOATERS);
   PrintFlag(f, "playercross", flags, ULF_PLAYERCROSS);
   PrintFlag(f, "playeruse", flags, ULF_PLAYERUSE);
   PrintFlag(f, "monstercross", flags, ULF_MONSTERCROSS);
   PrintFlag(f, "monsteruse", flags, ULF_MONSTERUSE);
   PrintFlag(f, "impact", flags, ULF_IMPACT);
   PrintFlag(f, "monstershoot", flags, ULF_MONSTERSHOOT);
   PrintFlag(f, "playerpush", flags, ULF_PLAYERPUSH);
   PrintFlag(f, "monsterpush", flags, ULF_MONSTERPUSH);
   PrintFlag(f, "missilecross", flags, ULF_MISSILECROSS);
   PrintFlag(f, "repeatspecial", flags, ULF_REPEATSPECIAL);
   PrintFlag(f, "polycross", flags, ULF_POLYCROSS);
   PrintFlag(f, "midtex3d", flags, ULF_MIDTEX3D);
   PrintFlag(f, "firstsideonly", flags, ULF_FIRSTSIDEONLY);
   PrintFlag(f, "blockeverything", flags, ULF_BLOCKEVERYTHING);
   PrintFlag(f, "zoneboundary", flags, ULF_ZONEBOUNDARY);
   PrintFlag(f, "clipmidtex", flags, ULF_CLIPMIDTEX);
   PrintFlag(f, "midtex3dimpassible", flags, ULF_MIDTEX3DIMPASSIBLE);
   PrintFlag(f, "lowerportal", flags, ULF_LOWERPORTAL);
   PrintFlag(f, "upperportal", flags, ULF_UPPERPORTAL);
   fprintf(f, "}\n");
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
UDMFLevel::UDMFLevel(const DoomLevel &level, const ExtraData &extraData) :
mExtraData(extraData),
mDoomLevel(level)
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

   for (const DeferredLineSetup &setup : mDeferredLines)
   {
      mLines[setup.index].id = setup.tag;
      mLines[setup.index].special = setup.special;
      memcpy(mLines[setup.index].arg, setup.arg, sizeof(setup.arg));
      mLines[setup.index].portal = setup.portal;

      printf("Set line %d tag %d special %d args %d %d %d %d %d portal %d\n",
             setup.index, setup.tag, setup.special, setup.arg[0], setup.arg[1], setup.arg[2],
             setup.arg[3], setup.arg[4], setup.portal);
   }
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

   int secnum = IndexOf(*sector);

   switch (special)
   {
      case EV_STATIC_ATTACH_SET_CEILING_CONTROL:
         sector->ceilingid = tag;
         printf("Sector %d gets ceilingid %d\n", secnum, tag);
         break;
      case EV_STATIC_ATTACH_SET_FLOOR_CONTROL:
         sector->floorid = tag;
         printf("Sector %d gets floorid %d\n", secnum, tag);
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

   int secnum = IndexOf(*sector);

   switch (special)
   {
      case EV_STATIC_ATTACH_CEILING_TO_CONTROL:
         sector->attachceiling = tag;
         printf("Sector %d gets attachceiling %d\n", secnum, tag);
         break;
      case EV_STATIC_ATTACH_FLOOR_TO_CONTROL:
         sector->attachfloor = tag;
         printf("Sector %d gets attachfloor %d\n", secnum, tag);
         break;
      case EV_STATIC_ATTACH_MIRROR_CEILING:
         sector->attachceiling = -tag;
         printf("Sector %d gets attachceiling %d\n", secnum, -tag);
         break;
      case EV_STATIC_ATTACH_MIRROR_FLOOR:
         sector->attachfloor = -tag;
         printf("Sector %d gets attachfloor %d\n", secnum, -tag);
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
   line.id = tag;
}

//
// portal info from special
//
struct PortalInfo
{
   PortalKind kind;
   int anchorspec;
   bool floor;
   bool ceiling;

   inline bool IsAnchored() const
   {
      return kind == PortalKind::anchored || kind == PortalKind::twoway || kind == PortalKind::linked;
   }

   PortalInfo(int special) : anchorspec(), floor(), ceiling()
   {
      switch (special)
      {
         case EV_STATIC_PORTAL_ANCHORED_CEILING:
            kind = PortalKind::anchored;
            anchorspec = EV_STATIC_PORTAL_ANCHOR;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_HORIZON_CEILING:
            kind = PortalKind::horizon;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_LINKED_CEILING:
            kind = PortalKind::linked;
            anchorspec = EV_STATIC_PORTAL_LINKED_ANCHOR;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_PLANE_CEILING:
            kind = PortalKind::plane;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_SKYBOX_CEILING:
            kind = PortalKind::skybox;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_TWOWAY_CEILING:
            kind = PortalKind::twoway;
            anchorspec = EV_STATIC_PORTAL_TWOWAY_ANCHOR;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_ANCHORED_CEILING_FLOOR:
            kind = PortalKind::anchored;
            anchorspec = EV_STATIC_PORTAL_ANCHOR;
            floor = true;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_HORIZON_CEILING_FLOOR:
            kind = PortalKind::horizon;
            floor = true;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_PLANE_CEILING_FLOOR:
            kind = PortalKind::plane;
            floor = true;
            ceiling = true;
            break;
         case EV_STATIC_PORTAL_SKYBOX_CEILING_FLOOR:
            kind = PortalKind::skybox;
            ceiling = floor = true;
            break;
         case EV_STATIC_PORTAL_ANCHORED_FLOOR:
            kind = PortalKind::anchored;
            anchorspec = EV_STATIC_PORTAL_ANCHOR_FLOOR;
            floor = true;
            break;
         case EV_STATIC_PORTAL_HORIZON_FLOOR:
            kind = PortalKind::horizon;
            floor = true;
            break;
         case EV_STATIC_PORTAL_LINKED_FLOOR:
            kind = PortalKind::linked;
            anchorspec = EV_STATIC_PORTAL_LINKED_ANCHOR_FLOOR;
            floor = true;
            break;
         case EV_STATIC_PORTAL_PLANE_FLOOR:
            kind = PortalKind::plane;
            floor = true;
            break;
         case EV_STATIC_PORTAL_SKYBOX_FLOOR:
            kind = PortalKind::skybox;
            floor = true;
            break;
         case EV_STATIC_PORTAL_TWOWAY_FLOOR:
            kind = PortalKind::twoway;
            anchorspec = EV_STATIC_PORTAL_TWOWAY_ANCHOR_FLOOR;
            floor = true;
            break;

         default:
            return;
      }
   }
};

//
// All the complex portal_define logic.
//
void UDMFLevel::PortalDefine(int special, int tag, UDMFLine &line)
{
   const PortalInfo info(special);
   int curindex = IndexOf(line);

   printf("Found portal line %d\n", curindex);

   int portalid = 0;
   if (info.IsAnchored())
   {
      // check if already defined

      const UDMFVertex *myv[2] = { GetVertex(line.v[0]), GetVertex(line.v[1]) };
      if(!myv[0] || !myv[1])
      {
         fprintf(stderr, "Line %d is invalid\n", curindex);
         return;
      }

      double myx = (myv[0]->x + myv[1]->x) / 2;
      double myy = (myv[0]->y + myv[1]->y) / 2;

      for (const Linedef &otherline : mDoomLevel.GetLinedefs())
      {
         int otherindex = mDoomLevel.IndexOf(otherline);
         if(curindex == otherindex || otherline.tag != tag || otherline.special != info.anchorspec)
            continue;

         const Vertex *ov[2] = {
            mDoomLevel.GetVertex(otherline.v1),
            mDoomLevel.GetVertex(otherline.v2)
         };
         if(!ov[0] || !ov[1])
            continue;

         // Reverse direction
         double dx = myx - (ov[0]->x + ov[1]->x) / 2.0;
         double dy = myy - (ov[0]->y + ov[1]->y) / 2.0;

         printf("Anchor offset is %g %g\n", dx, dy);

         AnchoredPortal portal = {};
         portal.kind = info.kind;
         portal.dx = dx;
         portal.dy = dy;

         for(const AnchoredPortal &oportal : mPortals)
         {
            if(oportal == portal)
            {
               portalid = oportal.id;
               printf("Line %d reuses portal %d\n", curindex, portalid);
            }
            else if(oportal.IsOpposite(portal))
            {
               portalid = -oportal.id;
               printf("Line %d mirrors portal %d\n", curindex, -portalid);
            }
            if(portalid)
               break;
         }
         // portalid none, so add this
         if(!portalid)
         {
            portalid = portal.id = MakeNextPortalID();
            mPortals.push_back(portal);

            DeferredLineSetup setup = {};
            setup.index = otherindex;
            setup.special = Portal_Define;
            setup.arg[0] = portal.id;
            setup.arg[1] = int(info.kind);
            setup.arg[2] = tag;
            mDeferredLines.push_back(setup);

            line.id = tag;
            printf("Line %d makes new portal %d\n", curindex, portalid);
         }

         break;
      }
   }
   else
   {
      line.special = Portal_Define;
      portalid = line.arg[0] = MakeNextPortalID();
      line.arg[1] = int(info.kind);
   }

   if(!portalid)
      return;
   for(UDMFSector &sector : mSectors)
   {
      if(sector.id == tag)
      {
         if(info.ceiling)
            sector.portalceiling = portalid;
         if(info.floor)
            sector.portalfloor = portalid;
         printf("Sector %d gets floor(%d) or ceiling(%d) portal %d\n", IndexOf(sector), info.floor,
                info.ceiling, portalid);
      }
   }

   for (const Linedef &cline : mDoomLevel.GetLinedefs())
   {
      if(cline.tag != tag)
         continue;
      if(cline.special == EV_STATIC_PORTAL_LINE)
      {
         DeferredLineSetup setup = {};
         setup.index = mDoomLevel.IndexOf(cline);
         setup.portal = portalid;
         mDeferredLines.push_back(setup);
      }
      else if(cline.special == EV_STATIC_PORTAL_APPLY_FRONTSECTOR)
      {
         int secnum = mDoomLevel.GetFrontSectorIndex(cline);
         if(secnum != -1)
         {
            UDMFSector &sector = mSectors[secnum];
            if(info.ceiling)
               sector.portalceiling = portalid;
            if(info.floor)
               sector.portalfloor = portalid;
            printf("Sector %d copies floor(%d) or ceiling(%d) portal %d\n", IndexOf(sector),
                   info.floor, info.ceiling, portalid);
         }
      }
   }

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
   if(line.sidefront < 0 || line.sidefront >= mSides.size())
      return;
   const UDMFSide &side = mSides[line.sidefront];
   const char *midtex = side.texturemiddle.c_str();
   if(!strcasecmp(midtex, "tranmap"))
      line.tranmap = "TRANMAP";
   else if(mDoomLevel.GetWad())
   {
      // check lump
      const Lump *lump = mDoomLevel.GetWad()->FindLump(midtex);
      if(lump && lump->Data().size() == 65536)
      {
         line.tranmap = midtex;
         printf("Line %d gets tranmap '%s'\n", IndexOf(line), midtex);
      }
      else
         fprintf(stderr, "Line %d FAILS tranmap '%s'\n", IndexOf(line), midtex);
   }
}

//
// Writes to a file its content
//
void UDMFLevel::WriteToStream(FILE *stream) const
{
   fprintf(stream, "namespace=\"eternity\";\n");
   int i = 0;
   for (const UDMFThing &thing : mThings)
      thing.WriteToStream(stream, i++);
   i = 0;
   for (const UDMFVertex &vertex : mVertices)
      vertex.WriteToStream(stream, i++);
   i = 0;
   for (const UDMFLine &line : mLines)
      line.WriteToStream(stream, i++);
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

//
// Returns a vertex if index is valid.
//
const UDMFVertex *UDMFLevel::GetVertex(int index) const
{
   return index >= 0 && index < mVertices.size() ? &mVertices[index] : nullptr;
}
