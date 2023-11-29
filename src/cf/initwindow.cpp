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
// initwindow.cpp
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <iostream>

#include "initwindow.h"
#include "filewindow.h"
#include "misc.h"
#include "game.h"
#include "strutil.h"
#include "options.h"
#include "msgs.h"

#ifdef PSP
#include "SDL_fakemouse.h"
#endif

#define IW_WIDGET_TYPE		0
#define IW_WIDGET_MAPTYPE	1
#define IW_WIDGET_LEVELS	2
#define IW_WIDGET_START		3
#define IW_WIDGET_OPTIONS	4
#define IW_WIDGET_GENERAL_OPTIONS  5
#define IW_WIDGET_LANGUAGE_OPTIONS 6
#define IW_WIDGET_VIDEO_OPTIONS    7
#define IW_WIDGET_SOUND_OPTIONS    8
#define IW_WIDGET_DIFFICULTY	9

#define MAPTYPE_SINGLE   0
#define MAPTYPE_CAMPAIGN 1
#define MAPTYPE_SAVE     2

extern Options CFOptions;

const short GenericOptionsWindow::B_ID_OK = 1;

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::InitWindow
// DESCRIPTION: Create the main menu window. The player(s) can select
//              the level they want to play and set other options.
// PARAMETERS : view  - view to attach the window to
//              title - pointer to title window; must be closed when this
//                      window is closed
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

