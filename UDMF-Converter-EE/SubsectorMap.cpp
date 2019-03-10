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
// Purpose: bsp map, as a precursor to znodes
// Authors: Ioan Chera
//

#include <float.h>
#include <math.h>
#include "DoomLevel.hpp"
#include "SubsectorMap.hpp"

//
// Checks nodes and builds a map
//
SubsectorMap::SubsectorMap(const DoomLevel &level) : mLevel(level)
{
   if(level.GetNodes().empty())
   {
      fprintf(stderr, "Warning: level has no nodes\n");
      return;
   }

   int left, bottom, right, top;
   level.GetBounds(left, bottom, right, top);
   if(left >= right || bottom >= top)
   {
      fprintf(stderr, "Warning: empty map\n");
      return;
   }

   left -= 32;
   bottom -= 32;
   right += 32;
   top += 32;

   mSplitColl.reserve(level.GetNodes().size() + 4);

   mLineStack.reserve(level.GetNodes().size() + 4);
   mLineStack.emplace_back(-1, 1, 0, -left);
   mLineStack.emplace_back(-1, 0, 1, -top);
   mLineStack.emplace_back(-1, 1, 0, -right);
   mLineStack.emplace_back(-1, 0, 1, -bottom);

   RecursiveGetLines(level.IndexOf(level.GetNodes().back()));
}

void SubsectorMap::RecursiveGetLines(int nodenum)
{
   if(nodenum & NF_SUBSECTOR)
      return;

   if (nodenum < 0 || nodenum >= mLevel.GetNodes().size())
   {
      fprintf(stderr, "Invalid node index %d\n", nodenum);
      return;
   }
   const Node &node = mLevel.GetNodes()[nodenum];
   LineEq le(node);
   if(!le.a && !le.b)
   {
      fprintf(stderr, "Degenerate node %d\n", nodenum);
      return;
   }

   double dmin1 = DBL_MAX, dmin2 = DBL_MAX;
   double x1 = NAN, y1 = NAN, x2 = NAN, y2 = NAN;
   int imin1 = -1, imin2 = -1;

   for (const IndexedLineEq &ile : mLineStack)
   {
      double ix, iy;
      const LineEq &le2 = ile.le;
      if(!le.Intersects(le2, ix, iy))
         continue;
      double dist1 = fabs(ix - node.partx) + fabs(iy - node.party);
      double dist2 = fabs(ix - node.partx - node.dx) + fabs(iy - node.party - node.dy);

      if(dist2 < dist1)
      {
         if (dist2 < dmin2)
         {
            dmin2 = dist2;
            x2 = ix;
            y2 = iy;
            imin2 = ile.ind;
         }
      }
      else
      {
         if(dist1 < dmin1)
         {
            dmin1 = dist1;
            x1 = ix;
            y1 = iy;
            imin1 = ile.ind;
         }
      }
   }
   if(imin1 >= 0)
      mSplitColl[imin1].push_back({ x1, y1 });
   if(imin2 >= 0)
      mSplitColl[imin2].push_back({ x2, y2 });

   mSplitColl.push_back({{ x1, y1 }, {x2, y2 }});

   mLineStack.emplace_back(int(mSplitColl.size() - 1), le);
   RecursiveGetLines(node.rightchild);
   RecursiveGetLines(node.leftchild);
   mLineStack.pop_back();
}
