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

#ifndef LineEq_hpp
#define LineEq_hpp

struct Node;

struct LineEq
{
   const double a, b, c;

   LineEq(double a, double b, double c) : a(a), b(b), c(c)
   {
   }

   LineEq(const Node &node);

   bool Intersects(const LineEq &other, double &ix, double &iy) const;
};

#endif /* LineEq_hpp */
