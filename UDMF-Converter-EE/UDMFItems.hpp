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

#ifndef UDMFItems_hpp
#define UDMFItems_hpp

#include <ostream>
#include <string>
#include <vector>
#include "MapItems.h"

class DoomLevel;
class ExtraData;
class LinedefConversion;
class Wad;

enum UDMFThingFlags
{
   UTF_SKILL1 = 1,
   UTF_SKILL2 = 2,
   UTF_SKILL3 = 4,
   UTF_SKILL4 = 8,
   UTF_SKILL5 = 0x10,
   UTF_AMBUSH = 0x20,
   UTF_SINGLE = 0x40,
   UTF_DM = 0x80,
   UTF_COOP = 0x100,
   UTF_FRIEND = 0x200,
   UTF_DORMANT = 0x400,
   UTF_CLASS1 = 0x800,
   UTF_CLASS2 = 0x1000,
   UTF_CLASS3 = 0x2000,
   UTF_STANDING = 0x4000,
   UTF_STRIFEALLY = 0x8000,
   UTF_TRANSLUCENT = 0x10000,
   UTF_INVISIBLE = 0x20000,
};

enum UDMFLineFlags
{
   ULF_BLOCKING = 1,
   ULF_BLOCKMONSTERS = 2,
   ULF_TWOSIDED = 4,
   ULF_DONTPEGTOP = 8,
   ULF_DONTPEGBOTTOM = 0x10,
   ULF_SECRET = 0x20,
   ULF_BLOCKSOUND = 0x40,
   ULF_DONTDRAW = 0x80,
   ULF_MAPPED = 0x100,
   ULF_PASSUSE = 0x200,
   ULF_TRANSLUCENT = 0x400,
   ULF_JUMPOVER = 0x800,
   ULF_BLOCKFLOATERS = 0x1000,
   ULF_PLAYERCROSS = 0x2000,
   ULF_PLAYERUSE = 0x4000,
   ULF_MONSTERCROSS = 0x8000,
   ULF_MONSTERUSE = 0x10000,
   ULF_IMPACT = 0x20000,
   ULF_MONSTERSHOOT = 0x40000,
   ULF_PLAYERPUSH = 0x80000,
   ULF_MONSTERPUSH = 0x100000,
   ULF_MISSILECROSS = 0x200000,
   ULF_REPEATSPECIAL = 0x400000,

   // EE extra
   ULF_POLYCROSS = 0x800000,
   ULF_MIDTEX3D = 0x1000000,
   ULF_FIRSTSIDEONLY = 0x2000000,
   ULF_BLOCKEVERYTHING = 0x4000000,
   ULF_ZONEBOUNDARY = 0x8000000,
   ULF_CLIPMIDTEX = 0x10000000,
   ULF_MIDTEX3DIMPASSIBLE = 0x20000000,
   ULF_LOWERPORTAL = 0x40000000,
   ULF_UPPERPORTAL = 0x80000000
};

enum UDMFSectorFlags
{
   // EE extra
   USF_SECRET = 1,
   USF_DAMAGE_ENDGODMODE = 2,
   USF_DAMAGE_EXITLEVEL = 4,
   USF_DAMAGETERRAINEFFECT = 8,
   USF_LIGHTFLOORABSOLUTE = 0x10,
   USF_LIGHTCEILINGABSOLUTE = 0x20,
   USF_PORTAL_FLOOR_DISABLED = 0x40,
   USF_PORTAL_FLOOR_NORENDER = 0x80,
   USF_PORTAL_FLOOR_NOPASS = 0x100,
   USF_PORTAL_FLOOR_BLOCKSOUND = 0x200,
   USF_PORTAL_FLOOR_USEGLOBALTEX = 0x400,
   USF_PORTAL_FLOOR_ATTACHED = 0x800,
   USF_PORTAL_CEIL_DISABLED = 0x1000,
   USF_PORTAL_CEIL_NORENDER = 0x2000,
   USF_PORTAL_CEIL_NOPASS = 0x4000,
   USF_PORTAL_CEIL_BLOCKSOUND = 0x8000,
   USF_PORTAL_CEIL_USEGLOBALTEX = 0x10000,
   USF_PORTAL_CEIL_ATTACHED = 0x20000,
   USF_PHASEDLIGHT = 0x40000,
   USF_LIGHTSEQUENCE = 0x80000,
   USF_LIGHTSEQALT = 0x100000
};

//
// UDMF vertex
//
struct UDMFVertex
{
   double x, y;

   UDMFVertex(const Vertex &v) : x(v.x), y(v.y)
   {
   }

   void WriteToStream(std::ostream &os, int index) const;
};

//
// UDMF thing
//
struct UDMFThing
{
   int id;
   double x, y;
   double height;
   int angle;
   int type;
   unsigned flags;
   int special;
   int arg[5];

   // EE extra
   double health;

   UDMFThing(const Thing &thing, const ExtraData &extraData);

   void WriteToStream(std::ostream &os, int index) const;

private:
   void SetUDMFFlagsFromDoomFlags(unsigned thflags);
};

//
// UDMF line
//
struct UDMFLine
{
   UDMFLine(const Linedef &linedef, LinedefConversion &conversion);

   void HandleDoomSpecial(int special, int tag, LinedefConversion &conversion);

   void WriteToStream(std::ostream &os, int index) const;

   int id;
   int v[2];
   unsigned flags;
   int special;
   int arg[5];
   int sidefront;
   int sideback;

   // EE extra
   int portal;
   double alpha;
   std::string renderstyle;
   std::string tranmap;
};