InitWindow::InitWindow( View *view, Window *title ) :
    Window( WIN_CENTER, view ), title(title) {
  // read list of maps/saves
  const string home_lev( get_home_levels_dir() );
  if ( !home_lev.empty() )
    FileWindow::CreateFilesList( home_lev.c_str(), ".lev", levels );
  FileWindow::CreateFilesList( get_levels_dir().c_str(), ".lev", levels );
  CompleteFilesList( levels );
  FileWindow::CreateFilesList( get_save_dir().c_str(), ".sav", saves );
  CompleteFilesList( saves );

  Audio::PlayMusic( CF_MUSIC_THEME );

  view->SetFGPen( Color( 0x00d87c00 ) );
  view->SetBGPen( Color( CF_COLOR_BLACK ) );

  Rebuild();
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::~InitWindow
// DESCRIPTION: Destroy window.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

InitWindow::~InitWindow( void ) {
  TLWNode *n;
  string *str;

  while ( !levels.IsEmpty() ) {
    n = static_cast<TLWNode *>( levels.RemHead() );
    str = (string *)n->UserData();
    delete str;
    delete n;
  }

  while ( !campaigns.IsEmpty() ) {
    n = static_cast<TLWNode *>( campaigns.RemHead() );
    str = (string *)n->UserData();
    delete str;
    delete n;
  }

  while ( !saves.IsEmpty() ) {
    n = static_cast<TLWNode *>( saves.RemHead() );
    str = (string *)n->UserData();
    delete str;
    delete n;
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::Rebuild
// DESCRIPTION: (Re)create all window content. This is used both in the
//              constructor and when the window size has changed.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void InitWindow::Rebuild( void ) {
  RemoveAllWidgets();

  // calculate window dimensions
  const char *typelbl = MSG(MSG_B_GAME_TYPE);
  const char *mapslbl = MSG(MSG_B_MAP_TYPE);
  const char *hcaplbl = MSG(MSG_B_HANDICAP);
  unsigned short typewidth = sfont->TextWidth(typelbl);
  unsigned short mapswidth = sfont->TextWidth(mapslbl);
  unsigned short wdh = sfont->Height() + 8;

  unsigned short xoff = MAX( typewidth, mapswidth ) - sfont->CharWidth('_') + 10;

  SetSize( MIN(view->Width(), xoff + sfont->Width() * 40 + 20),
           MIN(view->Height(), wdh * 12 + 10 ) );

  // create widgets
  Widget *wd;

  const char *typelabels[4];
  typelabels[0] = MSG(MSG_GAME_HOT_SEAT);
  typelabels[1] = MSG(MSG_GAME_AI);
  typelabels[2] = MSG(MSG_GAME_PBEM);
  typelabels[3] = 0;

  const char *mapslabels[4];
  mapslabels[0] = MSG(MSG_MAPS_SINGLES);
  mapslabels[1] = MSG(MSG_MAPS_CAMPAIGNS);
  mapslabels[2] = MSG(MSG_MAPS_SAVES);
  mapslabels[3] = 0;

  const char *difflabels[4];
  difflabels[0] = MSG(MSG_HANDICAP_NONE);
  difflabels[1] = MSG(MSG_HANDICAP_P1);
  difflabels[2] = MSG(MSG_HANDICAP_P2);
  difflabels[3] = 0;

  gtypewidget = new CycleWidget( IW_WIDGET_TYPE, xoff, 5,
                (w - xoff - 10) / 2, wdh, 0,
                typelbl, CFOptions.GetGameType(), typelabels, this );
  gtypewidget->SetHook( this );

  xoff = gtypewidget->LeftEdge() + gtypewidget->Width() + 5;
  wd = new DropWidget( IW_WIDGET_OPTIONS, xoff, gtypewidget->TopEdge(),
           w - xoff - 5, wdh, 0, MSG(MSG_B_OPTIONS), this );
  wd->SetHook( this );

  mtypewidget = new CycleWidget( IW_WIDGET_MAPTYPE,
           gtypewidget->LeftEdge(), gtypewidget->TopEdge() + wdh + 5,
           gtypewidget->Width(), wdh, 0, mapslbl, 0, mapslabels, this );
  mtypewidget->SetHook( this );
  CFOptions.SetCampaign( false );

  xoff += sfont->TextWidth( hcaplbl ) - sfont->CharWidth( '_' ) + 5;
  diffwidget = new CycleWidget( IW_WIDGET_DIFFICULTY,
           xoff, mtypewidget->TopEdge(), w - xoff - 5, wdh,
           0, hcaplbl, 0, difflabels, this );

  levwidget = new TextListWidget( IW_WIDGET_LEVELS,
                    5, mtypewidget->TopEdge() + wdh + 5,
                    w/2 - 7, h - (wdh + 5) * 3 - 5, &levels,
                    -1, WIDGET_HSCROLLKEY|WIDGET_VSCROLLKEY,
                    NULL, this );
  levwidget->SetHook( this );

  maxmap = Rect( w/2 + 2, levwidget->TopEdge(), w/2 - 7, levwidget->Height() );

  // of the these two widgets, only one is actually present at a time
  mapwidget = new MapWidget( 0, maxmap.x, maxmap.y, maxmap.w, maxmap.h,
              0, this );
  campinfowidget = new TextScrollWidget( 0, maxmap.x, maxmap.y,
              maxmap.w, maxmap.h, 0, WIDGET_HIDDEN, 0, this );

  wd = new ButtonWidget( IW_WIDGET_START, 5, h - wdh - 2, (w - 10) / 2 - 2, wdh,
                WIDGET_DEFAULT, MSG(MSG_B_START), this );
  wd->SetHook( this );

  wd = new ButtonWidget( GUI_QUIT, wd->LeftEdge() + wd->Width() + 4,
                wd->TopEdge(), wd->Width(), wdh,
                0, MSG(MSG_B_EXIT), this );
  Draw();
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::VideoModeChange
// DESCRIPTION: This method is called by the view whenever the video
//              resolution changes. We can then resize the window and
//              widgets accordingly.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void InitWindow::VideoModeChange( void ) {
  Rebuild();
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::WidgetActivated
// DESCRIPTION: Handle activation of widgets in the window.
// PARAMETERS : button - calling widget
//              win    - pointer to active window
// RETURNS    : GUI status
////////////////////////////////////////////////////////////////////////

GUI_Status InitWindow::WidgetActivated( Widget *button, Window *win ) {
  TLWNode *n;
  GUI_Status rc = GUI_OK;

  switch ( button->ID() ) {
  case IW_WIDGET_LEVELS:  // user selected mission, show map
    n = static_cast<TLWNode *>( levwidget->Selected() );
    if ( n ) {
      string *lname = (string *)n->UserData();
      Mission *m = LoadMission( lname->c_str() );
      if ( m ) {
        if ( mtypewidget->GetValue() == MAPTYPE_CAMPAIGN ) {
          campinfowidget->SetText( m->GetCampaignInfo() );
        } else {
          Map &map = m->GetMap();
          unsigned char magnify = MIN( maxmap.Width() / map.Width(),
                                       maxmap.Height() / map.Height() );
          if ( magnify == 0 ) magnify = 1;
          else if ( magnify > 6 ) magnify = 6;

          mapwidget->w = magnify * map.Width() + 2;
          mapwidget->h = magnify * map.Height() + 2 + magnify/2;
          mapwidget->Center( maxmap );
          mapwidget->Clip( maxmap );

          mapwidget->SetPlayerColors( m->GetPlayer(PLAYER_ONE).LightColor(),
                                      m->GetPlayer(PLAYER_TWO).LightColor() );
          mapwidget->SetMap( &map, Rect(0,0,250,250), magnify );
        }
        delete m;
        Draw();
        Show();
      }
    }
    break;
  case IW_WIDGET_START:
    n = static_cast<TLWNode *>( levwidget->Selected() );
    if ( !n ) new NoteWindow( MSG(MSG_ERROR), MSG(MSG_ERR_NO_MAP), WIN_CLOSE_ESC, view );
    else {
      string *lname = (string *)n->UserData();

      Gam = new Game( view );
      int val = Gam->Load( lname->c_str() );
      if ( val  ) {
        NoteWindow *nw = new NoteWindow( MSG(MSG_ERROR), MSG(MSG_ERR_LOAD_MAP), 0, view );
        nw->SetButtonID( 0, GUI_RESTART );
      } else {
        Mission *m = Gam->GetMission();
        Audio::StopMusic( CF_MUSIC_FADE_TIME );
        view->CloseWindow( this );

        switch (mtypewidget->GetValue() ) {
        case MAPTYPE_SINGLE:
          if ( CFOptions.IsAI() ) {
            string pnames( "[_1] " );
            pnames += m->GetPlayer(PLAYER_ONE).Name();
            pnames += "|[_2] ";
            pnames += m->GetPlayer(PLAYER_TWO).Name();

            DialogWindow *pwin = new DialogWindow( MSG(MSG_PLAYER_SELECTION),
                  MSG(MSG_ASK_SIDE), pnames, 0, 0, view );
            pwin->SetButtonID( 0, PLAYER_ONE );
            pwin->SetButtonID( 1, PLAYER_TWO );
            short rc = pwin->EventLoop();
            view->CloseWindow( pwin );
            m->GetPlayer( rc ).SetType( HUMAN );
          } else {
            m->GetPlayer(PLAYER_ONE).SetType( HUMAN );
            m->GetPlayer(PLAYER_TWO).SetType( HUMAN );
          }
          break;
        case MAPTYPE_CAMPAIGN:
          m->GetPlayer(PLAYER_ONE).SetType( HUMAN );
          break;
        }

        if ( mtypewidget->GetValue() != MAPTYPE_SAVE )
          m->SetHandicap( 1 << diffwidget->GetValue() );

        view->Flood( Color(CF_COLOR_SHADOW) );
        view->Update();
        Gam->InitWindows();
        rc = Gam->StartTurn();         // let the games begin...
      }
    }
    break;
  case IW_WIDGET_MAPTYPE:
    switch ( mtypewidget->GetValue() ) {
    case MAPTYPE_SINGLE:
      mapwidget->SetMap( 0, Rect(0,0,0,0), 0 );
      mapwidget->SetSize( maxmap.x, maxmap.y, maxmap.w, maxmap.h );
      levwidget->SwitchList( &levels, -1 );
      gtypewidget->Enable();
      diffwidget->Enable();
      campinfowidget->Hide();
      mapwidget->Unhide();
      CFOptions.SetCampaign( false );
      break;
    case MAPTYPE_CAMPAIGN:
      levwidget->SwitchList( &campaigns, -1 );
      gtypewidget->Disable();
      diffwidget->Enable();
      campinfowidget->Unhide();
      mapwidget->Hide();
      CFOptions.SetCampaign( true );
      break;
    case MAPTYPE_SAVE:
      mapwidget->SetMap( 0, Rect(0,0,0,0), 0 );
      mapwidget->SetSize( maxmap.x, maxmap.y, maxmap.w, maxmap.h );
      levwidget->SwitchList( &saves, -1 );
      diffwidget->Disable();
      gtypewidget->Disable();
      campinfowidget->Hide();
      mapwidget->Unhide();
      CFOptions.SetCampaign( false );
      break;
    }

    Draw();
    Show();
    break;

  case IW_WIDGET_OPTIONS:{
    MenuWindow *menu = new MenuWindow( 0, this, view );
    menu->AddItem( 0, IW_WIDGET_GENERAL_OPTIONS, 0, MSG(MSG_B_OPT_GENERAL) );
    menu->AddItem( 0, IW_WIDGET_LANGUAGE_OPTIONS, 0, MSG(MSG_B_OPT_LANGUAGE) );
    menu->AddItem( 0, IW_WIDGET_VIDEO_OPTIONS, 0, MSG(MSG_B_OPT_VIDEO) );
#ifndef DISABLE_SOUND
    menu->AddItem( 0, IW_WIDGET_SOUND_OPTIONS, 0, MSG(MSG_B_OPT_AUDIO) );
#endif
    menu->SetPosition(
      Point( x + button->LeftEdge() + 1,
             y + button->TopEdge() + button->Height() - 1 )
    );
    menu->SetMinWidth( button->Width() - 2 );
    menu->Layout();
    break; }

  case IW_WIDGET_TYPE:
    CFOptions.SetGameType( gtypewidget->GetValue() );
    break;
  case IW_WIDGET_GENERAL_OPTIONS:
    view->CloseWindow( win );
    new GeneralOptionsWindow( 0, view );
    break;
  case IW_WIDGET_VIDEO_OPTIONS:
    view->CloseWindow( win );
    new VideoOptionsWindow( view );
    break;
  case IW_WIDGET_LANGUAGE_OPTIONS:
    view->CloseWindow( win );
    new LocaleOptionsWindow( 0, view );
    break;
#ifndef DISABLE_SOUND
  case IW_WIDGET_SOUND_OPTIONS:
    view->CloseWindow( win );
    new SoundOptionsWindow( view );
    break;
#endif
  }

  return rc;
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::CompleteFilesList
// DESCRIPTION: For the levels list display we need some information
//              about the maps (campaign info, 1 or 2 players). This
//              information is attached to the list items.
// PARAMETERS : list - list of files created using create_files_list()
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void InitWindow::CompleteFilesList( TLWList &list ) {
  bool reorder = false;

  for ( TLWNode *n = static_cast<TLWNode *>(list.Head()), *next; n; n = next ) {
    next = static_cast<TLWNode *>(n->Next());
    string *fullname = (string *)n->UserData();
    bool campaign = false, skirmish = false, remove = false;

    Mission *m = LoadMission( fullname->c_str(), false );
    if ( m ) {
      string filename;
      string title;

      filename.assign( n->Name() );
      filename.erase( filename.size() - 4 );    // remove .sav/.lev

      if ( (m->GetFlags() & GI_SAVEFILE) || !m->GetName() ) {
        // saved game -> use file name
        title = filename;
      } else {
        // new mission
        // campaign maps are only available if they have been unlocked
        // by playing the campaign
        campaign = ( (m->GetFlags() & GI_CAMPAIGN) &&   // campaign map
                     (m->GetCampaignName() != 0) );     // first map of a campaign

        skirmish = ( (m->GetFlags() & GI_SKIRMISH) &&     // skirmish map
                     (!(m->GetFlags() & GI_CAMPAIGN) ||   // no campaign map
                      campaign ||                         // first campaign map
                      !CFOptions.IsLocked( filename )) ); // or already played

        if ( (m->GetFlags() & GI_CAMPAIGN) &&   // campaign map
             (m->GetCampaignName() != 0) ) {    // first map of a campaign
          TLWNode *cnode = new TLWNode( m->GetCampaignName() );
          string *full_path = new string(*fullname);
          cnode->SetUserData( full_path );
          campaigns.InsertNodeSorted( cnode );
        }

        if ( skirmish ) {
          title.assign( m->GetName() );
          reorder = true;
        }

        remove = !skirmish;
      }

      if ( !remove ) {
        title += ' ';
        title += '(';
        if ( m->GetFlags() & GI_PBEM ) title += MSG(MSG_TAG_PBEM);
        else if ( m->GetFlags() & GI_SAVEFILE ) {
          if ( !m->GetPlayer(PLAYER_ONE).IsHuman() ||
               !m->GetPlayer(PLAYER_TWO).IsHuman() )
            title += '1';
          else title += '2';
        } else title += ((m->GetFlags() & GI_AI) != 0 ? '1' : '2');

        if ( m->GetFlags() & GI_SAVEFILE ) {
          title += ", ";
          title += MSG(MSG_TURN);
          title += ' ';
          title += StringUtil::tostring(m->GetTurn());
        }
        title += ')';
        n->SetName( title );
      }
      delete m;
    } else remove = true;

    if ( remove ) {
      n->Remove();
      delete (string *)n->UserData();
      delete n;
    }
  }

  if ( reorder ) list.Sort();
}

////////////////////////////////////////////////////////////////////////
// NAME       : InitWindow::LoadMission
// DESCRIPTION: Load a mission into memory for inspection.
// PARAMETERS : filename - mission filename
//              full     - whether to completely load the mission
//                         (Mission::Load) or only initialize the parts
//                         for the map selection (Mission::QuickLoad).
//                         Defaults to TRUE.
// RETURNS    : mission if successful, NULL on error
////////////////////////////////////////////////////////////////////////

Mission *InitWindow::LoadMission( const char *filename, bool full ) const {
  Mission *m = 0;

  File file( filename );
  if ( file.Open( "rb" ) ) {
    int rc;
    m = new Mission();

    if ( full ) rc = m->Load( file );
    else rc = m->QuickLoad( file );
    if ( rc != -1 ) {
      m->SetLocale( CFOptions.GetLanguage() );
    } else {
      delete m;
      m = 0;
    }
  }

  return m;
}

////////////////////////////////////////////////////////////////////////
// NAME       : TitleWindow::TitleWindow
// DESCRIPTION: Create a mostly invisible window containing the title
//              screen image.
// PARAMETERS : view - view to attach the window to
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

TitleWindow::TitleWindow( View *view ) : Window( WIN_CENTER, view ) {
  string tname = get_data_dir();
  append_path_delim( tname );
  tname.append( CF_TITLE_SCREEN );

  if ( LoadBMP( tname.c_str() ) ) SetSize( 0, 0, 0, 0 );
  else {
    DisplayFormat();
    Center( *view ); 

    Show();
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : TitleWindow::HandleEvent
// DESCRIPTION: Close (or rather, return control to main) when a key or
//              a mouse button is pressed.
// PARAMETERS : event - event received by the event handler
// RETURNS    : GUI status
////////////////////////////////////////////////////////////////////////

GUI_Status TitleWindow::HandleEvent( const SDL_Event &event ) {
  GUI_Status rc = GUI_OK;

#ifdef PSP
	fakemouse_event(event);

	if((event.type == SDL_KEYDOWN) || (event.type == SDL_MOUSEBUTTONDOWN)|| (event.type == SDL_JOYBUTTONDOWN))
	{
		rc = GUI_CLOSE;
	}
#else
  if ( (event.type == SDL_KEYDOWN) || (event.type == SDL_MOUSEBUTTONDOWN) )
    rc = GUI_CLOSE;
#endif
  return rc;
}


////////////////////////////////////////////////////////////////////////
// NAME       : VideoOptionsWindow::VideoOptionsWindow
// DESCRIPTION: Show the video options menu.
// PARAMETERS : view - view to attach the window to
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

VideoOptionsWindow::VideoOptionsWindow( View *view ) :
     GenericOptionsWindow( MSG(MSG_OPTIONS_VIDEO), view ) {
  SDL_Rect std_sizes[] = { { 0, 0, 1280, 1024 }, { 0, 0, 1024, 768 },
                           { 0, 0, 800, 600}, { 0, 0, 640, 480 } };
  SDL_Rect *std_modes[5];
  short current = -1;

  // create modes list
  SDL_Rect **sdl_modes = SDL_ListModes( NULL, SDL_FULLSCREEN );

  // if any mode is ok offer some standard ones
  if ( sdl_modes == (SDL_Rect **)-1 ) {
    sdl_modes = std_modes;
    std_modes[0] = &std_sizes[0];
    std_modes[1] = &std_sizes[1];
    std_modes[2] = &std_sizes[2];
    std_modes[3] = &std_sizes[3];
    std_modes[4] = NULL;
  }
 
  if ( sdl_modes != NULL ) {
    for ( int i = 0; sdl_modes[i]; ++i ) AddMode( sdl_modes[i] );

    // add current mode
    SDL_Rect screen = { 0, 0, view->Width(), view->Height() };
    current = AddMode( &screen );
  }

  // set window size
  const char *fslabel = MSG(MSG_B_OPT_FULLSCREEN);
  unsigned short width = sfont->TextWidth(fslabel) + DEFAULT_CBW_SIZE + 20;
  if ( width < sfont->Width() * 15 ) width = sfont->Width() * 15;
  SetLayout( width, sfont->Height() * 11 + 30 );

  // create widgets
  const Rect &b = GetBounds();

  modewidget = new TextListWidget( 0, b.x + 5, b.y + 5,
                    b.w - 10, sfont->Height() * 10, &modes, current,
                    WIDGET_HSCROLLKEY|WIDGET_VSCROLLKEY|WIDGET_ALIGN_CENTER,
                    NULL, this );

  fswidget = new CheckboxWidget( 0, modewidget->LeftEdge() + 10,
                   modewidget->TopEdge() + modewidget->Height() + 10,
                   DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, view->IsFullScreen(),
                   WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT,
                   fslabel, this );

  Draw();
  Show();
}

////////////////////////////////////////////////////////////////////////
// NAME       : VideoOptionsWindow::~VideoOptionsWindow
// DESCRIPTION: Destroy the video options window.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

VideoOptionsWindow::~VideoOptionsWindow( void ) {
  TLWNode *n2;

  for ( TLWNode *n = static_cast<TLWNode *>(modes.Head()); n; n = n2 ) {
    n2 = static_cast<TLWNode *>(n->Next());
    n->Remove();

    SDL_Rect *rect = (SDL_Rect *)n->UserData();
    delete rect;
    delete n;
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : VideoOptionsWindow::AddMode
// DESCRIPTION: Add another resultion to the list. Duplicates and
//              invalid sizes will be rejected.
// PARAMETERS : res - resolution to add
// RETURNS    : position at which the mode has been added (or existed
//              before); -1 if mode was rejected
////////////////////////////////////////////////////////////////////////

short VideoOptionsWindow::AddMode( SDL_Rect *res ) {
  short rc = -1;

  if ( (res->w >= MIN_XRES) && (res->h >= MIN_YRES) ) {
    TLWNode *walk, *prev = NULL;
    bool add = true;
    rc = 0;

    for ( walk = static_cast<TLWNode *>(modes.Head());
          walk; prev = walk, walk = static_cast<TLWNode *>(walk->Next()) ) {
      SDL_Rect *nres = (SDL_Rect *)walk->UserData();
      if ( nres->w <= res->w ) {
        if ( (nres->w == res->w) && (nres->h == res->h) ) add = false;
        break;
      }
      ++rc;
    }

    if ( add ) {
      char buf[16];
      sprintf( buf, "%d x %d", res->w, res->h );

      SDL_Rect *mode = new SDL_Rect;
      mode->w = res->w;
      mode->h = res->h;
      TLWNode *n = new TLWNode( buf, mode );
      modes.InsertNode( n, prev );
    }
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////
// NAME       : VideoOptionsWindow::WidgetActivated
// DESCRIPTION: When the user pushes the 'OK' button, switch to the
//              selected video mode.
// PARAMETERS : button - calling widget
//              win    - pointer to active window
// RETURNS    : GUI status
////////////////////////////////////////////////////////////////////////

GUI_Status VideoOptionsWindow::WidgetActivated( Widget *button, Window *win ) {
  view->CloseWindow( win );

  TLWNode *mode = static_cast<TLWNode *>( modewidget->Selected() );

  if ( mode ) {
    bool fs = fswidget->Clicked();
    unsigned long mode_flags = SDL_HWSURFACE|(fs ? SDL_FULLSCREEN : 0);

    // if selected mode is the same as current mode only check for fullscreen
    // dimensions of the selected mode are available in the user_data field
    SDL_Rect *dim = (SDL_Rect *)mode->UserData();
    if ( (dim->w == view->Width()) && (dim->h == view->Height()) ) {
      if ( fs != view->IsFullScreen() ) view->ToggleFullScreen();
    } else {
      view->SetVideoMode( dim->w, dim->h, DISPLAY_BPP, mode_flags );
    }
  }
  return GUI_OK;
}


////////////////////////////////////////////////////////////////////////
// NAME       : GenericOptionsWindow::SetLayout
// DESCRIPTION: Set the size of the window. This must be called before
//              trying to display it or using GetBounds().
// PARAMETERS : w     - width (only the part needed by the subclass)
//              h     - height (only the part needed by the subclass)
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void GenericOptionsWindow::SetLayout( unsigned short w, unsigned short h ) {
  unsigned short tw = lfont->TextWidth(title);

  if ( w < tw + 20 ) w = tw + 20;

  if ( h + 25 + lfont->Height() + sfont->Height() >= view->Height() )
    h = view->Height() - lfont->Height() - sfont->Height() - 26;

  clientarea = Rect( 5, 13 + lfont->Height(), w, h );

  Window::SetSize( w + 10, h + lfont->Height() + sfont->Height() + 25 );

  Widget *wd = new ButtonWidget( B_ID_OK, 1, Height() - sfont->Height() - 9,
                   Width() - 2, sfont->Height() + 8, WIDGET_DEFAULT, MSG(MSG_B_OK), this );
  wd->SetHook( this );
}

////////////////////////////////////////////////////////////////////////
// NAME       : GenericOptionsWindow::Draw
// DESCRIPTION: Draw the options window.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void GenericOptionsWindow::Draw( void ) {
  Window::Draw();

  DrawBox( clientarea, BOX_RECESSED );

  int xpos = (w - lfont->TextWidth(title)) / 2;
  lfont->Write( title, this, xpos + 3, 8, view->GetBGPen() );
  lfont->Write( title, this, xpos, 5, view->GetFGPen() );
}


////////////////////////////////////////////////////////////////////////
// NAME       : GeneralOptionsWindow::GeneralOptionsWindow
// DESCRIPTION: Show the general options menu.
// PARAMETERS : mv   - current map view (may be NULL)
//              view - view to attach the window to
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

GeneralOptionsWindow::GeneralOptionsWindow( MapView *mv, View *view ) :
     GenericOptionsWindow( MSG(MSG_OPTIONS_GENERAL), view ), mv(mv) {
  unsigned short widths[3] = { 25 + DEFAULT_CBW_SIZE,
                               25 + DEFAULT_CBW_SIZE,
                               35 + DEFAULT_CBW_SIZE }, maxw = 0;
  const char *labels[3];

  labels[0] = MSG(MSG_B_OPT_DAMAGE);
  labels[1] = MSG(MSG_B_OPT_REPLAYS);
  labels[2] = MSG(MSG_B_OPT_REPLAYS_QUICK);

  for ( int i = 0; i < 3; ++i ) {
    widths[i] += sfont->TextWidth(labels[i]);
    if ( widths[i] > maxw ) maxw = widths[i];
  }

  SetLayout( maxw, sfont->Height() * 3 + 35 );

  // create widgets
  const Rect &b = GetBounds();

  // damage indicator
  diwidget = new CheckboxWidget( 0, b.x + 5, b.y + 5,
               DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, CFOptions.GetDamageIndicator(),
               WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT,
               labels[0], this );

  // replay
  repwidget = new CheckboxWidget( 0, b.x + 5, b.y + diwidget->Height() + 10,
              DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, CFOptions.GetTurnReplay(),
              WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT,
              labels[1], this );
  repwidget->SetHook(this);

  // quick replays
  qrepwidget = new CheckboxWidget( 0, b.x + 15, repwidget->y + repwidget->Height() + 5,
              DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, CFOptions.GetQuickReplay(),
              WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT|
              (repwidget->Clicked() ? 0 : WIDGET_DISABLED),
              labels[2], this );
  Draw();
  Show();
}

////////////////////////////////////////////////////////////////////////
// NAME       : GeneralOptionsWindow::WidgetActivated
// DESCRIPTION: When the user pushes the 'OK' button, propagate the
//              selected options.
// PARAMETERS : widget - calling widget
//              win    - pointer to active window
// RETURNS    : GUI_OK
////////////////////////////////////////////////////////////////////////

GUI_Status GeneralOptionsWindow::WidgetActivated( Widget *widget, Window *win ) {

  if ( widget == repwidget ) {

    if ( repwidget->Clicked() ) qrepwidget->Enable();
    else {
      qrepwidget->Release();
      qrepwidget->Disable();
    }

    Draw();
    Show();

  } else {

    view->CloseWindow( win );

    if ( CFOptions.GetDamageIndicator() != diwidget->Clicked() ) {
      CFOptions.SetDamageIndicator( diwidget->Clicked() );

      if ( mv ) {
        if ( diwidget->Clicked() ) mv->EnableUnitStats();
        else mv->DisableUnitStats();

        // now redraw the map display
        mv->Draw();
        view->Refresh( *mv );
      }
    }

    CFOptions.SetTurnReplay( repwidget->Clicked() );
    CFOptions.SetQuickReplay( qrepwidget->Clicked() );
  }

  return GUI_OK;
}

////////////////////////////////////////////////////////////////////////
// NAME       : LocaleOptionsWindow::LocaleOptionsWindow
// DESCRIPTION: Show the language options menu.
// PARAMETERS : game - current game. If it is NULL we assume the window
//                     has been opened from the initial start window.
//                     Otherwise we do an in-game language switch.
//              view - view to attach the window to
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

LocaleOptionsWindow::LocaleOptionsWindow( Game *game, View *view ) :
     GenericOptionsWindow( MSG(MSG_OPTIONS_LANGUAGE), view ), game(game) {

  // collect language data files
  short current = ReadLocales();
  short found = locales.CountNodes();
  if ( found > 5 ) found = 5;

  SetLayout( sfont->Width() * 20 + 20, found * (sfont->Height() + 2) + 25 );

  // create widgets
  const Rect &b = GetBounds();

  locwidget = new TextListWidget( 0, b.x, b.y, b.w, b.h, &locales, current,
                  WIDGET_HSCROLLKEY|WIDGET_VSCROLLKEY|WIDGET_ALIGN_CENTER,
                  NULL, this );

  Draw();
  Show();
}

////////////////////////////////////////////////////////////////////////
// NAME       : LocaleOptionsWindow::~LocaleOptionsWindow
// DESCRIPTION: Free resources.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

LocaleOptionsWindow::~LocaleOptionsWindow( void ) {
  while ( !locales.IsEmpty() ) {
    TLWNode *n = static_cast<TLWNode *>(locales.RemHead());
    delete (Language *)n->UserData();
    delete n;
  }
}

////////////////////////////////////////////////////////////////////////
// NAME       : LocaleOptionsWindow::ReadLocales
// DESCRIPTION: Get the list of locales.
// PARAMETERS : -
// RETURNS    : index of currently selected language
////////////////////////////////////////////////////////////////////////

short LocaleOptionsWindow::ReadLocales( void ) {

  TLWList files;
  FileWindow::CreateFilesList( get_locale_dir().c_str(),
                               ".dat", files );
  short i = 0, current = -1;

  while ( !files.IsEmpty() ) {
    TLWNode *n = static_cast<TLWNode *>(files.RemHead());

    string *fname = (string *)n->UserData();

    Language *l = new Language();
    short num = l->ReadCatalog( fname->c_str() );
    if ( num == CF_MSGS ) {
      n->SetName( l->Name() );
      n->SetUserData( l );
      n->SetID( i );

      locales.AddTail( n );

      if ( string(l->ID()) == CFOptions.GetLanguage() )
        current = i;

      ++i;
    } else {
      if ( num == -1 ) {
        cerr << "Error: Could not load language resources from "
             << n->Name() << endl;
      } else {
        cerr << "Error: Language catalog '" << n->Name()
             << "' contains " << num << " strings, expected "
             << CF_MSGS << endl;
      }
      delete l;
      delete n;
    }

    delete fname;
  }

  return current;
}

////////////////////////////////////////////////////////////////////////
// NAME       : LocaleOptionsWindow::WidgetActivated
// DESCRIPTION: When the user pushes the 'OK' button, propagate the
//              selected options.
// PARAMETERS : widget - calling widget (only OK)
//              win    - pointer to active window
// RETURNS    : GUI_OK
////////////////////////////////////////////////////////////////////////

GUI_Status LocaleOptionsWindow::WidgetActivated( Widget *widget, Window *win ) {
  GUI_Status rc = GUI_OK;

  view->CloseWindow( win );

  TLWNode *n = static_cast<TLWNode *>(locwidget->Selected());
  if ( n ) {
    Language *l = (Language *)n->UserData();
    if ( string(l->ID()) != CFOptions.GetLanguage() ) {

      Lang = *l;
      CFOptions.SetLanguage( l->ID() );

      if ( game ) {
        // we've changed the language in-game:
        // simply set the language and update the panel
        game->InitKeys();
        game->GetMission()->SetLocale( l->ID() );

        MapWindow *mwin = game->GetMapWindow();
        MapView *mv = mwin->GetMapView();
        mwin->GetPanel()->Update( mv->GetMap()->GetMapObject(mv->Cursor()) );
      } else {
        // we've been called from the start menu: set the new language
        // and start over. This way we don't need to update the individual
        // widgets
        rc = GUI_RESTART;
      }
    }
  }

  return rc;
}

////////////////////////////////////////////////////////////////////////
// NAME       : SoundOptionsWindow::SoundOptionsWindow
// DESCRIPTION: Show the sound options menu.
// PARAMETERS : view - view to attach the window to
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_SOUND

const short SoundOptionsWindow::B_ID_SFX       = 10;
const short SoundOptionsWindow::B_ID_MUSIC     = 11;
const short SoundOptionsWindow::S_ID_VOL_SFX   = 12;
const short SoundOptionsWindow::S_ID_VOL_MUSIC = 13;

SoundOptionsWindow::SoundOptionsWindow( View *view ) :
     GenericOptionsWindow( MSG(MSG_OPTIONS_AUDIO), view ),
     volgfx( view->GetSystemIcons(), 145, 46, 12 ,12 ) {
  const char *sfxlabel = MSG(MSG_B_OPT_SFX);
  const char *musiclabel = MSG(MSG_B_OPT_MUSIC);
  unsigned short sfxw = sfont->TextWidth(sfxlabel);
  unsigned short musw = sfont->TextWidth(musiclabel);

  SetLayout( MAX(sfxw, musw) + DEFAULT_CBW_SIZE +
             volgfx.Width() + 100,
             view->SmallFont()->Height() * 2 + 20 );

  // create widgets
  const Rect &b = GetBounds();
  Widget *wd;

  // sfx
  wd = new CheckboxWidget( B_ID_SFX, b.x + 5, b.y + 5,
           DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, Audio::GetSfxState(),
           WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT,
           sfxlabel, this );
  wd->SetHook( this );

  // sfx volume
  short xoff = wd->LeftEdge() + wd->Width() + MAX(sfxw, musw) +
               volgfx.Width() + 15;
  sfxvol = new SliderWidget( S_ID_VOL_SFX,
           xoff, wd->TopEdge() + (wd->Height() - DEFAULT_SLIDER_SIZE)/2,
           b.w - xoff - 5, DEFAULT_SLIDER_SIZE, 0, MIX_MAX_VOLUME,
           Audio::GetSfxVolume(), 20,
           WIDGET_HSCROLL|WIDGET_HSCROLLKEY|
             (Audio::GetSfxState() ? 0 : WIDGET_DISABLED),
           NULL, this );
  sfxvol->SetHook( this );

  // music
  wd = new CheckboxWidget( B_ID_MUSIC,
           wd->LeftEdge(), wd->TopEdge() + wd->Height() + 5,
           DEFAULT_CBW_SIZE, DEFAULT_CBW_SIZE, Audio::GetMusicState(),
           WIDGET_STYLE_NOBORDER|WIDGET_STYLE_GFX|WIDGET_ALIGN_RIGHT,
           musiclabel, this );
  wd->SetHook( this );

  // music volume
  musicvol = new SliderWidget( S_ID_VOL_MUSIC,
           xoff, wd->TopEdge() + (wd->Height() - DEFAULT_SLIDER_SIZE)/2,
           b.w - xoff - 5, DEFAULT_SLIDER_SIZE, 0, MIX_MAX_VOLUME,
           Audio::GetMusicVolume(), 20, WIDGET_HSCROLL|WIDGET_VSCROLLKEY|
             (Audio::GetMusicState() ? 0 : WIDGET_DISABLED),
           NULL, this );
  musicvol->SetHook( this );

  Draw();
  Show();
}

////////////////////////////////////////////////////////////////////////
// NAME       : SoundOptionsWindow::WidgetActivated
// DESCRIPTION: When the user activates a widget, propagate the changes
//              to the sound layer.
// PARAMETERS : widget - calling widget
//              win    - pointer to active window
// RETURNS    : GUI_OK
////////////////////////////////////////////////////////////////////////

GUI_Status SoundOptionsWindow::WidgetActivated( Widget *widget, Window *win ) {

  switch ( widget->ID() ) {
  case B_ID_SFX:
    Audio::ToggleSfxState();

    if ( Audio::GetSfxState() ) sfxvol->Enable();
    else sfxvol->Disable();
    break;

  case S_ID_VOL_SFX:
    Audio::SetSfxVolume( sfxvol->Level() );
    break;

  case B_ID_MUSIC:
    Audio::ToggleMusicState();

    if ( Audio::GetMusicState() ) musicvol->Enable();
    else musicvol->Disable();
    break;

  case S_ID_VOL_MUSIC:
    Audio::SetMusicVolume( musicvol->Level() );
    break;

  case B_ID_OK:
    view->CloseWindow( this );
    break;

  default:
    break;
  }

  return GUI_OK;
}

////////////////////////////////////////////////////////////////////////
// NAME       : SoundOptionsWindow::Draw
// DESCRIPTION: Draw the sound options window.
// PARAMETERS : -
// RETURNS    : -
////////////////////////////////////////////////////////////////////////

void SoundOptionsWindow::Draw( void ) {
  GenericOptionsWindow::Draw();

  short xoff = sfxvol->LeftEdge() - volgfx.Width() - 5;

  volgfx.Draw( this, xoff,
         sfxvol->TopEdge() + (sfxvol->Height() - volgfx.Height())/2 );
  volgfx.Draw( this, xoff,
         musicvol->TopEdge() + (musicvol->Height() - volgfx.Height())/2 );
}
#endif

