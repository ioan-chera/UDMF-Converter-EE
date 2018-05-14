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
// Purpose: Parsing and processing for Hexen's data scripts (also EMAPINFO)
// Authors: James Haley (from Eternity), Ioan Chera
//

#include "Lump.hpp"
#include "Wad.hpp"
#include "XLParser.hpp"

//
// Tokenizer constructor
//
XLTokenizer::XLTokenizer(const Lump &lump) :
mInput(reinterpret_cast<const char *>(lump.Data().data()), lump.Data().size())
{
   mToken.reserve(32);
}

// State table for the tokenizer - static array of method pointers :)
void (XLTokenizer::* XLTokenizer::States[])() =
{
   &XLTokenizer::DoStateScan,
   &XLTokenizer::DoStateInToken,
   &XLTokenizer::DoStateInBrackets,
   &XLTokenizer::DoStateQuoted,
   &XLTokenizer::DoStateComment
};

//
// Call this to retrieve the next token from the input string. The token
// type is returned for convenience. Get the text of the token using the
// GetToken() method.
//
enum XLTokenizer::TokenType XLTokenizer::GetNextToken()
{
   mToken.clear();
   mState = STATE_SCAN; // always start out scanning for a new token
   mTokenType = TOKEN_NONE;   // nothing has been determined yet

   // already at end of input?
   if(mInput[mIndex])
   {
      while(mState != STATE_DONE)
      {
         (this->*States[mState])();
         ++mIndex;
      }
   }
   else
      mTokenType = TOKEN_EOF;

   return mTokenType;
}

//
// Tokenizer States
//

// Looking for the start of a new token
void XLTokenizer::DoStateScan()
{
   char c = mInput[mIndex];

   switch(c)
   {
      case ' ':
      case '\t':
      case '\r':
         // remain in this state
         break;
      case '\n':
         // if linebreak tokens are enabled, return one now
         if(mFlags & TF_LINEBREAKS)
         {
            mTokenType = TOKEN_LINEBREAK;
            mState = STATE_DONE;
         }
         // otherwise, remain in STATE_SCAN
         break;
      case '\0': // end of input
         mTokenType = TOKEN_EOF;
         mState = STATE_DONE;
         break;
      case ';': // start of a comment
         mState = STATE_COMMENT;
         break;
      case '"': // start of quoted string
         mTokenType = TOKEN_STRING;
         mState = STATE_QUOTED;
         break;
      default:
         // anything else is the start of a new token
         if(c == '#' && (mFlags & TF_HASHCOMMENTS)) // possible start of hash comment
         {
            mState = STATE_COMMENT;
            break;
         }
         else if(c == '/' && mInput[mIndex + 1] == '/' &&
                 (mFlags & TF_SLASHCOMMENTS))
         {
            mState = STATE_COMMENT;
            break;
         }
         else if(c == '[' && (mFlags & TF_BRACKETS))
         {
            mTokenType = TOKEN_BRACKETSTR;
            mState = STATE_INBRACKETS;
            break;
         }
         else if(c == '$') // detect $ keywords
            mTokenType = TOKEN_KEYWORD;
         else
            mTokenType = TOKEN_STRING;
         mState = STATE_INTOKEN;
         mToken += c;
         break;
   }
}

//
// True if it's alphanumeric or _
//
inline static bool XL_isIdentifierChar(char c)
{
   return isalnum(c) || c == '_';
}

// Scanning inside a token
void XLTokenizer::DoStateInToken()
{
   char c = mInput[mIndex];

   switch(c)
   {
      case '\n':
         if(mFlags & TF_LINEBREAKS) // if linebreaks are tokens, we need to back up
            --mIndex;
         // fall through
      case ' ':  // whitespace
      case '\t':
      case '\r':
         // end of token
         mState = STATE_DONE;
         break;
      case '\0':  // end of input -OR- start of a comment
      case ';':
         --mIndex;   // backup, next call will handle it in STATE_SCAN.
         mState = STATE_DONE;
         break;
      default:
         if(c == '#' && (mFlags & TF_HASHCOMMENTS))
         {
            // hashes may conditionally be supported as comments
            --mIndex;
            mState = STATE_DONE;
            break;
         }
         else if(c == '/' && mInput[mIndex + 1] == '/' &&
                 (mFlags & TF_SLASHCOMMENTS))
         {
            // double slashes may conditionally be supported as comments
            --mIndex;
            mState = STATE_DONE;
            break;
         }
         else if(mFlags & TF_OPERATORS && !mToken.empty() &&
                 XL_isIdentifierChar(c) != XL_isIdentifierChar(mToken[0]))
         {
            // operators and identifiers are separate
            --mIndex;
            mState = STATE_DONE;
            break;
         }
         mToken += c;
         break;
   }
}

