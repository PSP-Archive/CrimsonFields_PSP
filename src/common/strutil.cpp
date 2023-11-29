// Crimson Fields -- a game of tactical warfare
// Copyright (C) 2000-2005 Jens Granseuer
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

////////////////////////////////////////////////////////////////////////
// strutil.cpp - string utilities
////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "strutil.h"

////////////////////////////////////////////////////////////////////////
// NAME       : crypt
// DESCRIPTION: Encrypt/decrypt ASCII strings. The "algorithm" is really
//              simple and designed to prevent users from "accidentally"
//              browsing through data files with a hex editor.
// PARAMETERS : str - string to encrypt or decrypt
// RETURNS    : encrypted or decrypted string, depending on input
////////////////////////////////////////////////////////////////////////

string StringUtil::crypt( const string &str ) {
  string crypted;

  for ( string::const_iterator iter = str.begin();
        (iter != str.end()) && (*iter != '\0'); ++iter )
    crypted += ~(*iter);

  return crypted;
}

////////////////////////////////////////////////////////////////////////
// NAME       : strprintf
// DESCRIPTION: Replace the first occurence of %d in a printf-style
//              format string with the given input.
// PARAMETERS : format - printf-style formatting template
//              arg    - value to insert
// RETURNS    : formatted string
////////////////////////////////////////////////////////////////////////

string StringUtil::strprintf( const string &format, int arg ) {
  string result( format );
  size_t pos = result.find( "%d", 0 );

  if ( pos != string::npos )
    result.replace( pos, 2, tostring(arg) );

  return result;
}

////////////////////////////////////////////////////////////////////////
// NAME       : strprintf
// DESCRIPTION: Replace the first occurence of %s in a printf-style
//              format string with the given input.
// PARAMETERS : format - printf-style formatting template
//              arg    - value to insert
// RETURNS    : formatted string
////////////////////////////////////////////////////////////////////////

string StringUtil::strprintf( const string &format, const string &arg ) {
  string result( format );
  size_t pos = result.find( "%s", 0 );

  if ( pos != string::npos )
    result.replace( pos, 2, arg );

  return result;
}

////////////////////////////////////////////////////////////////////////
// NAME       : tostring
// DESCRIPTION: Convert an integer to a string.
// PARAMETERS : number - integer to convert
// RETURNS    : number as string
////////////////////////////////////////////////////////////////////////

string StringUtil::tostring( int number ) {
  stringstream numstr;

  numstr << number;
  return numstr.str();
}

