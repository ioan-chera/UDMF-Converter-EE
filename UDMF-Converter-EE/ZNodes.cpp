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

#include "DoomLevel.hpp"
#include "IOHelpers.hpp"
#include "ZNodes.hpp"

void WriteZNodes(const DoomLevel &level, std::ostream &os)
{
   os << "XGL3";

   WriteInt(level.GetVertices().size(), os);
   WriteInt(level.GetNodeVertices().size(), os);

   for(const Vertex &vertex : level.GetNodeVertices())
   {
      WriteInt(vertex.x << 16, os);
      WriteInt(vertex.y << 16, os);
   }

   WriteInt(level.GetSubsectors().size(), os);

   for(const Subsector &ss : level.GetSubsectors())
   {
      // needed because of the GL3 single-vertex format
      WriteInt(ss.segcount * 2, os);
   }

   WriteInt(level.GetSegs().size() * 2, os);

   for (const Seg &seg : level.GetSegs())
   {
      WriteInt(seg.startVertex, os);
      os.write("\xff\xff\xff\xff", 4);
      WriteInt(seg.linedef, os);
      os.put(!!seg.dir);

      // now draw the virtual gl seg, just to define endVertex of previous physical one
      WriteInt(seg.endVertex, os);
      os.write("\xff\xff\xff\xff\xff\xff\xff\xff", 8);   // mark it as virtual
      os.put(0);
   }

   WriteInt(level.GetNodes().size(), os);

   for(const Node &node : level.GetNodes())
   {
      WriteInt(node.partx << 16, os);
      WriteInt(node.party << 16, os);
      WriteInt(node.dx << 16, os);
      WriteInt(node.dy << 16, os);
      for(int i = 0; i < 4; ++i)
         WriteShort(node.rightbox[i], os);
      for(int i = 0; i < 4; ++i)
         WriteShort(node.leftbox[i], os);
      WriteInt(node.rightchild, os);
      WriteInt(node.leftchild, os);
   }
}
