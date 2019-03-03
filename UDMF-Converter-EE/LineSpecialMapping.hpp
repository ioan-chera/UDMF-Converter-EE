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
// Purpose: Mapping Doom line specials to UDMF
// Authors: Ioan Chera
//

#ifndef LineSpecialMapping_hpp
#define LineSpecialMapping_hpp

#include "UDMFItems.hpp"

enum UdmfSpecial
{
   Polyobj_StartLine = 1,
   Polyobj_RotateLeft = 2,
   Polyobj_RotateRight = 3,
   Polyobj_Move = 4,
   Polyobj_ExplicitLine = 5,
   Polyobj_MoveTimes8 = 6,
   Polyobj_DoorSwing = 7,
   Polyobj_DoorSlide = 8,
   Line_Horizon = 9,
   Door_Close = 10,
   Door_Open = 11,
   Door_Raise = 12,
   Door_LockedRaise = 13,
   Thing_Raise = 17,
   Thing_Stop = 19,
   Floor_LowerByValue = 20,
   Floor_LowerToLowest = 21,
   Floor_LowerToNearest = 22,
   Floor_RaiseByValue = 23,
   Floor_RaiseToHighest = 24,
   Floor_RaiseToNearest = 25,
   Floor_RaiseAndCrush = 28,
   Pillar_Build = 29,
   Pillar_Open = 30,
   Floor_RaiseByValueTimes8 = 35,
   Floor_LowerByValueTimes8 = 36,
   Floor_MoveToValue = 37,
   Ceiling_Waggle = 38,
   Ceiling_LowerByValue = 40,
   Ceiling_RaiseByValue = 41,
   Ceiling_CrushAndRaise = 42,
   Ceiling_LowerAndCrush = 43,
   Ceiling_CrushStop = 44,
   Ceiling_CrushRaiseAndStay = 45,
   Floor_CrushStop = 46,
   Ceiling_MoveToValue = 47,
   Sector_Attach3dMidtex = 48,
   Sector_SetPortal = 57,
   Polyobj_OR_MoveToSpot = 59,
   Plat_PerpetualRaise = 60,
   Plat_Stop = 61,
   Plat_DownWaitUpStay = 62,
   Plat_DownByValue = 63,
   Plat_UpWaitDownStay = 64,
   Plat_UpByValue = 65,
   Floor_LowerInstant = 66,
   Floor_RaiseInstant = 67,
   Floor_MoveToValueTimes8 = 68,
   Ceiling_MoveToValueTimes8 = 69,
   Teleport = 70,
   Teleport_NoFog = 71,
   ThrustThing = 72,
   DamageThing = 73,
   Teleport_NewMap = 74,
   Teleport_EndGame = 75,
   ACS_Execute = 80,
   ACS_Suspend = 81,
   ACS_Terminate = 82,
   ACS_LockedExecute = 83,
   ACS_ExecuteWithResult = 84,
   ACS_LockedExecuteDoor = 85,
   Polyobj_MoveToSpot = 86,
   Polyobj_Stop = 87,
   Polyobj_MoveTo = 88,
   Polyobj_OR_MoveTo = 89,
   Polyobj_OR_RotateLeft = 90,
   Polyobj_OR_RotateRight = 91,
   Polyobj_OR_Move = 92,
   Polyobj_OR_MoveTimes8 = 93,
   Pillar_BuildAndCrush = 94,
   FloorAndCeiling_LowerByValue = 95,
   FloorAndCeiling_RaiseByValue = 96,
   Ceiling_LowerAndCrushDist = 97,
   Scroll_Texture_Left = 100,
   Scroll_Texture_Right = 101,
   Scroll_Texture_Up = 102,
   Scroll_Texture_Down = 103,
   Ceiling_CrushAndRaiseSilentDist = 104,
   Door_WaitRaise = 105,
   Door_WaitClose = 106,
   Light_RaiseByValue = 110,
   Light_LowerByValue = 111,
   Light_ChangeToValue = 112,
   Light_Fade = 113,
   Light_Glow = 114,
   Light_Flicker = 115,
   Light_Strobe = 116,
   Plane_Copy = 118,
   Thing_Damage = 119,
   Radius_Quake = 120,
   Line_SetIdentification = 121,
   ThrustThingZ = 128,
   Thing_Activate = 130,
   Thing_Deactivate = 131,
   Thing_Remove = 132,
   Thing_Destroy = 133,
   Thing_Projectile = 134,
   Thing_Spawn = 135,
   Thing_ProjectileGravity = 136,
   Thing_SpawnNoFog = 137,
   Floor_Waggle = 138,
   Sector_ChangeSound = 140,
   Line_SetPortal = 156,
   Ceiling_CrushAndRaiseDist = 168,
   Thing_ChangeTID = 176,
   ChangeSkill = 179,
   Plane_Align = 181,
   Sector_SetRotation = 185,
   Sector_SetCeilingPanning = 186,
   Sector_SetFloorPanning = 187,
   Static_Init = 190,
   Ceiling_LowerToHighestFloor = 192,
   Ceiling_LowerInstant = 193,
   Ceiling_RaiseInstant = 194,
   Ceiling_CrushRaiseAndStayA = 195,
   Ceiling_CrushAndRaiseA = 196,
   Ceiling_CrushAndRaiseSilentA = 197,
   Ceiling_RaiseByValueTimes8 = 198,
   Ceiling_LowerByValueTimes8 = 199,
   Generic_Floor = 200,
   Generic_Ceiling = 201,
   Generic_Crusher = 205,
   Plat_DownWaitUpStayLip = 206,
   Plat_PerpetualRaiseLip = 207,
   Transfer_Heights = 209,
   Transfer_FloorLight = 210,
   Transfer_CeilingLight = 211,
   Teleport_Line = 215,
   Stairs_BuildUpDoom = 217,
   Sector_SetWind = 218,
   Sector_SetFriction = 219,
   Sector_SetCurrent = 220,
   Scroll_Texture_Model = 222,
   Scroll_Floor = 223,
   Scroll_Ceiling = 224,
   Scroll_Texture_Offsets = 225,
   ACS_ExecuteAlways = 226,
   PointPush_SetForce = 227,
   Plat_RaiseAndStayTx0 = 228,
   Plat_UpByValueStayTx = 230,
   Plat_ToggleCeiling = 231,
   Light_StrobeDoom = 232,
   Light_MinNeighbor = 233,
   Light_MaxNeighbor = 234,
   Floor_TransferTrigger = 235,
   Floor_TransferNumeric = 236,
   Floor_RaiseToLowestCeiling = 238,
   Floor_RaiseByTexture = 240,
   Floor_LowerToHighest = 242,
   Exit_Normal = 243,
   Exit_Secret = 244,
   Elevator_RaiseToNearest = 245,
   Elevator_MoveToFloor = 246,
   Elevator_LowerToNearest = 247,
   HealThing = 248,
   Door_CloseWaitOpen = 249,
   Floor_Donut = 250,
   FloorAndCeiling_LowerRaise = 251,
   Ceiling_RaiseToNearest = 252,
   Ceiling_LowerToLowest = 253,
   Ceiling_LowerToFloor = 254,
   Ceiling_CrushRaiseAndStaySilA = 255,
   Floor_LowerToHighestEE = 256,
   Floor_RaiseToLowest = 257,
   Floor_LowerToLowestCeiling = 258,
   Floor_RaiseToCeiling = 259,
   Floor_ToCeilingInstant = 260,
   Floor_LowerByTexture = 261,
   Ceiling_RaiseToHighest = 262,
   Ceiling_ToHighestInstant = 263,
   Ceiling_LowerToNearest = 264,
   Ceiling_RaiseToLowest = 265,
   Ceiling_RaiseToHighestFloor = 266,
   Ceiling_ToFloorInstant = 267,
   Ceiling_RaiseByTexture = 268,
   Ceiling_LowerByTexture = 269,
   Stairs_BuildDownDoom = 270,
   Stairs_BuildUpDoomSync = 271,
   Stairs_BuildDownDoomSync = 272,
   Stairs_BuildUpDoomCrush = 273,
   Portal_Define = 300,
   Line_QuickPortal = 301
};

