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
// Code also taken from Eternity engine by Quasar
//

#include "Confuse/confuse.h"
#include "ExtraData.hpp"
#include "Helpers.hpp"
#include "LineSpecialMapping.hpp"
#include "MapItems.h"
#include "Wad.hpp"

// ExtraData section names
#define SEC_MAPTHING "mapthing"
#define SEC_LINEDEF  "linedef"
#define SEC_SECTOR   "sector"

// ExtraData field names
// mapthing fields:
#define FIELD_NUM     "recordnum"
#define FIELD_TID     "tid"
#define FIELD_TYPE    "type"
#define FIELD_OPTIONS "options"
#define FIELD_ARGS    "args"
#define FIELD_HEIGHT  "height"
#define FIELD_SPECIAL "special"

// linedef fields:
#define FIELD_LINE_NUM       "recordnum"
#define FIELD_LINE_PORTALID  "portalid"
#define FIELD_LINE_SPECIAL   "special"
#define FIELD_LINE_TAG       "tag"
#define FIELD_LINE_EXTFLAGS  "extflags"
#define FIELD_LINE_ARGS      "args"
#define FIELD_LINE_ID        "id"
#define FIELD_LINE_ALPHA     "alpha"

// sector fields:
#define FIELD_SECTOR_NUM            "recordnum"
#define FIELD_SECTOR_FLAGS          "flags"
#define FIELD_SECTOR_FLAGSADD       "flags.add"
#define FIELD_SECTOR_FLAGSREM       "flags.remove"
#define FIELD_SECTOR_DAMAGE         "damage"
#define FIELD_SECTOR_DAMAGEMASK     "damagemask"
#define FIELD_SECTOR_DAMAGEMOD      "damagemod"
#define FIELD_SECTOR_DAMAGEFLAGS    "damageflags"
#define FIELD_SECTOR_DMGFLAGSADD    "damageflags.add"
#define FIELD_SECTOR_DMGFLAGSREM    "damageflags.remove"
#define FIELD_SECTOR_FLOORTERRAIN   "floorterrain"
#define FIELD_SECTOR_FLOORANGLE     "floorangle"
#define FIELD_SECTOR_FLOOROFFSETX   "flooroffsetx"
#define FIELD_SECTOR_FLOOROFFSETY   "flooroffsety"
#define FIELD_SECTOR_FLOORSCALEX    "floorscalex"
#define FIELD_SECTOR_FLOORSCALEY    "floorscaley"
#define FIELD_SECTOR_CEILINGTERRAIN "ceilingterrain"
#define FIELD_SECTOR_CEILINGANGLE   "ceilingangle"
#define FIELD_SECTOR_CEILINGOFFSETX "ceilingoffsetx"
#define FIELD_SECTOR_CEILINGOFFSETY "ceilingoffsety"
#define FIELD_SECTOR_CEILINGSCALEX  "ceilingscalex"
#define FIELD_SECTOR_CEILINGSCALEY  "ceilingscaley"
#define FIELD_SECTOR_TOPMAP         "colormaptop"
#define FIELD_SECTOR_MIDMAP         "colormapmid"
#define FIELD_SECTOR_BOTTOMMAP      "colormapbottom"
#define FIELD_SECTOR_PORTALFLAGS_F  "portalflags.floor"
#define FIELD_SECTOR_PORTALFLAGS_C  "portalflags.ceiling"
#define FIELD_SECTOR_OVERLAYALPHA_F "overlayalpha.floor"
#define FIELD_SECTOR_OVERLAYALPHA_C "overlayalpha.ceiling"
#define FIELD_SECTOR_PORTALID_F     "portalid.floor"
#define FIELD_SECTOR_PORTALID_C     "portalid.ceiling"

#define MAXFLAGFIELDS 4

enum
{
   FLAG_DOOM_SPECIAL = 0x80000000,  // if this is set, the extradata line has the old special set
};

