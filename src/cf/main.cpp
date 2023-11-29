// Crimson Fields -- a hex-based game of tactical warfare
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

/////////////////////////////////////////////////////////////////////
// main.cpp -- Crimson Fields
/////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#ifndef _WIN32_WCE
# include <time.h>
#endif

#include "SDL.h"

#include "view.h"
#include "game.h"
#include "misc.h"
#include "fileio.h"
#include "initwindow.h"
#include "globals.h"
#include "sound.h"
#include "options.h"
#include "strutil.h"
#include "msgs.h"

#ifdef PSP
#include "SDL_fakemouse.h"
#endif

// global vars
Game *Gam;
Image *Images[NUM_IMAGES] = { NULL };
Language Lang;

// global options, can be accessed from everywhere
Options CFOptions;

struct GUIOptions {
  short px_width;
  short px_height;
  short bpp;
  bool sfx;
  bool music;
  unsigned char sfx_vol;
  unsigned char music_vol;
  unsigned long sdl_flags;
  const char *level;
};

// local vars
static View *display;

// local function prototypes
static void parse_options( int argc, char **argv, GUIOptions &opts );
static void print_usage( char *prog );
static View *init( GUIOptions &opts );
static bool init_locale( void );
static void load_settings( GUIOptions &opts );
static void save_settings( View *display );
static void set_icon( const Surface &s, const Rect &icon );
static GUI_Status event_filter( SDL_Event &event, Window *window );
static void do_exit( void );

#ifdef _WIN32_WCE
static bool init_wince( struct GUIOptions &opts );
# define time(n) GetTickCount()
#endif

// Start of program functions //////////////////////////////////////////

	
#ifdef PSP

