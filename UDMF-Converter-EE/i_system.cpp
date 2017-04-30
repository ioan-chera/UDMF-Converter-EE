// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 James Haley et al.
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
//----------------------------------------------------------------------------
//
// DESCRIPTION:
//
//   General-purpose system-specific routines, including timer
//   installation, keyboard, mouse, and joystick code.
//
//-----------------------------------------------------------------------------

#include "z_zone.h"
#include "i_system.h"
//
// I_FatalError
//
// haleyjd 05/21/10: Call this for super-evil errors such as heap corruption,
// system-related problems, etc.
//
void I_FatalError(int code, const char *error, ...)
{
   va_list ap;
   va_start(ap, error);
   vfprintf(stderr, error, ap);
   va_end(ap);
   exit(-1);
}

//
// I_Error
//
// Normal error reporting / exit routine.
//
void I_Error(const char *error, ...) // killough 3/20/98: add const
{
   va_list ap;
   va_start(ap, error);
   vfprintf(stderr, error, ap);
   va_end(ap);
   exit(EXIT_FAILURE);
}

//
// I_ErrorVA
//
// haleyjd: varargs version of I_Error used chiefly by libConfuse.
//
void I_ErrorVA(const char *error, va_list args)
{
   vfprintf(stderr, error, args);
   exit(-1);
}

// EOF