enum
{
   DEHFLAGS_MODE1,
   DEHFLAGS_MODE2,
   DEHFLAGS_MODE3,
   DEHFLAGS_MODE4,
   DEHFLAGS_MODE_ALL
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

// haleyjd: flag field parsing stuff is now global for EDF and
// ExtraData usage
struct dehflags_t
{
   const char *name;
   unsigned int value;
   int index;
};
struct dehflagset_t
{
   dehflags_t *flaglist;
   int mode;
   unsigned int results[MAXFLAGFIELDS];
};

// mapthing flag values and mnemonics

static dehflags_t mapthingflags[] =
{
   { "EASY",      TF_EASY },
   { "NORMAL",    TF_NORMAL },
   { "HARD",      TF_HARD },
   { "AMBUSH",    TF_AMBUSH },
   { "NOTSINGLE", TF_NOTSINGLE },
   { "NOTDM",     TF_NOTDM },
   { "NOTCOOP",   TF_NOTCOOP },
   { "FRIEND",    TF_FRIEND },
   { "DORMANT",   TF_DORMANT },
   { NULL,        0 }
};

static dehflagset_t mt_flagset =
{
   mapthingflags, // flaglist
   0,             // mode
};

// linedef extended flag values and mnemonics

static dehflags_t extlineflags[] =
{
   { "CROSS",        EX_ML_CROSS        },
   { "USE",          EX_ML_USE          },
   { "IMPACT",       EX_ML_IMPACT       },
   { "PUSH",         EX_ML_PUSH         },
   { "PLAYER",       EX_ML_PLAYER       },
   { "MONSTER",      EX_ML_MONSTER      },
   { "MISSILE",      EX_ML_MISSILE      },
   { "REPEAT",       EX_ML_REPEAT       },
   { "1SONLY",       EX_ML_1SONLY       },
   { "ADDITIVE",     EX_ML_ADDITIVE     },
   { "BLOCKALL",     EX_ML_BLOCKALL     },
   { "ZONEBOUNDARY", EX_ML_ZONEBOUNDARY },
   { "CLIPMIDTEX",   EX_ML_CLIPMIDTEX   },
   { "LOWERPORTAL",  EX_ML_LOWERPORTAL  },
   { "UPPERPORTAL",  EX_ML_UPPERPORTAL  },
   { "POLYOBJECT",   EX_ML_POLYOBJECT   },
   { NULL,           0                  }
};

static dehflagset_t ld_flagset =
{
   extlineflags, // flaglist
   0,            // mode
};

// mapthing options and related data structures

static cfg_opt_t mapthing_opts[] =
{
   CFG_INT(FIELD_NUM,     0,  CFGF_NONE),
   CFG_INT(FIELD_TID,     0,  CFGF_NONE),
   CFG_STR(FIELD_TYPE,    "", CFGF_NONE),
   CFG_STR(FIELD_OPTIONS, "", CFGF_NONE),
   CFG_STR(FIELD_ARGS,    0,  CFGF_LIST),
   CFG_INT(FIELD_HEIGHT,  0,  CFGF_NONE),
   CFG_INT(FIELD_SPECIAL, 0,  CFGF_NONE),
   CFG_END()
};

static dehflags_t sectorflags[] =
{
   { "SECRET",        SECF_SECRET        },
   { "FRICTION",      SECF_FRICTION      },
   { "PUSH",          SECF_PUSH          },
   { "KILLSOUND",     SECF_KILLSOUND     },
   { "KILLMOVESOUND", SECF_KILLMOVESOUND },
   { "PHASEDLIGHT",   SECF_PHASEDLIGHT   },
   { "LIGHTSEQUENCE", SECF_LIGHTSEQUENCE },
   { "LIGHTSEQALT",   SECF_LIGHTSEQALT   },
   { NULL,            0                  }
};

static dehflagset_t sector_flagset =
{
   sectorflags, // flaglist
   0,           // mode
};

static dehflags_t sectordamageflags[] =
{
   { "LEAKYSUIT",  SDMG_LEAKYSUIT  },
   { "IGNORESUIT", SDMG_IGNORESUIT },
   { "ENDGODMODE", SDMG_ENDGODMODE },
   { "EXITLEVEL",  SDMG_EXITLEVEL  },
   { "TERRAINHIT", SDMG_TERRAINHIT },
   { NULL,         0               }
};

static dehflagset_t sectordamage_flagset =
{
   sectordamageflags, // flaglist
   0                  // mode
};

static dehflags_t sectorportalflags[] =
{
   { "DISABLED",     PF_DISABLED     },
   { "NORENDER",     PF_NORENDER     },
   { "NOPASS",       PF_NOPASS       },
   { "BLOCKSOUND",   PF_BLOCKSOUND   },
   { "OVERLAY",      PS_OVERLAY      },
   { "ADDITIVE",     PS_ADDITIVE     },
   { "USEGLOBALTEX", PS_USEGLOBALTEX },
   { "ATTACHEDPORTAL", PF_ATTACHEDPORTAL },
   { NULL,           0               }
};

static dehflagset_t sectorportal_flagset =
{
   sectorportalflags, // flaglist
   0                  // mode
};

// linedef options and related data structures

//
// E_LineSpecForName
//
// Gets a line special for a name.
//
int E_LineSpecForName(const char *name)
{
   int special = 0;

//   // check static inits first
//   if(!(special = EV_SpecialForStaticInitName(name)))
//   {
//      // check normal line types
//      ev_binding_t *binding = EV_BindingForName(name);
//      if(binding)
//         special = binding->actionNumber;
//   }

   return special;
}

//
// E_LineSpecCB
//
// libConfuse value-parsing callback function for the linedef special
// field. This function can resolve normal and generalized special
// names and accepts special numbers, too.
//
// Normal and parameterized specials are stored in the exlinespecs
// hash table and are resolved via name lookup.
//
// Generalized specials use a functional syntax which is parsed by
// E_ProcessGenSpec and E_GenTokenizer above. It is rather complicated
// and probably not that useful, but is provided for completeness.
//
// Raw special numbers are not checked for validity, but invalid
// names are turned into zero specials. Malformed generalized specials
// will cause errors, but bad argument values are zeroed.
//
static int E_LineSpecCB(cfg_t *cfg, cfg_opt_t *opt, const char *value,
                        void *result)
{
   int  num;
   char *endptr;

   num = static_cast<int>(strtol(value, &endptr, 0));

   // check if value is a number or not
   if(*endptr != '\0')
   {
      // value is a special name

      // NOTE: don't support generalized stuff
      *static_cast<int *>(result) = static_cast<int>(GetSpecialByName(value));
   }
   else
   {
      // value is a number
      if(errno == ERANGE)
      {
         if(cfg)
         {
            cfg_error(cfg,
                      "integer value for option '%s' is out of range\n",
                      opt->name);
         }
         return -1;
      }

      *(int *)result = num | FLAG_DOOM_SPECIAL;
   }

   return 0;
}

static cfg_opt_t linedef_opts[] =
{
   CFG_INT(FIELD_LINE_NUM,         0, CFGF_NONE),
   CFG_INT_CB(FIELD_LINE_SPECIAL,  0, CFGF_NONE, E_LineSpecCB),
   CFG_INT(FIELD_LINE_TAG,         0, CFGF_NONE),
   CFG_STR(FIELD_LINE_EXTFLAGS,   "", CFGF_NONE),
   CFG_STR(FIELD_LINE_ARGS,        0, CFGF_LIST),
   CFG_INT(FIELD_LINE_ID,         -1, CFGF_NONE),
   CFG_FLOAT(FIELD_LINE_ALPHA,   1.0, CFGF_NONE),
   CFG_INT(FIELD_LINE_PORTALID,    0, CFGF_NONE),
   CFG_END()
};

//
// E_TranslucCB2
//
// libConfuse value-parsing callback for translucency fields. Can accept
// an integer value or a percentage. This one expects an integer from 0
// to 255 when percentages are not used, and does not convert to fixed point.
//
static int E_TranslucCB2(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
   char *endptr;
   const char *pctloc;

   // test for a percent sign (start looking at end)
   pctloc = strrchr(value, '%');

   if(pctloc)
   {
      int pctvalue;

      // get the percentage value (base 10 only)
      pctvalue = static_cast<int>(strtol(value, &endptr, 10));

      // strtol should stop at the percentage sign
      if(endptr != pctloc)
      {
         if(cfg)
         {
            cfg_error(cfg, "invalid percentage value for option '%s'\n",
                      opt->name);
         }
         return -1;
      }
      if(errno == ERANGE || pctvalue < 0 || pctvalue > 100)
      {
         if(cfg)
         {
            cfg_error(cfg,
                      "percentage value for option '%s' is out of range\n",
                      opt->name);
         }
         return -1;
      }

      *(int *)result = (255 * pctvalue) / 100;
   }
   else
   {
      // process an integer
      *(int *)result = (int)strtol(value, &endptr, 0);

      if(*endptr != '\0')
      {
         if(cfg)
            cfg_error(cfg, "invalid integer value for option '%s'\n", opt->name);
         return -1;
      }
      if(errno == ERANGE)
      {
         if(cfg)
         {
            cfg_error(cfg,
                      "integer value for option '%s' is out of range\n",
                      opt->name);
         }
         return -1;
      }
   }

   return 0;
}

// haleyjd 06/26/07: sector options and related data structures

static cfg_opt_t sector_opts[] =
{
   CFG_INT(FIELD_SECTOR_NUM,              0,          CFGF_NONE),
   CFG_STR(FIELD_SECTOR_FLAGS,            "",         CFGF_NONE),
   CFG_STR(FIELD_SECTOR_FLAGSADD,         "",         CFGF_NONE),
   CFG_STR(FIELD_SECTOR_FLAGSREM,         "",         CFGF_NONE),
   CFG_INT(FIELD_SECTOR_DAMAGE,           0,          CFGF_NONE),
   CFG_INT(FIELD_SECTOR_DAMAGEMASK,       0,          CFGF_NONE),
   CFG_STR(FIELD_SECTOR_DAMAGEMOD,        "Unknown",  CFGF_NONE),
   CFG_STR(FIELD_SECTOR_DAMAGEFLAGS,      "",         CFGF_NONE),
   CFG_STR(FIELD_SECTOR_DMGFLAGSADD,      "",         CFGF_NONE),
   CFG_STR(FIELD_SECTOR_DMGFLAGSREM,      "",         CFGF_NONE),
   CFG_STR(FIELD_SECTOR_FLOORTERRAIN,     "@flat",    CFGF_NONE),
   CFG_STR(FIELD_SECTOR_CEILINGTERRAIN,   "@flat",    CFGF_NONE),
   CFG_STR(FIELD_SECTOR_TOPMAP,           "@default", CFGF_NONE),
   CFG_STR(FIELD_SECTOR_MIDMAP,           "@default", CFGF_NONE),
   CFG_STR(FIELD_SECTOR_BOTTOMMAP,        "@default", CFGF_NONE),

   CFG_FLOAT(FIELD_SECTOR_FLOOROFFSETX,   0.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_FLOOROFFSETY,   0.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_CEILINGOFFSETX, 0.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_CEILINGOFFSETY, 0.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_FLOORSCALEX,    1.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_FLOORSCALEY,    1.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_CEILINGSCALEX,  1.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_CEILINGSCALEY,  1.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_FLOORANGLE,     0.0,        CFGF_NONE),
   CFG_FLOAT(FIELD_SECTOR_CEILINGANGLE,   0.0,        CFGF_NONE),

   // haleyjd 01/08/11: portal properties
   CFG_STR(FIELD_SECTOR_PORTALFLAGS_F,     "",        CFGF_NONE),
   CFG_STR(FIELD_SECTOR_PORTALFLAGS_C,     "",        CFGF_NONE),
   CFG_INT_CB(FIELD_SECTOR_OVERLAYALPHA_F, 255,       CFGF_NONE, E_TranslucCB2),
   CFG_INT_CB(FIELD_SECTOR_OVERLAYALPHA_C, 255,       CFGF_NONE, E_TranslucCB2),
   CFG_INT(FIELD_SECTOR_PORTALID_F,        0,         CFGF_NONE),
   CFG_INT(FIELD_SECTOR_PORTALID_C,        0,         CFGF_NONE),

   CFG_END()
};

// primary ExtraData options table

static cfg_opt_t ed_opts[] =
{
   CFG_SEC(SEC_MAPTHING, mapthing_opts, CFGF_MULTI|CFGF_NOCASE),
   CFG_SEC(SEC_LINEDEF,  linedef_opts,  CFGF_MULTI|CFGF_NOCASE),
   CFG_SEC(SEC_SECTOR,   sector_opts,   CFGF_MULTI|CFGF_NOCASE),
   CFG_END()
};

//
// Called when an EDF error arises. Should throw something.
//
static void OnError(cfg_t *cfg, const char *fmt, va_list ap)
{
   vfprintf(stderr, fmt, ap);
   throw EXIT_FAILURE;
}

//
// Load a lump for ExtraData
//
bool ExtraData::LoadLump(const Wad &wad, const char *name)
{
   cfg_t *cfg = nullptr;
   try
   {
      int index = 0;
      const Lump *lump = wad.FindLump(name, &index);
      if(!lump)
      {
         fprintf(stderr, "Couldn't find ExtraData lump %s\n", name);
         Clear();
         return false;
      }

      cfg = cfg_init(ed_opts, CFGF_NOCASE);
      cfg_set_error_function(cfg, OnError);

      int result = cfg_parselump(cfg, wad, name, index);
      if(result != CFG_SUCCESS)
      {
         fprintf(stderr, "Couldn't parse ExtraData lump %s: error %d\n", name, result);
         cfg_free(cfg);
         Clear();
         return false;
      }

      if(!ProcessThings(cfg))
      {
         fprintf(stderr, "Couldn't process things from ExtraData %s\n", name);
         cfg_free(cfg);
         Clear();
         return false;
      }

      if(!ProcessLines(cfg))
      {
         fprintf(stderr, "Couldn't process linedefs from ExtraData %s\n", name);
         cfg_free(cfg);
         Clear();
         return false;
      }

      if(!ProcessSectors(cfg))
      {
         fprintf(stderr, "Couldn't process sectors from ExtraData %s\n", name);
         cfg_free(cfg);
         Clear();
         return false;
      }

      cfg_free(cfg);
      return true;
   }
   catch(int result)
   {
      fprintf(stderr, "An error occurred, quitting ExtraData processing for %s\n", name);
      cfg_free(cfg);
      Clear();
      return false;
   }
}

//
// From Eternity:
// Returns the result of strchr called on the string in value.
// If the return is non-NULL, you can expect to find the extracted
// prefix written into prefixbuf. If the return value is NULL,
// prefixbuf is unmodified.
//
static const char *ExtractPrefix(const char *value, char *prefixbuf, int buflen)
{
   const char *colonloc = strchr(value, ':');
   if(!colonloc)
      return nullptr;
   const char *strval = colonloc + 1;
   const char *rover = value;
   memset(prefixbuf, 0, buflen);
   int i = 0;
   while(rover != colonloc && i < buflen - 1)
   {
      prefixbuf[i] = *rover++;
      ++i;
   }
   // check validity of the string value location (could be end)
   if(!(*strval))
   {
      fprintf(stderr, "ExtractPrefix: invalid prefix:value %s\n", value);
      throw EXIT_FAILURE;  // dunno what else to do, just kill it off
   }
   return colonloc;
}

//
// From Eternity:
// Parses thing type fields in ExtraData. Allows resolving of
// EDF thingtype mnemonics to their corresponding doomednums.
//
static int ParseTypeField(const char *value)
{
   char *numpos = nullptr;
   long num = strtol(value, &numpos, 0);

   char prefix[16] = {};
   const char *colonloc = ExtractPrefix(value, prefix, sizeof(prefix));
   if(colonloc || (numpos && *numpos))
   {
      const char *strval;
      if(colonloc)
         strval = colonloc + 1;
      else
         strval = value;

      // TODO: find doomednum
      fprintf(stderr, "Unknown thing type %s\n", strval);
      return 0;
   }
   if(num < 0)
      num = 0;
   return static_cast<int>(num);
}

// ============================================================

//
// davidph 01/14/14: split from deh_ParseFlags
//
static dehflags_t *deh_ParseFlag(dehflagset_t &flagset, const char *name)
{
   int mode = flagset.mode;

   for(dehflags_t *flag = flagset.flaglist; flag->name; ++flag)
   {
      if(!strcasecmp(name, flag->name) &&
         (flag->index == mode || mode == DEHFLAGS_MODE_ALL))
      {
         return flag;
      }
   }

   return NULL;
}

// deh_ParseFlags
// Purpose: Handle thing flag fields in a general manner
// Args:    flagset -- pointer to a dehflagset_t object
//          strval  -- ptr-to-ptr to string containing flags
//                     Note: MUST be a mutable string pointer!
// Returns: Nothing. Results for each parsing mode are written
//          into the corresponding index of the results array
//          within the flagset object.
//
// haleyjd 11/03/02: generalized from code that was previously below
// haleyjd 04/10/03: made global for use in EDF and ExtraData
// haleyjd 02/19/04: rewrote for combined flags support
//
static void deh_ParseFlags(dehflagset_t &flagset, char **strval)
{
   unsigned int *results  = flagset.results;  // pointer to results array

   // haleyjd: init all results to zero
   memset(results, 0, MAXFLAGFIELDS * sizeof(*results));

   // killough 10/98: replace '+' kludge with strtok() loop
   // Fix error-handling case ('found' var wasn't being reset)
   //
   // Use OR logic instead of addition, to allow repetition

   for(;(*strval = strtok(*strval, ",+| \t\f\r")); *strval = NULL)
   {
      dehflags_t *flag = deh_ParseFlag(flagset, *strval);

      if(flag)
         results[flag->index] |= flag->value;
      else
         fprintf(stderr, "Could not find flag %s\n", *strval);
   }
}

//
// Parses EDF syntax flags
// From Eternity
//
static unsigned ParseFlags(const char *str, dehflagset_t &flagset)
{
   char *buffer, *bufptr;
   bufptr = buffer = strdup(str);
   deh_ParseFlags(flagset, &bufptr);
   free(buffer);
   return flagset.results[0];
}

//
// Parse ExtraData thing args
//
static void ParseArgs(int *args, int numArgs, cfg_t *sec, const char *fieldName)
{
   unsigned numargs = cfg_size(sec, fieldName);
   memset(args, 0, sizeof(int) * numArgs);
   for(unsigned i = 0; i < numargs && i < numArgs; ++i)
      args[i] = cfg_getnint(sec, fieldName, i);
}

//
// Load things
//
bool ExtraData::ProcessThings(cfg_t *cfg)
{
   unsigned size = cfg_size(cfg, SEC_MAPTHING);
   for(unsigned i = 0; i < size; ++i)
   {
      cfg_t *thingsec = cfg_getnsec(cfg, SEC_MAPTHING, i);
      int recordnum = cfg_getint(thingsec, FIELD_NUM);
      if(mThings.find(recordnum) != mThings.end())
      {
         fprintf(stderr, "Error: duplicate mapthing recordnum %d\n", recordnum);
         return false;
      }

      EDThing &thing = mThings[recordnum];
      const char *name = cfg_getstr(thingsec, FIELD_TYPE);
      thing.type = ParseTypeField(name);
      if(thing.type == kExtraDataDoomednum)
         thing.type = 0;   // just remove it
      if(!thing.type)   // don't waste time processing zero-type things
      {
         mThings.erase(recordnum);
         continue;
      }
      const char *opts = cfg_getstr(thingsec, FIELD_OPTIONS);
      if(!*opts)
         thing.options = 0;
      else
         thing.options = ParseFlags(opts, mt_flagset);

      thing.tid = cfg_getint(thingsec, FIELD_TID);
      if(thing.tid < 0)
         thing.tid = 0;

      ParseArgs(thing.args, lengthof(thing.args), thingsec, FIELD_ARGS);
      thing.height = cfg_getint(thingsec, FIELD_HEIGHT);
      thing.special = cfg_getint(thingsec, FIELD_SPECIAL);

   }
   return true;
}

//
// Processes ExtraData linedefs
//
bool ExtraData::ProcessLines(cfg_t *cfg)
{
   unsigned size = cfg_size(cfg, SEC_LINEDEF);
   for(unsigned i = 0; i < size; ++i)
   {
      cfg_t *linesec = cfg_getnsec(cfg, SEC_LINEDEF, i);
      int recordnum = cfg_getint(linesec, FIELD_LINE_NUM);
      if(mLines.find(recordnum) != mLines.end())
      {
         fprintf(stderr, "Error: duplicate linedef recordnum %d\n", recordnum);
         return false;
      }

      EDLine &line = mLines[recordnum];
      line.special = cfg_getint(linesec, FIELD_LINE_SPECIAL);
      line.tag = cfg_getint(linesec, FIELD_LINE_TAG);
      bool tagset = cfg_size(linesec, FIELD_LINE_TAG) > 0;

      const char *flags = cfg_getstr(linesec, FIELD_LINE_EXTFLAGS);
      if(!*flags)
         line.extflags = 0;
      else
         line.extflags = ParseFlags(flags, ld_flagset);

      ParseArgs(line.args, lengthof(line.args), linesec, FIELD_LINE_ARGS);

      if(!tagset)
         line.tag = cfg_getint(linesec, FIELD_LINE_ID);

      line.alpha = cfg_getfloat(linesec, FIELD_LINE_ALPHA);
      if(line.alpha < 0)
         line.alpha = 0;
      else if(line.alpha > 1)
         line.alpha = 1;

      line.portalid = cfg_getint(linesec, FIELD_LINE_PORTALID);
   }

   return true;
}

//
// From Eternity:
// ZDoom decided for us that floors and ceilings should rotate backward with
// respect to DOOM's normal angular coordinate system, so don't blame me for
// the reversal.
// MaxW: 2016/07/11: This is no longer static as the function is needed for UDMF
//
static double NormalizeFlatAngle(double input)
{
   // first account for negative angles
   while(input < 0.0)
      input += 360.0;

   // now account for super angles
   while(input >= 360.0)
      input -= 360.0;

   // now that we're between 0 and 359, reverse the angle...
   input = 360.0 - input;

   // don't return 360 for 0
   if(input == 360.0)
      input = 0.0;

   return input;
}

//
// Process ExtraData sectors
//
bool ExtraData::ProcessSectors(cfg_t *cfg)
{
   unsigned size = cfg_size(cfg, SEC_SECTOR);
   for(unsigned i = 0; i < size; ++i)
   {
      cfg_t *section = cfg_getnsec(cfg, SEC_SECTOR, i);
      int recordnum = cfg_getint(section, FIELD_SECTOR_NUM);
      if(mSectors.find(recordnum) != mSectors.end())
      {
         fprintf(stderr, "Error: duplicate sector recordnum %d\n", recordnum);
         return false;
      }

      EDSector &sector = mSectors[recordnum];
      const char *flags = cfg_getstr(section, FIELD_SECTOR_FLAGS);
      if(*flags)
      {
         sector.hasflags = true;
         sector.flags = ParseFlags(flags, sector_flagset);
      }

      flags = cfg_getstr(section, FIELD_SECTOR_FLAGSADD);
      if(*flags)
         sector.flagsadd = ParseFlags(flags, sector_flagset);

      flags = cfg_getstr(section, FIELD_SECTOR_FLAGSREM);
      if(*flags)
         sector.flagsrem = ParseFlags(flags, sector_flagset);

      sector.damage = cfg_getint(section, FIELD_SECTOR_DAMAGE);
      sector.damagemask = cfg_getint(section, FIELD_SECTOR_DAMAGEMASK);

      sector.damagetype = cfg_getstr(section, FIELD_SECTOR_DAMAGEMOD);

      flags = cfg_getstr(section, FIELD_SECTOR_DAMAGEFLAGS);
      if(*flags)
      {
         sector.hasdamageflags = true;
         sector.damageflags = ParseFlags(flags, sectordamage_flagset);
      }

      flags = cfg_getstr(section, FIELD_SECTOR_DMGFLAGSADD);
      if(*flags)
         sector.damageflagsadd = ParseFlags(flags, sectordamage_flagset);
      flags = cfg_getstr(section, FIELD_SECTOR_DMGFLAGSREM);
      if(*flags)
         sector.damageflagsrem = ParseFlags(flags, sectordamage_flagset);

      sector.floor_xoffs = cfg_getfloat(section, FIELD_SECTOR_FLOOROFFSETX);
      sector.floor_yoffs = cfg_getfloat(section, FIELD_SECTOR_FLOOROFFSETY);
      sector.ceiling_xoffs = cfg_getfloat(section, FIELD_SECTOR_CEILINGOFFSETX);
      sector.ceiling_yoffs = cfg_getfloat(section, FIELD_SECTOR_CEILINGOFFSETY);

      sector.floor_xscale = cfg_getfloat(section, FIELD_SECTOR_FLOORSCALEX);
      sector.floor_yscale = cfg_getfloat(section, FIELD_SECTOR_FLOORSCALEY);
      sector.ceiling_xscale = cfg_getfloat(section, FIELD_SECTOR_CEILINGSCALEX);
      sector.ceiling_yscale = cfg_getfloat(section, FIELD_SECTOR_CEILINGSCALEY);

      sector.floorangle = NormalizeFlatAngle(cfg_getfloat(section, FIELD_SECTOR_FLOORANGLE));
      sector.ceilingangle = NormalizeFlatAngle(cfg_getfloat(section, FIELD_SECTOR_CEILINGANGLE));

      sector.topmap = cfg_getstr(section, FIELD_SECTOR_TOPMAP);
      sector.midmap = cfg_getstr(section, FIELD_SECTOR_MIDMAP);
      sector.bottommap = cfg_getstr(section, FIELD_SECTOR_BOTTOMMAP);

      sector.floorterrain = cfg_getstr(section, FIELD_SECTOR_FLOORTERRAIN);
      sector.ceilingterrain = cfg_getstr(section, FIELD_SECTOR_CEILINGTERRAIN);

      flags = cfg_getstr(section, FIELD_SECTOR_PORTALFLAGS_F);
      if(*flags)
         sector.f_pflags = ParseFlags(flags, sectorportal_flagset);
      flags = cfg_getstr(section, FIELD_SECTOR_PORTALFLAGS_C);
      if(*flags)
         sector.c_pflags = ParseFlags(flags, sectorportal_flagset);

      sector.f_alpha = cfg_getint(section, FIELD_SECTOR_OVERLAYALPHA_F);
      if(sector.f_alpha < 0)
         sector.f_alpha = 0;
      else if(sector.f_alpha > 255)
         sector.f_alpha = 255;

      sector.c_alpha = cfg_getint(section, FIELD_SECTOR_OVERLAYALPHA_C);
      if(sector.c_alpha < 0)
         sector.c_alpha = 0;
      else if(sector.c_alpha > 255)
         sector.c_alpha = 255;

      sector.f_portalid = cfg_getint(section, FIELD_SECTOR_PORTALID_F);
      sector.c_portalid = cfg_getint(section, FIELD_SECTOR_PORTALID_C);
   }
   return true;
}

void ExtraData::Clear()
{
   mThings.clear();
   mLines.clear();
   mSectors.clear();
}