// Reading out a bracketed string token
void XLTokenizer::DoStateInBrackets()
{
   switch(mInput[mIndex])
   {
      case ']':  // end of bracketed token
         mState = STATE_DONE;
         break;
      case '\0': // end of input (technically, malformed)
         --mIndex;
         mState = STATE_DONE;
         break;
      default:   // anything else is part of the token
         mToken += mInput[mIndex];
         break;
   }
}

// Reading out a quoted string token
void XLTokenizer::DoStateQuoted()
{
   char c;
   int i;

   switch(mInput[mIndex])
   {
      case '"':  // end of quoted string
         mState = STATE_DONE;
         break;
      case '\0': // end of input (technically, this is malformed)
         --mIndex;
         mState = STATE_DONE;
         break;
      case '\\':
         if(!(mFlags & TF_ESCAPESTRINGS))
         {
            mToken += mInput[mIndex];
            break;
         }
         // Increase IDX, check against '\0' for guarding.
         if(mInput[++mIndex] == '\0')
         {
            --mIndex;
            mState = STATE_DONE;
            break;
         }
         switch(mInput[mIndex])   // these correspond to the C escape sequences
      {
         case 'a':
            mToken += '\a';
            break;
         case 'b':
            mToken += '\b';
            break;
         case 'f':
            mToken += '\f';
            break;
         case 'n':
            mToken += '\n';
            break;
         case 't':
            mToken += '\t';
            break;
         case 'r':
            mToken += '\r';
            break;
         case 'v':
            mToken += '\v';
            break;
         case '?':
            mToken += '\?';
            break;
         case '\n':  // for escaping newlines
            break;
         case 'x':
         case 'X':
            c = 0;
            for(i = 0; i < 2; ++i)
            {
               mIndex++;
               if(mInput[mIndex] >= '0' && mInput[mIndex] <= '9')
                  c = (c << 4) + mInput[mIndex] - '0';
               else if(mInput[mIndex] >= 'a' && mInput[mIndex] <= 'f')
                  c = (c << 4) + 10 + mInput[mIndex] - 'a';
               else if(mInput[mIndex] >= 'A' && mInput[mIndex] <= 'F')
                  c = (c << 4) + 10 + mInput[mIndex] - 'A';
               else
               {
                  mIndex--;
                  break;
               }
            }
            mToken += c;
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
            c = mInput[mIndex] - '0';
            for(i = 0; i < 2; ++i)
            {
               mIndex++;
               if(mInput[mIndex] >= '0' && mInput[mIndex] <= '7')
                  c = (c << 3) + mInput[mIndex] - '0';
               else
               {
                  mIndex--;
                  break;
               }
            }
            mToken += c;
            break;
         default:
            mToken += mInput[mIndex];
            break;
      }
         break;
      default:
         mToken += mInput[mIndex];
         break;
   }
}

// Reading out a single-line comment
void XLTokenizer::DoStateComment()
{
   // consume all input to the end of the line
   if(mInput[mIndex] == '\n')
   {
      // if linebreak tokens are enabled, send one now
      if(mFlags & TF_LINEBREAKS)
      {
         mTokenType = TOKEN_LINEBREAK;
         mState = STATE_DONE;
      }
      else
         mState = STATE_SCAN;
   }
   else if(mInput[mIndex] == '\0') // end of input
   {
      mTokenType = TOKEN_EOF;
      mState     = STATE_DONE;
   }
}

//
// Parses one lump
//
void XLParser::ParseLump(const Lump &lump)
{
   if(lump.Data().empty())
      return;
   mCurLump = &lump;
   StartLump();
   XLTokenizer tokenizer(lump);
   bool early = false;
   InitTokenizer(tokenizer);
   while(tokenizer.GetNextToken() != XLTokenizer::TOKEN_EOF)
      if(!DoToken(tokenizer))
      {
         early = true;
         break;
      }
   OnEOF(early);
}

//
// Parses all lumps
//
void XLParser::ParseAll(const Wad &wad)
{
   mWad = &wad;
   if(mLumpName.empty())
      return;
   for(const Lump &lump : wad.Lumps())
   {
      if(strcasecmp(lump.Name(), mLumpName.c_str()))
         continue;
      ParseLump(lump);
   }
}
