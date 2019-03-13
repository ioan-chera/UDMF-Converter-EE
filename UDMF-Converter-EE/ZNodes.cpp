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
// Purpose: UDMF GL ZNODES
// Authors: Ioan Chera
//

#include <stdio.h>
#include "DoomLevel.hpp"
#include "ZNodes.hpp"

static void WriteAsInt(intptr_t number, std::ostream &os)
{
   char n[4];
   n[0] = number & 0xff;
   n[1] = number >> 8 & 0xff;
   n[2] = number >> 16 & 0xff;
   n[3] = number >> 24 & 0xff;
   os.write(n, 4);
}

static void WriteShort(intptr_t number, std::ostream &os)
{
   char n[2];
   n[0] = number & 0xff;
   n[1] = number >> 8 & 0xff;
   os.write(n, 2);
}

void WriteZNodes(const DoomLevel &level, std::ostream &os)
{
   os << "XGL3";

   WriteAsInt(level.GetVertices().size(), os);
   WriteAsInt(level.GetNodeVertices().size(), os);

   int32_t ival;

   for(const Vertex &vertex : level.GetNodeVertices())
   {
      WriteAsInt(vertex.x << 16, os);
      WriteAsInt(vertex.y << 16, os);
   }

   WriteAsInt(level.GetSubsectors().size(), os);

   for(const Subsector &ss : level.GetSubsectors())
   {
      // needed because of the GL3 single-vertex format
      WriteAsInt(ss.segcount * 2, os);
   }

   WriteAsInt(level.GetSegs().size() * 2, os);

   for (const Seg &seg : level.GetSegs())
   {
      WriteAsInt(seg.startVertex, os);
      os.write("\xff\xff\xff\xff", 4);
      WriteAsInt(seg.linedef, os);
      os.put(!!seg.dir);

      // now draw the virtual gl seg, just to define endVertex of previous physical one
      WriteAsInt(seg.endVertex, os);
      os.write("\xff\xff\xff\xff\xff\xff\xff\xff", 8);   // mark it as virtual
      os.put(0);
   }

   WriteAsInt(level.GetNodes().size(), os);

   for(const Node &node : level.GetNodes())
   {
      WriteAsInt(node.partx << 16, os);
      WriteAsInt(node.party << 16, os);
      WriteAsInt(node.dx << 16, os);
      WriteAsInt(node.dy << 16, os);
      for(int i = 0; i < 4; ++i)
         WriteShort(node.rightbox[i], os);
      for(int i = 0; i < 4; ++i)
         WriteShort(node.leftbox[i], os);
      WriteAsInt(node.rightchild, os);
      WriteAsInt(node.leftchild, os);
   }
}
