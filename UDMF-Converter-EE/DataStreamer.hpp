//
// UDMF Converter EE
// Copyright (C) 2018 Ioan Chera
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
// Purpose: Data streamer
// Authors: Ioan Chera
//

#ifndef DataStreamer_hpp
#define DataStreamer_hpp

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

class DataStreamer
{
public:
   DataStreamer(const uint8_t *data, size_t dataSize) :
   mData(data),
   mDataSize(dataSize)
   {
   }

   DataStreamer(const std::vector<uint8_t> &data) :
   DataStreamer(data.data(), data.size())
   {
   }

   uint8_t ReadByte();
   int16_t ReadShort()
   {
      return ReadByte() | ReadByte() << 8;
   }
   std::string ReadString(size_t length);

   bool IsEOF() const
   {
      return mPos >= mDataSize;
   }
private:
   const uint8_t *mData = nullptr;
   size_t mDataSize = 0;
   size_t mPos = 0;
};

#endif /* DataStreamer_hpp */
