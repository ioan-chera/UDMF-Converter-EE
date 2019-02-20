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


// linedef options and related data structures

//
// EV_SpecialForStaticInitName
//
// Some static init specials have symbolic names. This will
// return the bound special for such a name if one exists.
//
static int EV_SpecialForStaticInitName(const char *name)
{
   struct staticname_t
   {
      int staticFn;
      const char *name;
   };
   // TODO
//   static staticname_t namedStatics[] =
//   {
//      { EV_STATIC_3DMIDTEX_ATTACH_PARAM,   "Sector_Attach3dMidtex"  },
//      { EV_STATIC_INIT_PARAM,              "Static_Init"            },
//      { EV_STATIC_PORTAL_LINE_PARAM_QUICK, "Line_QuickPortal"       },
//      { EV_STATIC_PORTAL_LINE_PARAM_COMPAT, "Line_SetPortal"        },
//      { EV_STATIC_SLOPE_PARAM,             "Plane_Align"            },
//      { EV_STATIC_SLOPE_PARAM_TAG,         "Plane_Copy"             },
//      { EV_STATIC_POLYOBJ_START_LINE,      "Polyobj_StartLine"      },
//      { EV_STATIC_POLYOBJ_EXPLICIT_LINE,   "Polyobj_ExplicitLine"   },
//      { EV_STATIC_PUSHPULL_CONTROL_PARAM,  "PointPush_SetForce"     },
//      { EV_STATIC_PORTAL_DEFINE,           "Portal_Define"          },
//      { EV_STATIC_SCROLL_BY_OFFSETS,       "Scroll_Texture_Offsets" },
//      { EV_STATIC_SCROLL_CEILING_PARAM,    "Scroll_Ceiling"         },
//      { EV_STATIC_SCROLL_FLOOR_PARAM,      "Scroll_Floor"           },
//      { EV_STATIC_SCROLL_LEFT_PARAM,       "Scroll_Texture_Left"    },
//      { EV_STATIC_SCROLL_WALL_PARAM,       "Scroll_Texture_Model"   },
//      { EV_STATIC_SCROLL_RIGHT_PARAM,      "Scroll_Texture_Right"   },
//      { EV_STATIC_SCROLL_UP_PARAM,         "Scroll_Texture_Up"      },
//      { EV_STATIC_SCROLL_DOWN_PARAM,       "Scroll_Texture_Down"    },
//      { EV_STATIC_CURRENT_CONTROL_PARAM,   "Sector_SetCurrent"      },
//      { EV_STATIC_FRICTION_TRANSFER,       "Sector_SetFriction"     },
//      { EV_STATIC_PORTAL_SECTOR_PARAM_COMPAT, "Sector_SetPortal"    },
//      { EV_STATIC_WIND_CONTROL_PARAM,      "Sector_SetWind"         },
//      { EV_STATIC_PORTAL_HORIZON_LINE,     "Line_Horizon"           },
//      { EV_STATIC_LINE_SET_IDENTIFICATION, "Line_SetIdentification" },
//      { EV_STATIC_LIGHT_TRANSFER_FLOOR,    "Transfer_FloorLight"    },
//      { EV_STATIC_LIGHT_TRANSFER_CEILING,  "Transfer_CeilingLight"  },
//      { EV_STATIC_TRANSFER_HEIGHTS,        "Transfer_Heights"       },
//   };

   // There aren't enough of these to warrant a hash table. Yet.
//   for(size_t i = 0; i < earrlen(namedStatics); i++)
//   {
//      if(!strcasecmp(namedStatics[i].name, name))
//         return EV_SpecialForStaticInit(namedStatics[i].staticFn);
//   }

   return 0;
}

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
      *(int *)result = (int)(E_LineSpecForName(value));
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

      *(int *)result = num;
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
// Load a lump for ExtraData
//
bool ExtraData::LoadLump(const Wad &wad, const char *name)
{
   const Lump *lump = wad.FindLump(name);
   if(!lump)
   {
      fprintf(stderr, "Couldn't find ExtraData lump %s\n", name);
      return false;
   }

   cfg_t *cfg;

   return true;
}