#ifndef PSP
#undef main
#else
/* On the PSP we need main defined as SDL_main. */
extern "C"
#endif
int main(int argc, char *argv[]) {

#else

#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR szCmdLine, int sw) {
#else
int main( int argc, char **argv ) {

#endif

#endif

#ifdef PSP
  struct GUIOptions guiopts = {480, 272, 24, true, true,
                    MIX_MAX_VOLUME*3/4, MIX_MAX_VOLUME/2, SDL_HWSURFACE, NULL };
#else
  struct GUIOptions guiopts = { 800, 600, DISPLAY_BPP, true, true,
                    MIX_MAX_VOLUME*3/4, MIX_MAX_VOLUME/2, SDL_HWSURFACE, NULL };
#endif

#ifndef PSP	

#ifdef _WIN32_WCE
  if ( !init_wince( guiopts ) ) return 0;
#else
  parse_options( argc, argv, guiopts );
#endif

#endif
  
  display = init( guiopts );
  if ( display ) {

    // only open intro screen if the user didn't supply a level on the command line
    int intro = 1;
 	
    if ( guiopts.level ) {
      Gam = new Game( display );
      intro = Gam->Load( guiopts.level );
      Mission *m = Gam->GetMission();
      if ( !intro ) {
        // default is to play single-player single-map
        if ( !(m->GetFlags() & GI_SAVEFILE) )
          m->GetPlayer(PLAYER_ONE).SetType( HUMAN );
        Gam->InitWindows();
        Gam->StartTurn();
      } else {
        delete Gam;
        Gam = NULL;
        intro = 1;
      }
    }

    GUI_Status status;
    do {
      if ( intro ) {
        display->Refresh();
        TitleWindow *twin = new TitleWindow( display );

        // preload main window, levels, etc.
        InitWindow *iwin = new InitWindow( display, twin );

        // if title image was successfully loaded, wait for click
        if ( twin->Width() > 0 ) {
          display->SelectWindow( twin );
          twin->EventLoop();
          display->SelectWindow( iwin );
        }

        iwin->Show();
      } else intro = 1;

      do {
        status = display->HandleEvents();
      } while ( (status != GUI_QUIT) && (status != GUI_RESTART) );

      delete Gam;
      Gam = NULL;
      display->CloseAllWindows();
    } while ( status != GUI_QUIT );
  }

  do_exit();
  return 0;
}

////////////////////////////////////////////////////////////////////////
// NAME       : parse_options
// DESCRIPTION: Process any options given to the program on the command
//              line.
// PARAMETERS : argc - argument count
//              argv - pointer to array of arguments
//              opts - buffer to store GUI and audio options; should be
//                     initialized with defaults before calling this
//                     method
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

// parse command line arguments
void parse_options( int argc, char **argv, GUIOptions &opts ) {
  load_settings( opts );

  while ( argc > 1 ) {
    --argc;

    if (strcmp(argv[argc-1], "--width") == 0) {
      opts.px_width = atoi(argv[argc]);
    } else if (strcmp(argv[argc-1], "--height") == 0) {
      opts.px_height = atoi(argv[argc]);
    } else if (strcmp(argv[argc-1], "--level") == 0) {
      opts.level = argv[argc];
    } else if (strcmp(argv[argc-1], "--fullscreen") == 0) {
      if ( atoi( argv[argc] ) ) opts.sdl_flags |= SDL_FULLSCREEN;
      else opts.sdl_flags &= ~SDL_FULLSCREEN;
    } else if (strcmp(argv[argc-1], "--sound") == 0) {
      if ( atoi( argv[argc] ) ) opts.sfx = opts.music = true;
      else opts.sfx = opts.music = false;
    } else {
      if (strcmp(argv[argc], "--version") == 0)
        cout << PROGRAMNAME" "VERSION << endl;
      else print_usage( argv[0] );
      exit ( 0 );
    }
    --argc;
  }
  if ( opts.px_width < MIN_XRES ) opts.px_width = MIN_XRES;
  if ( opts.px_height < MIN_YRES ) opts.px_height = MIN_YRES;
}

////////////////////////////////////////////////////////////////////////
// NAME       : print_usage
// DESCRIPTION: Print a usage message to stdout.
// PARAMETERS : prog - program name as given on the command line
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void print_usage( char *prog ) {
  cout << "Usage: " << prog << " [options]" << endl << endl
            << "Available options:" << endl
            << "  --level <level>      load level or save file" << endl
            << "  --width <width>      set screen width" << endl
            << "  --height <height>    set screen height" << endl
            << "  --fullscreen <1|0>   enable/disable fullscreen mode" << endl
#ifndef DISABLE_SOUND
            << "  --sound <1|0>        enable/disable sound" << endl
#endif
            << "  --help               display this help and exit" << endl
            << "  --version            output version information and exit" << endl;
}

////////////////////////////////////////////////////////////////////////
// NAME       : init
// DESCRIPTION: Initialize the display, the system icons surface and the
//              game fonts.
// PARAMETERS : opts - contains the user settings for the sound and
//                     graphics subsystems
// RETURNS    : a pointer to the display surface on success, NULL
//              otherwise
////////////////////////////////////////////////////////////////////////

View *init( GUIOptions &opts ) {
  View *view = NULL;
  Font *f1 = NULL, *f2 = NULL;
  Surface *icons = NULL;
  bool ok = false;

	#ifdef PSP
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) >= 0)
	{
		opts.sfx = true;
		opts.music = true;
		
	#else
  if ( SDL_Init( SDL_INIT_VIDEO ) >= 0 ) {
	#endif

    // load locale
    if ( !init_locale() ) return NULL;

    string datpath( get_data_dir() );
    datpath.append( CF_DATFILE );
    File datfile( datpath );
    if ( datfile.Open( "rb" ) ) {

      icons = new Surface;		// load icons surface
      if ( !icons->LoadImageData( datfile ) ) {

        set_icon( *icons, Rect(64, 0, 32, 32) );

        view = new View( opts.px_width, opts.px_height, opts.bpp,
                         opts.sdl_flags );
		
		#ifdef PSP
		SDL_JoystickEventState(SDL_ENABLE);
		
		if(SDL_NumJoysticks())
		{
			view->m_joystick = SDL_JoystickOpen(0);
			fakemouse_init();
			
			if(view->s_surface && !TTF_Init())
			{
				// load fonts

				datpath = get_data_dir() + CF_FONT;
				f1 = new Font();
				f2 = new Font();
        
       			if(!f1->Load(datpath.c_str(), CF_FONT_SMALL) && !f2->Load(datpath.c_str(), CF_FONT_LARGE))
       			{
					ok = true;
       			}
				else
				{
					cerr << "Error: Couldn't load font " << datpath << endl;
				}
			}
			else
			{
				cerr << "Error: Couldn't set video mode (" << SDL_GetError() << ')' << endl;
			}
		}
		else
		{
			cerr << "Error: Couldn't not initialize joystick (" << SDL_GetError() << ')' << endl;
		}
		#else
        if ( view->s_surface && !TTF_Init()) {	// load fonts

          datpath = get_data_dir() + CF_FONT;
          f1 = new Font();
          f2 = new Font();
          if ( !f1->Load( datpath.c_str(), CF_FONT_SMALL ) &&
               !f2->Load( datpath.c_str(), CF_FONT_LARGE ) )
            ok = true;
          else cerr << "Error: Couldn't load font " << datpath << endl;
        } else cerr << "Error: Couldn't set video mode (" << SDL_GetError() << ')' << endl;
		#endif
		
      } else cerr << "Error: Couldn't read data file" << endl;
    } else cerr << "Error: Couldn't open " << datpath << endl;
  } else cerr << "Error: Couldn't initialize ( " << SDL_GetError() << ')' << endl;

  if ( ok ) {
    // set main window title
    SDL_WM_SetCaption( PROGRAMNAME, PROGRAMNAME );
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );

    srand( time(0) );				// initialize random number generator

    Audio::InitSfx( opts.sfx, opts.sfx_vol );	// load sound effects
    Audio::InitMusic( opts.music, opts.music_vol );

    for ( int i = 0; i <= XP_MAX_LEVEL; ++i )
      Images[ICON_XP_BASE+i] = new Image( icons, 64 + i * XP_ICON_WIDTH, 32, XP_ICON_WIDTH, XP_ICON_HEIGHT );

    icons->DisplayFormat();

    view->SetSmallFont( f1 );
    view->SetLargeFont( f2 );
    view->SetFGPen( Color(CF_COLOR_HIGHLIGHT) );
    view->SetBGPen( Color(CF_COLOR_SHADOW) );
    view->SetEventFilter( event_filter );
    view->SetSystemIcons( icons );

    // create options and save games directories if possible
    create_config_dir();

    return view;
  } else {
    delete icons;
    delete view;
    delete f1;
    delete f2;
    return NULL;
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : init_locale
// DESCRIPTION: Load the strings from a file.
// PARAMETERS : -
// RETURNS    : TRUE on success, FALSE otherwise
////////////////////////////////////////////////////////////////////////

bool init_locale( void ) {
  bool rc = false, retry = false;

  const string ldir( get_locale_dir() );
  string lid( CFOptions.GetLanguage() );

  // try the user locale first, and use the default if that fails
  do {
    string lfile( ldir + lid + ".dat" );
    short num = Lang.ReadCatalog( lfile.c_str() );
    if ( num == -1 ) {
      cerr << "Error: Couldn't load language resources for language '"
                << lid << "'" << endl;
    } else if ( num != CF_MSGS ) {
      cerr << "Error: Language catalog for '" << lid << "' contains "
                << num << " strings, expected " << CF_MSGS << endl;
    } else rc = true;

    // only retry once
    if ( (rc == false) && (retry == false) &&
         (lid != CF_LANG_DEFAULT) ) {
      lid.assign( CF_LANG_DEFAULT );
      cerr << "Using default locale (" << lid << ") instead" << endl;
      retry = true;
    } else {
      retry = false;

      if ( rc == true ) {
        // change the language setting if necessary
        if ( lid != CFOptions.GetLanguage() )
          CFOptions.SetLanguage( lid.c_str() );
      }
    }
  } while ( retry );

  return rc;
}

////////////////////////////////////////////////////////////////////////
// NAME       : load_settings
// DESCRIPTION: Read default display settings from the crimsonrc file.
// PARAMETERS : opts - buffer to store the settings. These should
//                     already be initialized with some defaults in
//                     case the rc file doesn't exist or this function
//                     fails.
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void load_settings( GUIOptions &opts ) {
 string crimsonrc( get_config_dir() );
  crimsonrc.append( CRIMSONRC );

  ifstream file( crimsonrc.c_str() );
  if ( file.is_open() ) {
    string buf;
    const char *val, *linebuf;
    unsigned int line = 0;
    size_t pos;


    while (!file.eof()) {
      getline(file, buf);
      ++line;

      if ( buf.size() > 0 ) { // ignore empty lines
        pos = buf.find( ' ' );
        if ( pos != string::npos ) {
          linebuf = buf.c_str();
          val = &linebuf[pos+1];
          while ( *val == ' ' ) ++val;

          if ( !strncmp( linebuf, "width", 5 ) ) opts.px_width = atoi(val);
          else if ( !strncmp( linebuf, "height", 6 ) ) opts.px_height = atoi(val);
          else if ( !strncmp( linebuf, "fullscreen", 10 ) ) {
            if ( atoi(val) != 0 ) opts.sdl_flags |= SDL_FULLSCREEN;
          } else if ( !strncmp( linebuf, "sfxvol", 6 ) ) opts.sfx_vol = atoi(val);
          else if ( !strncmp( linebuf, "musicvol", 8 ) ) opts.music_vol = atoi(val);
          else if ( !strncmp( linebuf, "sfx", 3 ) ) opts.sfx = (atoi(val) != 0);
          else if ( !strncmp( linebuf, "music", 5 ) ) opts.music = (atoi(val) != 0);
          else if ( !strncmp( linebuf, "locale", 6 ) ) CFOptions.SetLanguage(val);
          else if ( !strncmp( linebuf, "showdamage", 10 ) ) CFOptions.SetDamageIndicator( atoi(val) != 0 );
          else if ( !strncmp( linebuf, "unlock", 6 ) ) CFOptions.Unlock( val );
          else if ( !strncmp( linebuf, "showreplay", 10 ) ) {
            int rep = atoi(val);
            CFOptions.SetTurnReplay( rep != 0 );
            CFOptions.SetQuickReplay( rep == 1 );
          } else cerr << "Warning: unrecognized config option in line " << line << endl;
        }
      }
    }
    file.close();
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : save_settings
// DESCRIPTION: Save current display settings to the crimsonrc file.
// PARAMETERS : display - pointer to display
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void save_settings( View *display ) {
 string crimsonrc( get_config_dir() );
  crimsonrc.append( CRIMSONRC );

  ofstream file( crimsonrc.c_str() );
  if ( file.is_open() ) {
    file << "width " << display->Width() << '\n';
    file << "height " << display->Height() << '\n';
    file << StringUtil::strprintf("fullscreen %d", display->IsFullScreen()) << '\n';
    file << StringUtil::strprintf("sfx %d", Audio::GetSfxState()) << '\n';
    file << StringUtil::strprintf("music %d", Audio::GetMusicState()) << '\n';
    file << StringUtil::strprintf("sfxvol %d", Audio::GetSfxVolume()) << '\n';
    file << StringUtil::strprintf("musicvol %d", Audio::GetMusicVolume()) << '\n';
    file << "locale " << CFOptions.GetLanguage() << '\n';
    file << StringUtil::strprintf("showdamage %d", CFOptions.GetDamageIndicator()) << '\n';
    file << StringUtil::strprintf("showreplay %d",
            CFOptions.GetTurnReplay() ? (CFOptions.GetQuickReplay() ? 1 : 2) : 0 ) << '\n';

    const vector<string> &maps = CFOptions.GetUnlockedMaps();
    for ( vector<string>::const_iterator i = maps.begin(); i != maps.end(); ++i )
      file << "unlock " << *i << '\n';

    file.close();
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : set_icon
// DESCRIPTION: Set the application icon.
// PARAMETERS : s    - icon surface
//              icon - icon position and size on the surface
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void set_icon( const Surface &s, const Rect &icon ) {
 Surface is;
  is.Create( icon.Width(), icon.Height(), DISPLAY_BPP, 0 );
  is.SetColorKey( s.GetColorKey() );
  is.Flood( is.GetColorKey() );
  s.Blit( &is, icon, 0, 0 );

  SDL_WM_SetIcon( is.s_surface, NULL );
}

////////////////////////////////////////////////////////////////////////
// NAME       : event_filter
// DESCRIPTION: This is the global event filter function. It is hooked
//              to the display and called everytime the event handler
//              receives an event.
// PARAMETERS : event  - event received by the event handler
//              window - pointer to the currently active window
// RETURNS    : GUI_Status; if the filter returns GUI_NONE the event
//              handler will not pass the event to its windows, but
//              silently drop it.
////////////////////////////////////////////////////////////////////////

GUI_Status event_filter( SDL_Event &event, Window *window ) {

#ifdef PSP
	GUI_Status rc = GUI_OK;

	if(event.type == SDL_KEYDOWN)
	{
		// send to windows
		rc = GUI_OK;
    }
	else if(event.type == SDL_JOYBUTTONDOWN)
	{
		// send to windows
		rc = GUI_OK;
    }
	else if(event.type == SDL_QUIT)
	{
		do_exit();
	}
#else
 GUI_Status rc = GUI_OK;

  if ( event.type == SDL_KEYDOWN ) {
    rc = GUI_NONE;

    switch ( event.key.keysym.sym ) {
    case SDLK_F11:            // toggle sound
      Audio::ToggleSfxState();
      break;
    default:
      rc = GUI_OK;            // send to windows
    }
  } else if ( event.type == SDL_QUIT ) do_exit();
#endif
  return rc;
  
}

////////////////////////////////////////////////////////////////////////
// NAME       : init_wince
// DESCRIPTION: Initialize on Windows CE.
// PARAMETERS : opts - program options buffer (containing defaults)
// RETURNS    : TRUE if initialization succeeded, FALSE otherwise
////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
bool init_wince( struct GUIOptions &opts ) {
  HWND hwnd = FindWindow( _T("SDL_app"), _T(PROGRAMNAME) );
  if ( hwnd ) {
    SetForegroundWindow( hwnd );
    ShowWindow( hwnd, SW_SHOW );
    return false;
  }

  load_settings( opts );

  unsigned short maxw = GetSystemMetrics(SM_CXSCREEN); 
  unsigned short maxh = GetSystemMetrics(SM_CYSCREEN); 

  if( opts.px_width > maxw ) opts.px_width = maxw;
  if( opts.px_height > maxh ) opts.px_height = maxh;

  opts.sdl_flags |= SDL_FULLSCREEN;
  return true;
}
#endif

////////////////////////////////////////////////////////////////////////
// NAME       : do_exit
// DESCRIPTION: Free all resources and exit the program.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void do_exit( void ) {
 delete Gam;
	#ifdef PSP

	if(display->m_joystick)
	{
		fakemouse_finish();
		SDL_JoystickClose((SDL_Joystick *)display->m_joystick);
	}

	#endif

  if ( display ) {
    save_settings( display );
    delete display;
  }

  Audio::ShutdownSfx();
  Audio::ShutdownMusic();

  for ( int i = 0; i < NUM_IMAGES; ++i ) delete Images[i];

  TTF_Quit();
  SDL_Quit();
  exit( 0 );
}

