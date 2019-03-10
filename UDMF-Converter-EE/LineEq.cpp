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
// Purpose: line equation
// Authors: Ioan Chera
//

#include "LineEq.hpp"
#include "MapItems.h"

LineEq::LineEq(const Node &node) :
a(-node.dy),
b(node.dx),
c((double)node.dy * node.partx - (double)node.dx * node.party)
{
}

bool LineEq::Intersects(const LineEq &other, double &ix, double &iy) const
{
   double deltax, deltay, delta;
   delta = a * other.b - other.a * b;
   if(!delta)
      return false;

   deltax = -c * other.b + other.c * b;
   deltay = -a * other.c + other.a * c;

   ix = deltax / delta;
   iy = deltay / delta;
   return true;
}
