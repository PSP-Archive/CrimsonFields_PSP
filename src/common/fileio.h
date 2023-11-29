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
// fileio.h
////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_FILEIO_H
#define _INCLUDE_FILEIO_H

#if HAVE_DIRENT_H
# include <dirent.h>
#elif HAVE_SYS_NDIR_H
# include <sys/ndir.h>
#elif HAVE_SYS_DIR_H
# include <sys/dir.h>
#elif HAVE_NDIR_H
# include <ndir.h>
#elif defined WIN32
# include <windows.h>
#endif

#include <string>
using namespace std;

#include "SDL.h"
#include "SDL_endian.h"

#ifdef WIN32
#define PATHDELIM	'\\'
#define CRIMSONRC	"crimson.ini"
#else
#define PATHDELIM	'/'
#define CRIMSONRC	"crimsonrc"
#endif

#ifdef PSP
#define CURRENTDIR	"ms0:/PSP/GAME/CrimsonFields/"
#else
#define CURRENTDIR	"."
#endif

class Directory {
public:
  Directory( const char *dir );
  ~Directory( void );

  bool IsValid( void ) const
#ifdef WIN32
    { return m_Dir != INVALID_HANDLE_VALUE; }
#else
    { return m_Dir != NULL; }
#endif

  const char *GetFileName( void ) const;
  size_t GetFileNameLen() const;
  bool IsFileHidden( void ) const;
  bool NextFile( void );

private:
#ifdef WIN32
  HANDLE           m_Dir;
  WIN32_FIND_DATA  m_Entry;
# ifdef UNICODE
  char             m_AsciiDir[MAX_PATH];
# endif
#elif HAVE_DIRENT_H
  DIR    *m_Dir;
  dirent *m_Entry;
#else
  DIR    *m_Dir;
  direct *m_Entry;
#endif
};

// file abstraction to encapsulate SDL file access layer
class File {
public:
  File( const string &path ) : fh(0), name(path) {}
  ~File( void ) { if (fh) Close(); }

  bool Open( const char *mode, bool compressed = true );
  bool OpenData( const char *mode, const string &subdir = "", bool compressed = true );
  void Close( void );
  static bool Exists( const string &name );

  int Read( void *buffer, int size ) const
      { return SDL_RWread(fh, buffer, 1, size); }
  unsigned char Read8( void ) const;
  unsigned short Read16( void ) const { return SDL_ReadLE16(fh); }
  unsigned long Read32( void ) const { return SDL_ReadLE32(fh); }
  string ReadS( int size ) const;

  int Write( const void *values, int size ) const
      { return SDL_RWwrite(fh, values, size, 1); }
  int Write8( unsigned char value ) const
      { return SDL_RWwrite(fh, &value, 1, 1); }
  int Write16( unsigned short value ) const
      { return SDL_WriteLE16(fh, value); }
  int Write32( unsigned long value ) const
      { return SDL_WriteLE32(fh, value); }
  int WriteS( string value, int len = 0 ) const;

  const string &Name( void ) const { return name; }
  SDL_RWops *GetFD( void ) { return fh; }

private:
  SDL_RWops *fh;
  string name;
};

string get_config_dir( void );
string get_home_dir( void );
string get_save_dir( void );
string get_data_dir( void );
string get_data_subdir( const string &sub );
string get_sfx_dir( void );
string get_music_dir( void );
string get_levels_dir( void );
string get_locale_dir( void );
string get_home_levels_dir( void );
void create_config_dir( void );

string file_part( const string &path );
void append_path( string &path, const string &sub );
void append_path_delim( string &path );
void make_dir( const char *dir );

#endif	/* _INCLUDE_FILEIO_H */

