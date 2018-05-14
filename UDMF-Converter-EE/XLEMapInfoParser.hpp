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
// Purpose: SMMU/Eternity EMAPINFO Parser
// Authors: James Haley (from Eternity), Ioan Chera
//

#ifndef XLEMapInfoParser_hpp
#define XLEMapInfoParser_hpp

#include <unordered_map>
#include "XLParser.hpp"

typedef std::unordered_multimap<std::string, std::string> LevelInfo;

//
// Parses level info
//
class XLEMapInfoParser : public XLParser
{
public:
   XLEMapInfoParser() :
   XLParser("EMAPINFO")
   {
   }

   void LocalLevel(const char *lumpname)
   {
      mLocalLevel = lumpname;
   }

   const LevelInfo *Get(const char *levelName) const
   {
      auto it = mAllInfo.find(levelName);
      return it != mAllInfo.end() ? &it->second : nullptr;
   }

private:
   // State table declaration
   static bool (XLEMapInfoParser::*States[])(XLTokenizer &);

   // parser state enumeration
   enum State
   {
      STATE_INVALID = -1,
      STATE_EXPECTHEADER = 0,  // need a bracketed header
      STATE_EXPECTKEYWORD, // expecting a mapinfo field keyword
      STATE_EXPECTEQUAL,   // expecting optional = or start of next token
      STATE_EXPECTVALUE,   // expecting value for mapinfo keyword
      STATE_EXPECTEOL,     // expecting end-of-line token
      STATE_SKIPSECTION    // scan until next header
   };

   // state handlers
   bool DoStateExpectHeader(XLTokenizer &);
   bool DoStateExpectKeyword(XLTokenizer &);
   bool DoStateExpectEqual(XLTokenizer &);
   bool DoStateExpectValue(XLTokenizer &);
   bool DoStateExpectEOL(XLTokenizer &);
   bool DoStateSkipSection(XLTokenizer &);

   State mState = STATE_EXPECTHEADER;
   State mNextState = STATE_INVALID;
   std::string mLocalLevel;
   LevelInfo *mCurInfo = nullptr;
   std::unordered_map<std::string, LevelInfo> mAllInfo;
   std::string mKey;
   std::string mValue;
   bool mAllowMultiValue = false;

   virtual bool DoToken(XLTokenizer &token) override;
   virtual void StartLump() override;
   virtual void InitTokenizer(XLTokenizer &tokenizer) override;
   virtual void OnEOF(bool early) override;
};

#endif /* XLEMapInfoParser_hpp */
