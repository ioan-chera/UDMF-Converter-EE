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

void WriteZNodes(const DoomLevel &level, FILE *f)
{
   fputs("XGL3", f);

   uint32_t val = (uint32_t)level.GetVertices().size();
   fwrite(&val, 4, 1, f);

   val = (uint32_t)level.GetNodeVertices().size();
   fwrite(&val, 4, 1, f);

   int32_t ival;

   for(const Vertex &vertex : level.GetNodeVertices())
   {
      ival = vertex.x << 16;
      fwrite(&ival, 4, 1, f);
      ival = vertex.y << 16;
      fwrite(&ival, 4, 1, f);
   }

   val = (uint32_t)level.GetSubsectors().size();
   fwrite(&val, 4, 1, f);

   for(const Subsector &ss : level.GetSubsectors())
   {
      val = ss.segcount * 2; // needed because of the GL3 single-vertex format
      fwrite(&val, 4, 1, f);
   }

   val = (uint32_t)(level.GetSegs().size() * 2);
   fwrite(&val, 4, 1, f);

   for (const Seg &seg : level.GetSegs())
   {
      val = seg.startVertex;
      fwrite(&val, 4, 1, f);
      fputs("\xff\xff\xff\xff", f);
      val = seg.linedef;
      fwrite(&val, 4, 1, f);
      putc(seg.dir, f);

      // now draw the virtual gl seg, just to define endVertex of previous physical one
      val = seg.endVertex;
      fwrite(&val, 4, 1, f);
      fputs("\xff\xff\xff\xff\xff\xff\xff\xff", f); // mark it as virtual
      putc(0, f);
   }

   val = (uint32_t)level.GetNodes().size();
   fwrite(&val, 4, 1, f);
   for(const Node &node : level.GetNodes())
   {
      ival = node.partx << 16;
      fwrite(&ival, 4, 1, f);
      ival = node.party << 16;
      fwrite(&ival, 4, 1, f);
      ival = node.dx << 16;
      fwrite(&ival, 4, 1, f);
      ival = node.dy << 16;
      fwrite(&ival, 4, 1, f);
      for(int i = 0; i < 4; ++i)
      {
         int16_t sv = node.rightbox[i];
         fwrite(&sv, 2, 1, f);
      }
      for(int i = 0; i < 4; ++i)
      {
         int16_t sv = node.leftbox[i];
         fwrite(&sv, 2, 1, f);
      }
      val = node.rightchild;
      fwrite(&val, 4, 1, f);
      val = node.leftchild;
      fwrite(&val, 4, 1, f);
   }
}
