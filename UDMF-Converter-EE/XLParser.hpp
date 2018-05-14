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

#ifndef XLParser_hpp
#define XLParser_hpp

#include <string>

class Lump;
class Wad;

//
// Tokenizer. Also from Eternity
//
class XLTokenizer
{
public:
   // Tokenizer states
   enum State
   {
      STATE_SCAN,       // scanning for a string token
      STATE_INTOKEN,    // in a string token
      STATE_INBRACKETS, // in a bracketed token
      STATE_QUOTED,     // in a quoted string
      STATE_COMMENT,    // reading out a comment (eat rest of line)
      STATE_DONE        // finished the current token
   };

   // Token types
   enum TokenType
   {
      TOKEN_NONE,       // Nothing identified yet
      TOKEN_KEYWORD,    // Starts with a $; otherwise, same as a string
      TOKEN_STRING,     // Generic string token; ex: 92 foobar
      TOKEN_EOF,        // End of input
      TOKEN_LINEBREAK,  // '\n' character, only a token when TF_LINEBREAKS is enabled
      TOKEN_BRACKETSTR, // bracketed string, when TF_BRACKETS is enabled
      TOKEN_ERROR       // An unknown token
   };

   // Tokenizer flags
   enum
   {
      TF_DEFAULT       = 0,          // default state (nothing special enabled)
      TF_LINEBREAKS    = 0x00000001, // line breaks are treated as tokens
      TF_BRACKETS      = 0x00000002, // supports [keyword] tokens
      TF_HASHCOMMENTS  = 0x00000004, // supports comments starting with # signs
      TF_SLASHCOMMENTS = 0x00000008, // supports double-slash comments
      TF_OPERATORS     = 0x00000010, // C-style identifiers, no space operators
      TF_ESCAPESTRINGS = 0x00000020, // Add support for escaping strings
   };

   explicit XLTokenizer(const Lump &lump);
   TokenType GetNextToken();

   // Accessors
   TokenType TokenType() const { return mTokenType; }
   const char *Token() const { return mToken.c_str(); }

   void SetTokenFlags(unsigned pFlags) { mFlags = pFlags; }

private:
   State mState = STATE_SCAN;  // state of the scanner
   std::string mInput;
   int mIndex = 0; // input string index
   enum TokenType mTokenType = TOKEN_NONE;   // type of current token
   std::string mToken;
   unsigned mFlags = TF_DEFAULT;

   void DoStateScan();
   void DoStateInToken();
   void DoStateInBrackets();
   void DoStateQuoted();
   void DoStateComment();

   // State table declaration
   static void (XLTokenizer::*States[])();
};

//
// Generic parser. Taken from Eternity and adapted for this project's classes
//
class XLParser
{
public:
   explicit XLParser(const char *lumpName) : mLumpName(lumpName)
   {
   }

   void ParseLump(const Lump &lump);
   void ParseAll(const Wad &wad);

   // Accessors
   const char *LumpName() const { return mLumpName.c_str(); }
   void LumpName(const char *s) { mLumpName = s; }

protected:
   virtual void StartLump() {}   // called at beginning of a new lump
   virtual void InitTokenizer(XLTokenizer &) {} // called before tokenization starts
   virtual bool DoToken(XLTokenizer &token) { return true; } // called for each token
   virtual void OnEOF(bool early) {} // called when EOF is reached

private:
   std::string mLumpName;
   const Lump *mCurLump = nullptr;
   const Wad *mWad = nullptr;
};

#endif /* XLParser_hpp */
