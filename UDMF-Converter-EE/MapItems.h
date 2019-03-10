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
// Purpose: Basic map items
// Authors: Ioan Chera
//

#ifndef MapItems_h
#define MapItems_h

#include <string>

#define GenFloorBase          0x6000
#define GenCeilingBase        0x4000
#define GenDoorBase           0x3c00
#define GenLockedBase         0x3800
#define GenLiftBase           0x3400
#define GenStairsBase         0x3000
#define GenCrusherBase        0x2F80

#define FloorCrushShift           12
#define FloorChangeShift          10
#define FloorTargetShift           7
#define FloorDirectionShift        6
#define FloorModelShift            5
#define FloorSpeedShift            3

#define DoorDelayShift             8
#define DoorMonsterShift           7
#define DoorKindShift              5
#define DoorSpeedShift             3

#define LockedNKeysShift           9
#define LockedKeyShift             6
#define LockedKindShift            5
#define LockedSpeedShift           3

#define LiftTargetShift            8
#define LiftDelayShift             6
#define LiftMonsterShift           5
#define LiftSpeedShift             3

#define StairIgnoreShift           9
#define StairDirectionShift        8
#define StairStepShift             6
#define StairMonsterShift          5
#define StairSpeedShift            3

#define CrusherSilentShift         6
#define CrusherMonsterShift        5
#define CrusherSpeedShift          3

#define TriggerType           0x0007
#define TriggerTypeShift      0

#define NF_SUBSECTOR    0x80000000

//
// Simple vertex definition
//
struct Vertex
{
   int x, y;
};

enum
{
   TF_EASY = 1,
   TF_NORMAL = 2,
   TF_HARD = 4,
   TF_AMBUSH = 8,
   TF_NOTSINGLE = 0x10,
   TF_NOTDM = 0x20,
   TF_NOTCOOP = 0x40,
   TF_FRIEND = 0x80,
   TF_RESERVED = 0x100,
   TF_DORMANT = 0x200
};

static const int ED_CTRL_DOOMEDNUM = 5004;

//
// Simple thing definition
//
struct Thing
{
   int x;
   int y;
   int angle;
   int type;
   unsigned flags;
};

//
// Linedef flags
//
enum
{
   LF_BLOCKING = 1,
   LF_BLOCKMONSTERS = 2,
   LF_TWOSIDED = 4,
   LF_DONTPEGTOP = 8,
   LF_DONTPEGBOTTOM = 0x10,
   LF_SECRET = 0x20,
   LF_BLOCKSOUND = 0x40,
   LF_HIDDEN = 0x80,
   LF_MAPPED = 0x100,
   LF_PASSUSE = 0x200,
   LF_3DMIDTEX = 0x400,
   LF_RESERVED = 0x800
};

//
// Simple line definition
//
struct Linedef
{
   int v1;
   int v2;
   int flags;
   int special;
   int tag;
   int sidenum[2];
};

//
// Simple side definition
//
struct Sidedef
{
   int xoffset;
   int yoffset;
   std::string upperpic;
   std::string lowerpic;
   std::string midpic;
   int sector;
};

//
// Simple sector definition
//
struct Sector
{
   int floorheight;
   int ceilingheight;
   std::string floorpic;
   std::string ceilingpic;
   int lightlevel;
   int special;
   int tag;
};

//
// Segment
//
struct Seg
{
   int startVertex;
   int endVertex;
   int angle;
   int linedef;
   int dir;
   int offset;
};

struct Subsector
{
   int segcount;
   int startseg;
};

struct Node
{
   int partx;
   int party;
   int dx;
   int dy;
   int rightbox[4];
   int leftbox[4];
   int rightchild;
   int leftchild;
};

#endif /* MapItems_h */