enum ClassicStaticSpecial
{
   EV_STATIC_SCROLL_LINE_LEFT =  48,
   EV_STATIC_SCROLL_LINE_RIGHT =  85,
   EV_STATIC_LIGHT_TRANSFER_FLOOR = 213,
   EV_STATIC_SCROLL_ACCEL_CEILING = 214,
   EV_STATIC_SCROLL_ACCEL_FLOOR = 215,
   EV_STATIC_CARRY_ACCEL_FLOOR = 216,
   EV_STATIC_SCROLL_CARRY_ACCEL_FLOOR = 217,
   EV_STATIC_SCROLL_ACCEL_WALL = 218,
   EV_STATIC_FRICTION_TRANSFER = 223,
   EV_STATIC_WIND_CONTROL = 224,
   EV_STATIC_CURRENT_CONTROL = 225,
   EV_STATIC_PUSHPULL_CONTROL = 226,
   EV_STATIC_TRANSFER_HEIGHTS = 242,
   EV_STATIC_SCROLL_DISPLACE_CEILING = 245,
   EV_STATIC_SCROLL_DISPLACE_FLOOR = 246,
   EV_STATIC_CARRY_DISPLACE_FLOOR = 247,
   EV_STATIC_SCROLL_CARRY_DISPLACE_FLOOR = 248,
   EV_STATIC_SCROLL_DISPLACE_WALL = 249,
   EV_STATIC_SCROLL_CEILING = 250,
   EV_STATIC_SCROLL_FLOOR = 251,
   EV_STATIC_CARRY_FLOOR = 252,
   EV_STATIC_SCROLL_CARRY_FLOOR = 253,
   EV_STATIC_SCROLL_WALL_WITH = 254,
   EV_STATIC_SCROLL_BY_OFFSETS = 255,
   EV_STATIC_TRANSLUCENT = 260,
   EV_STATIC_LIGHT_TRANSFER_CEILING = 261,
   EV_STATIC_EXTRADATA_LINEDEF = 270,
   EV_STATIC_SKY_TRANSFER = 271,
   EV_STATIC_SKY_TRANSFER_FLIPPED = 272,
   EV_STATIC_3DMIDTEX_ATTACH_FLOOR = 281,
   EV_STATIC_3DMIDTEX_ATTACH_CEILING = 282,
   EV_STATIC_PORTAL_PLANE_CEILING = 283,
   EV_STATIC_PORTAL_PLANE_FLOOR = 284,
   EV_STATIC_PORTAL_PLANE_CEILING_FLOOR = 285,
   EV_STATIC_PORTAL_HORIZON_CEILING = 286,
   EV_STATIC_PORTAL_HORIZON_FLOOR = 287,
   EV_STATIC_PORTAL_HORIZON_CEILING_FLOOR = 288,
   EV_STATIC_PORTAL_LINE = 289,
   EV_STATIC_PORTAL_SKYBOX_CEILING = 290,
   EV_STATIC_PORTAL_SKYBOX_FLOOR = 291,
   EV_STATIC_PORTAL_SKYBOX_CEILING_FLOOR = 292,
   EV_STATIC_HERETIC_WIND = 293,
   EV_STATIC_HERETIC_CURRENT = 294,
   EV_STATIC_PORTAL_ANCHORED_CEILING = 295,
   EV_STATIC_PORTAL_ANCHORED_FLOOR = 296,
   EV_STATIC_PORTAL_ANCHORED_CEILING_FLOOR = 297,
   EV_STATIC_PORTAL_ANCHOR = 298,
   EV_STATIC_PORTAL_ANCHOR_FLOOR = 299,
   EV_STATIC_PORTAL_TWOWAY_CEILING = 344,
   EV_STATIC_PORTAL_TWOWAY_FLOOR = 345,
   EV_STATIC_PORTAL_TWOWAY_ANCHOR = 346,
   EV_STATIC_PORTAL_TWOWAY_ANCHOR_FLOOR = 347,
   EV_STATIC_PORTAL_LINKED_CEILING = 358,
   EV_STATIC_PORTAL_LINKED_FLOOR = 359,
   EV_STATIC_PORTAL_LINKED_ANCHOR = 360,
   EV_STATIC_PORTAL_LINKED_ANCHOR_FLOOR = 361,
   EV_STATIC_PORTAL_LINKED_LINE2LINE = 376,
   EV_STATIC_PORTAL_LINKED_L2L_ANCHOR = 377,
   EV_STATIC_LINE_SET_IDENTIFICATION = 378,
   EV_STATIC_ATTACH_SET_CEILING_CONTROL = 379,
   EV_STATIC_ATTACH_SET_FLOOR_CONTROL = 380,
   EV_STATIC_ATTACH_FLOOR_TO_CONTROL = 381,
   EV_STATIC_ATTACH_CEILING_TO_CONTROL = 382,
   EV_STATIC_ATTACH_MIRROR_FLOOR = 383,
   EV_STATIC_ATTACH_MIRROR_CEILING = 384,
   EV_STATIC_PORTAL_APPLY_FRONTSECTOR = 385,
   EV_STATIC_SLOPE_FSEC_FLOOR = 386,
   EV_STATIC_SLOPE_FSEC_CEILING = 387,
   EV_STATIC_SLOPE_FSEC_FLOOR_CEILING = 388,
   EV_STATIC_SLOPE_BSEC_FLOOR = 389,
   EV_STATIC_SLOPE_BSEC_CEILING = 390,
   EV_STATIC_SLOPE_BSEC_FLOOR_CEILING = 391,
   EV_STATIC_SLOPE_BACKFLOOR_FRONTCEILING = 392,
   EV_STATIC_SLOPE_FRONTFLOOR_BACKCEILING = 393,
   EV_STATIC_SLOPE_FRONTFLOOR_TAG = 394,
   EV_STATIC_SLOPE_FRONTCEILING_TAG = 395,
   EV_STATIC_SLOPE_FRONTFLOORCEILING_TAG = 396,
   EV_STATIC_EXTRADATA_SECTOR = 401,
   EV_STATIC_SCROLL_LINE_UP = 417,
   EV_STATIC_SCROLL_LINE_DOWN = 418,
   EV_STATIC_SCROLL_LINE_DOWN_FAST = 419,
   EV_STATIC_PORTAL_HORIZON_LINE = 450
};

enum Spac
{
   PlayerCrosses = 1,
   MonsterCrosses = 2,
   PlayerUses = 4,
   MonsterUses = 8,
   PlayerShoots = 16,
   MonsterShoots = 32,
   Repeatable = 64,
   NoTag = 128,  // don't set first arg as tag.
   TagSecond = 256,  // tag the second arg instead of first
   TagThird = 512,
   FirstSide = 1024,
   TagFirstSecond = 2048
};

//
// UDMF target special
//
struct UdmfSpecialTarget
{
   int special;
   int args[5];
   unsigned flags;   // spac flags and related
   void (LinedefConversion::*additional)(int special, int tag, UDMFLine &line);
};

void InitLineMapping();
void InitExtraDataMappings();
UdmfSpecial GetSpecialByName(const char *name);
const UdmfSpecialTarget *GetUDMFSpecial(int special);

#endif /* LineSpecialMapping_hpp */
