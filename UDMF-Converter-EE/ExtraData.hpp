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
   kExtraDataDoomednum = 5004,
   FLAG_DOOM_SPECIAL = 0x80000000,  // if this is set, the extradata line has the old special set
};


// Extended line special flags
// Because these go into a new field used only by parameterized
// line specials, I don't face the issue of running out of line
// flags anytime soon. This provides another full word for
// future expansion.
enum extmlflags_e : unsigned int
{
   EX_ML_CROSS        = 0x00000001, // crossable
   EX_ML_USE          = 0x00000002, // usable
   EX_ML_IMPACT       = 0x00000004, // shootable
   EX_ML_PUSH         = 0x00000008, // reserved for future use
   EX_ML_PLAYER       = 0x00000010, // enabled for players
   EX_ML_MONSTER      = 0x00000020, // enabled for monsters
   EX_ML_MISSILE      = 0x00000040, // enabled for missiles
   EX_ML_REPEAT       = 0x00000080, // can be used multiple times
   EX_ML_1SONLY       = 0x00000100, // only activated from first side
   EX_ML_ADDITIVE     = 0x00000200, // uses additive blending
   EX_ML_BLOCKALL     = 0x00000400, // line blocks everything
   EX_ML_ZONEBOUNDARY = 0x00000800, // line marks a sound zone boundary
   EX_ML_CLIPMIDTEX   = 0x00001000, // line clips midtextures to floor and ceiling height
   EX_ML_3DMTPASSPROJ = 0x00002000, // with ML_3DMIDTEX, makes it pass projectiles
   EX_ML_LOWERPORTAL  = 0x00004000, // extends the floor portal of the back sector
   EX_ML_UPPERPORTAL  = 0x00008000, // extends the ceiling portal of the back sector
   EX_ML_POLYOBJECT   = 0x00010000, // enabled for polyobjects
};

// haleyjd 12/28/08: sector flags
enum
{
   // BOOM generalized sector properties
   SECF_SECRET             = 0x00000001,  // bit 7 of generalized special
   SECF_FRICTION           = 0x00000002,  // bit 8 of generalized special
   SECF_PUSH               = 0x00000004,  // bit 9 of generalized special
   SECF_KILLSOUND          = 0x00000008,  // bit A of generalized special
   SECF_KILLMOVESOUND      = 0x00000010,  // bit B of generalized special

   // Hexen phased lighting
   SECF_PHASEDLIGHT        = 0x00000020,  // spawned with sequence start special
   SECF_LIGHTSEQUENCE      = 0x00000040,  // spawned with sequence special
   SECF_LIGHTSEQALT        = 0x00000080,  // spawned with sequence alt special

   // UDMF given
   SECF_FLOORLIGHTABSOLUTE = 0x00000100,  // lightfloor is set absolutely
   SECF_CEILLIGHTABSOLUTE  = 0x00000200,  // lightceiling is set absolutely
};

// haleyjd 12/31/08: sector damage flags
enum
{
   SDMG_LEAKYSUIT  = 0x00000001, // rad suit leaks at random
   SDMG_IGNORESUIT = 0x00000002, // rad suit is ignored entirely
   SDMG_ENDGODMODE = 0x00000004, // turns off god mode if on
   SDMG_EXITLEVEL  = 0x00000008, // exits when player health <= 10
   SDMG_TERRAINHIT = 0x00000010, // damage causes a terrain hit
};

// These are flags used to represent configurable options for portals
typedef enum
{
   // -- Portal behavior flags --
   // Portal is completely disabled
   PF_DISABLED           = 0x001,
   // Portal does not render
   PF_NORENDER           = 0x002,
   // Portal does not allow passage
   PF_NOPASS             = 0x004,
   // Portal does not allow recursive sound to pass through
   PF_BLOCKSOUND         = 0x008,

   // -- Overlay flags --
   // Only used per-surface and indicate various overlay options for the portal
   // Portal has a blended texture overlay (alpha is default)
   PS_OVERLAY            = 0x010,
   // Overlay uses additive blending (must be used with PS_OVERLAY)
   PS_ADDITIVE           = 0x020,
   // Mask for overlay blending flags
   PS_OBLENDFLAGS        = PS_OVERLAY | PS_ADDITIVE,
   // Surface uses the global texture in the portal struct
   PS_USEGLOBALTEX       = 0x040,
   // Mask for all overlay flags
   PS_OVERLAYFLAGS       = PS_OBLENDFLAGS | PS_USEGLOBALTEX,

   // -- State flags --
   // These are only used per-surface and indicate the current state of the portal

   // Portal can be rendered
   PS_VISIBLE            = 0x080,
   // Portal can be passed through
   PS_PASSABLE           = 0x100,
   // Portal allows recursive sound
   PS_PASSSOUND          = 0x200,
   // Mask for state flags
   PS_STATEMASK          = PS_VISIBLE | PS_PASSABLE | PS_PASSSOUND,

   // More flags added along...
   PF_ATTACHEDPORTAL     = 0x400,

   // Mask for the flags portion
   PF_FLAGMASK           = PF_DISABLED | PF_NORENDER | PF_NOPASS | PF_BLOCKSOUND
   | PF_ATTACHEDPORTAL,


   // -- Opactiy --
   // The left-most 8 bits are reserved for the opacity value of the portal overlay
   PO_OPACITYSHIFT       = 24,
   PO_OPACITYMASK        = 0xFF000000,

   // All overlay and blending flags
   PS_BLENDFLAGS         = PS_OVERLAYFLAGS | PO_OPACITYMASK,
} portalflag_e;

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
   const EDLine *GetLine(int recordnum) const
   {
      return Get(mLines, recordnum);
   }
   const EDSector *GetSector(int recordnum) const
   {
      return Get(mSectors, recordnum);
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
