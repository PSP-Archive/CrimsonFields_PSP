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

///////////////////////////////////////////////////////////////
// globals.h - global variable declarations
// This file can be included by both C and C++ code.
///////////////////////////////////////////////////////////////

#ifndef _INCLUDE_GLOBALS_H
#define _INCLUDE_GLOBALS_H

/* data paths and files
 * the symbol VERSION should be defined externally (usually via autoconf)
 * the symbol CF_DATADIR may be defined externally to define the location
 *        of data files. If it is not defined the program will try to
 *        locate the files at runtime (default for Win32).
 */

#ifndef VERSION
# define VERSION "0.4.9"
#endif

#define CF_SHORTNAME	"crimson"
#define CF_DATFILE	"cf.dat"
#define CF_TITLE_SCREEN	"title.bmp"

#define CF_FONT		"Bepa-Roman.ttf"
#define CF_FONT_SMALL	12
#define CF_FONT_LARGE	16

#define CF_MUSIC_THEME		"theme"
#define CF_MUSIC_DEFAULT	"default"
#define CF_MUSIC_FADE_TIME	2000

#define FILE_VERSION  13
#define FID_MISSION   MakeID('M','S','S','N')  /* mission file identifier */

#define DISPLAY_BPP	16	/* display depth */

#define MIN_XRES	320
#define MIN_YRES	240

#ifdef _WIN32_WCE
# define strcasecmp _stricmp
# define strncasecmp _strnicmp
#else
# ifndef HAVE_STRCASECMP
#  define strcasecmp stricmp
# endif
# ifndef HAVE_STRNCASECMP
#  define strncasecmp strnicmp
# endif
#endif

#endif	/* _INCLUDE_GLOBALS_H */