//
// UDMF side
//
struct UDMFSide
{
   double offsetx;
   double offsety;
   std::string texturetop;
   std::string texturebottom;
   std::string texturemiddle;
   int sector;

   UDMFSide(const Sidedef &side) :
   offsetx(side.xoffset),
   offsety(side.yoffset),
   texturetop(side.upperpic),
   texturebottom(side.lowerpic),
   texturemiddle(side.midpic),
   sector(side.sector)
   {
   }

   void WriteToStream(std::ostream &os, int index) const;
};

//
// UDMF sector
//
struct UDMFSector
{
   UDMFSector(const Sector &sector);

   void WriteToStream(std::ostream &os, int index) const;

   double heightfloor;
   double heightceiling;
   std::string texturefloor;
   std::string textureceiling;
   int lightlevel;
   int special;
   int id;

   // EE extra
   double xpanningfloor;
   double ypanningfloor;
   double xpanningceiling;
   double ypanningceiling;
   double xscalefloor;
   double yscalefloor;
   double xscaleceiling;
   double yscaleceiling;
   double rotationfloor;
   double rotationceiling;
   unsigned flags;
   int friction;
   int leakiness;
   int damageamount;
   int damageinterval;
   std::string damagetype;
   std::string floorterrain;
   std::string ceilingterrain;
   int lightfloor;
   int lightceiling;
   std::string colormaptop;
   std::string colormapmid;
   std::string colormapbottom;
   double scroll_ceil_x;
   double scroll_ceil_y;
   std::string scroll_ceil_type;
   double scroll_floor_x;
   double scroll_floor_y;
   std::string scroll_floor_type;
   int floorid;
   int ceilingid;
   int attachfloor;
   int attachceiling;
   std::string soundsequence;
   int portalfloor;
   int portalceiling;
   std::string portal_floor_overlaytype;
   double alphafloor;
   std::string portal_ceil_overlaytype;
   double alphaceiling;
};

//
// Interface for linedef conversion
//
class LinedefConversion
{
public:
   virtual void SetLightTag(int special, int tag, UDMFLine &line) = 0;
   virtual void SetSurfaceControl(int special, int tag, UDMFLine &line) = 0;
   virtual void AttachToControl(int special, int tag, UDMFLine &line) = 0;
   virtual void ResolveLineExtraData(int special, int tag, UDMFLine &line) = 0;
   virtual void ResolveSectorExtraData(int special, int tag, UDMFLine &line) = 0;
   virtual void SetLineID(int special, int tag, UDMFLine &line) = 0;
   virtual void PortalDefine(int special, int tag, UDMFLine &line) = 0;
   virtual void QuickLinePortal(int special, int tag, UDMFLine &line) = 0;
   virtual void TranslucentLine(int special, int tag, UDMFLine &line) = 0;
};

enum class PortalKind : int
{
   plane = 0,
   horizon = 1,
   skybox = 2,
   anchored = 3,
   twoway = 4,
   linked = 5
};

//
// The entire UDMF level
//
class UDMFLevel : public LinedefConversion
{
public:
   UDMFLevel(const DoomLevel &level, const ExtraData &extraData);

   virtual void SetLightTag(int special, int tag, UDMFLine &line) override;
   virtual void SetSurfaceControl(int special, int tag, UDMFLine &line) override;
   virtual void AttachToControl(int special, int tag, UDMFLine &line) override;
   virtual void ResolveLineExtraData(int special, int tag, UDMFLine &line) override;
   virtual void ResolveSectorExtraData(int special, int tag, UDMFLine &line) override;
   virtual void SetLineID(int special, int tag, UDMFLine &line) override;
   virtual void PortalDefine(int special, int tag, UDMFLine &line) override;
   virtual void QuickLinePortal(int special, int tag, UDMFLine &line) override;
   virtual void TranslucentLine(int special, int tag, UDMFLine &line) override;

   friend std::ostream &operator << (std::ostream &os, const UDMFLevel &level);

private:

   struct AnchoredPortal
   {
      int id;
      PortalKind kind;
      double dx;
      double dy;

      bool operator == (const AnchoredPortal &other) const
      {
         return kind == other.kind && dx == other.dx && dy == other.dy;
      }
      bool IsOpposite(const AnchoredPortal &other) const
      {
         return kind == other.kind && dx == -other.dx && dy == -other.dy;
      }
   };

   struct DeferredLineSetup
   {
      int index;
      int tag;
      int special;
      int arg[5];
      int portal;
   };

   int IndexOf(const UDMFLine &line) const
   {
      return int(&line - &mLines[0]);
   }
   int IndexOf(const UDMFSector &sector) const
   {
      return int(&sector - &mSectors[0]);
   }

   UDMFSector *GetFrontSector(const UDMFLine &line);
   const UDMFVertex *GetVertex(int index) const;
   int MakeNextPortalID()
   {
      return mNextPortalID++;
   }

   const ExtraData &mExtraData;

   std::vector<UDMFThing> mThings;
   std::vector<UDMFVertex> mVertices;
   std::vector<UDMFSector> mSectors;
   std::vector<UDMFSide> mSides;
   std::vector<UDMFLine> mLines;

   const DoomLevel &mDoomLevel;
   int mNextPortalID = 1;

   std::vector<AnchoredPortal> mPortals;
   std::vector<DeferredLineSetup> mDeferredLines;
};

std::ostream &operator << (std::ostream &os, const UDMFLevel &level);

#endif /* UDMFItems_hpp */
