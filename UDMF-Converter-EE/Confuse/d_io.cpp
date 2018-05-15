// Emacs style mode select -*- C++ -*-
//----------------------------------------------------------------------------
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
// File/WAD Standard Input Routines
//
// This code was moved here from d_deh.c and generalized to make a consistent
// interface for emulating stdio functions on wad lumps. The structure here 
// handles either external files or wad lumps, depending on how it is 
// initialized, and emulates stdio regardless.
//
// James Haley
//
//----------------------------------------------------------------------------

//#include "d_keywds.h"
//#include "doomtype.h"
//#include "d_io.h"
#include "d_dwfile.h"
//#include "m_utils.h"
//#include "w_wad.h"
#include "../Wad.hpp"

//
// Constructor
//
DWFILE::DWFILE()
   : type(0), inp(NULL), lump(NULL), data(NULL), size(0), origsize(0), 
     lumpnum(0)
{
}

//
// Destructor
//
DWFILE::~DWFILE()
{
   if(isOpen())
      close();
}

//
// DWFILE::getStr
//
// Equivalent to fgets
//
char *DWFILE::getStr(char *buf, size_t n)
{
   // If this is a real file, return regular fgets
   if(type == DWF_FILE)
      return fgets(buf, static_cast<int>(n), (FILE *)inp);
   
   // If no more characters
   if(!n || size <= 0 || !*inp)
      return NULL;
  
   if(n == 1)
   {
      size--;
      *buf = *inp++;
   }
   else
   {  // copy buffer
      char *p = buf;
      while(n > 1 && *inp && size && (n--, size--, *p++ = *inp++) != '\n')
         ;
      *p = 0;
   }
   return buf; // Return buffer pointer
}

//
// DWFILE::atEof
//
// Equivalent to feof
//
int DWFILE::atEof() const
{
   return (type == DWF_FILE) ? 
           feof((FILE *)inp) : !*inp || size <= 0;
}

//
// DWFILE::getChar
//
// Equivalent to fgetc
//
int DWFILE::getChar()
{
   return (type == DWF_FILE) ? 
   fgetc((FILE *)inp) : size > 0 ? size--, *inp++ : EOF;
}

//
// DWFILE::unGetChar
//
// Equivalent to ungetc
//
// haleyjd 04/03/03: note that wad lump buffers will not be 
// written into by this function -- this is necessary to
// maintain cacheability.
//
int DWFILE::unGetChar(int c)
{
   return (type == DWF_FILE) ? 
   ungetc(c, (FILE *)inp) : size < origsize ? size++, *(--inp) : EOF;
}

//
// DWFILE::openFile
//
// Open a file into the DWFILE instance. Uses standard fopen.
//
void DWFILE::openFile(const char *filename, const char *mode)
{
   if(isOpen())
      close();

   inp     = (uint8_t *)fopen(filename, mode);
   lumpnum = -1;
   type    = DWF_FILE;

   // zero out fields not used for file reading
   data     = NULL;
   lump     = NULL;
   origsize = 0;
   size     = 0;
}

//
// DWFILE::openLump
//
// Open a wad lump into the DWFILE structure. The wad lump will
// be cached in zone memory at static allocation level.
//
void DWFILE::openLump(const Wad &wad, int p_lumpnum)
{
   if(isOpen())
      close();

   // haleyjd 04/03/03: added origsize field for D_Ungetc
   lumpnum = p_lumpnum;
   size = origsize = (int)wad.Lumps()[p_lumpnum].Data().size();
   inp = lump = wad.Lumps()[p_lumpnum].Data().data();
   type    = DWF_LUMP;

   // zero out fields not used for lump reading
   data = NULL;
}

/*
// TODO: support this properly
void D_OpenData(DWFILE *infile, byte *data, int size)
{
   memset(infile, 0, sizeof(*infile));
   infile->size = infile->origsize = size;
   infile->inp = infile->data = data;
   infile->lumpnum = -1;
   infile->type = DWF_DATA;
}
*/

//
// DWFILE::close
//
// Closes the file or wad lump.  Calls standard fclose for files;
// sets wad lumps to cache allocation level.
//
void DWFILE::close()
{
   if(!isOpen())
      return;

   switch(type)
   {
   case DWF_LUMP:
      break;
   case DWF_DATA:
      break;
   case DWF_FILE:
      fclose((FILE *)inp);
      break;
   default:
      break;
   }

   inp = lump = data = NULL;
}

//
// DWFILE::read
//
// haleyjd
//
size_t DWFILE::read(void *dest, size_t p_size, size_t p_num)
{
   if(type == DWF_FILE)
      return fread(dest, p_size, p_num, (FILE *)inp);
   else
   {
      size_t numbytes = p_size * p_num;
      size_t numbytesread = 0;
      uint8_t *d = (uint8_t *)dest;

      while(numbytesread < numbytes && size)
      {
         *d++ = *inp++;
         size--;
         numbytesread++;
      }

      return numbytesread;
   }
}

//
// Gets the length of a file given its handle.
//
static long M_FileLength(FILE *f)
{
   long curpos, len;

   curpos = ftell(f);
   fseek(f, 0, SEEK_END);
   len = ftell(f);
   fseek(f, curpos, SEEK_SET);

   return len;
}


//
// DWFILE::fileLength
//
// haleyjd 03/08/06: returns the length of the file.
//
long DWFILE::fileLength() const
{
   return (type == DWF_FILE) ? M_FileLength((FILE *)inp) : origsize;
}

// EOF

