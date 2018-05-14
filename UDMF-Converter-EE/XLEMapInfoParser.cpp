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

#include "Helpers.hpp"
#include "XLEMapInfoParser.hpp"

// State Table
bool (XLEMapInfoParser::* XLEMapInfoParser::States[])(XLTokenizer &) =
{
   &XLEMapInfoParser::DoStateExpectHeader,
   &XLEMapInfoParser::DoStateExpectKeyword,
   &XLEMapInfoParser::DoStateExpectEqual,
   &XLEMapInfoParser::DoStateExpectValue,
   &XLEMapInfoParser::DoStateExpectEOL,
   &XLEMapInfoParser::DoStateSkipSection
};

// Expecting a bracketed header to start a new definition
bool XLEMapInfoParser::DoStateExpectHeader(XLTokenizer &tokenizer)
{
   // if not a bracketed token, keep scanning
   if(tokenizer.TokenType() != XLTokenizer::TOKEN_BRACKETSTR)
      return true;

   if(!mLocalLevel.empty())
   {
      // if not in global mode, the only recognized header is "level info"
      if(!strcasecmp(tokenizer.Token(), "level info"))
      {
         // in level header mode, all [level info] blocks accumulate into a
         // single definition
         if(!mCurInfo)
            mCurInfo = &mAllInfo[mLocalLevel];  // don't clear here
         mNextState = STATE_EXPECTKEYWORD;
      }
      else
         mNextState = STATE_SKIPSECTION;

      mState = STATE_EXPECTEOL;
   }
   else
   {
      // in EMAPINFO, only map names are allowed; each populates its own
      // MetaTable object, and the newest definition for a given map is the
      // one which entirely wins (any data from a previous section with the
      // same name will be obliterated by a later definition).
      (mCurInfo = &mAllInfo[tokenizer.Token()])->clear();   // also clear
      mNextState = STATE_EXPECTKEYWORD;
      mState = STATE_EXPECTEOL;
   }

   return true;
}

// Expecting a MapInfo keyword
bool XLEMapInfoParser::DoStateExpectKeyword(XLTokenizer &tokenizer)
{
   enum XLTokenizer::TokenType tokenType = tokenizer.TokenType();

   switch(tokenType)
   {
      case XLTokenizer::TOKEN_BRACKETSTR:
         // if we find a bracketed string instead, it's actually the start of a new
         // section; process it through the header state handler immediately
         return DoStateExpectHeader(tokenizer);

      case XLTokenizer::TOKEN_KEYWORD:
      case XLTokenizer::TOKEN_STRING:
         // record as the current key and expect potential = to follow
         mKey = tokenizer.Token();
         mValue = "";
         mState = STATE_EXPECTEQUAL;
         if(!strcasecmp(mKey.c_str(), "levelaction"))
            mAllowMultiValue = true;
         else
            mAllowMultiValue = false;
         break;

      default:
         // if we see anything else, keep scanning
         break;
   }

   return true;
}

// Expecting optional = sign, or the start of the value
bool XLEMapInfoParser::DoStateExpectEqual(XLTokenizer &tokenizer)
{
   if(!strcmp(tokenizer.Token(), "="))
   {
      // found an optional =, expect value.
      mState = STATE_EXPECTVALUE;
      return true;
   }
   // otherwise, this is the value token; process it immediately
   return DoStateExpectValue(tokenizer);
}

// Expecting value
bool XLEMapInfoParser::DoStateExpectValue(XLTokenizer &tokenizer)
{
   enum XLTokenizer::TokenType tokenType = tokenizer.TokenType();

   // remain in this state until a TOKEN_LINEBREAK is encountered
   if(tokenType == XLTokenizer::TOKEN_LINEBREAK)
   {
      // push the key/value pair
      if(!mKey.empty() && !mValue.empty())
      {
         if(!mAllowMultiValue)
            mCurInfo->erase(mKey);
         MakeLowerCase(mKey);
         mCurInfo->insert({ mKey, mValue });
      }
      mState = STATE_EXPECTKEYWORD;
      mKey = "";
      mValue = "";
   }
   else
   {
      // add token into the accumulating value string; subsequent tokens will
      // be separated by a single whitespace in the output
      if(mValue.length())
         mValue += " ";
      mValue += tokenizer.Token();
      mState = STATE_EXPECTVALUE; // ensure we come back here
   }

   return true;
}

// Expecting the end of the line
bool XLEMapInfoParser::DoStateExpectEOL(XLTokenizer &tokenizer)
{
   if(tokenizer.TokenType() != XLTokenizer::TOKEN_LINEBREAK)
      return false; // error

   mState = mNextState;
   return true;
}

// Skip forward until the next section header token
bool XLEMapInfoParser::DoStateSkipSection(XLTokenizer &tokenizer)
{
   // ignore all tokens until another section header appears
   if(tokenizer.TokenType() != XLTokenizer::TOKEN_BRACKETSTR)
      return true;

   // process this token immediately through doStateExpectHeader
   return DoStateExpectHeader(tokenizer);
}

// Dispatch token to appropriate state handler
bool XLEMapInfoParser::DoToken(XLTokenizer &token)
{
   return (this->*States[mState])(token);
}

// Reinitialize parser at beginning of lump parsing
void XLEMapInfoParser::StartLump()
{
   mState = STATE_EXPECTHEADER;
   mNextState = STATE_INVALID;
   mCurInfo = nullptr;
   mKey = "";
   mValue = "";
   mAllowMultiValue = false;
}

// Setup tokenizer state before parsing begins
void XLEMapInfoParser::InitTokenizer(XLTokenizer &tokenizer)
{
   tokenizer.SetTokenFlags(
                           XLTokenizer::TF_LINEBREAKS   |  // linebreaks are treated as significant
                           XLTokenizer::TF_BRACKETS     |  // support [keyword] tokens
                           XLTokenizer::TF_HASHCOMMENTS |  // # can start a comment as well as ;
                           XLTokenizer::TF_SLASHCOMMENTS); // support C++-style comments too.
}

// Handle EOF
void XLEMapInfoParser::OnEOF(bool early)
{
   if(mState == STATE_EXPECTVALUE)
   {
      // if we ended in STATE_EXPECTVALUE and there is a currently valid
      // info, key, and value, we need to push it.
      if(mCurInfo && !mKey.empty() && !mValue.empty())
      {
         MakeLowerCase(mKey);
         mCurInfo->insert({ mKey, mValue });
      }
   }
}
