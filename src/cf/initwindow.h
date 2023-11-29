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
// initwindow.h
///////////////////////////////////////////////////////////////

#ifndef _INCLUDE_INITWINDOW_H
#define _INCLUDE_INITWINDOW_H

#include "window.h"
#include "textbox.h"
#include "button.h"
#include "mapwindow.h"

class TitleWindow : public Window {
public:
  TitleWindow( View *view );

  GUI_Status HandleEvent( const SDL_Event &event );
  void Draw( void ) {}
};

class InitWindow : public Window, public WidgetHook {
public:
  InitWindow( View *view, Window *title );
  ~InitWindow( void );

  void Close( void ) { Window::Close(); view->CloseWindow(title); }
  GUI_Status WidgetActivated( Widget *button, Window *win );
  void VideoModeChange( void );

private:
  void CompleteFilesList( TLWList &list );
  Mission *LoadMission( const char *filename, bool full = true ) const;
  void Rebuild( void );

  CycleWidget *gtypewidget;
  CycleWidget *mtypewidget;
  CycleWidget *diffwidget;
  TextListWidget *levwidget;
  MapWidget *mapwidget;
  TextScrollWidget *campinfowidget;

  Rect maxmap;

  TLWList levels;
  TLWList campaigns;
  TLWList saves;

  Window *title;
};

class GenericOptionsWindow : public Window, public WidgetHook {
public:
  GenericOptionsWindow( const char *title, View *view ) :
                Window( WIN_CENTER, view ), title(title) {}

  virtual void Draw( void );

protected:
  virtual GUI_Status WidgetActivated( Widget *widget, Window *win ) = 0;
  void SetLayout( unsigned short w, unsigned short h );

  const Rect &GetBounds( void ) const { return clientarea; }

  static const short B_ID_OK;

private:
  Rect clientarea;  // the part subclasses may draw into
  const char *title;
};

class GeneralOptionsWindow : public GenericOptionsWindow {
public:
  GeneralOptionsWindow( MapView *mv, View *view );

private:
  GUI_Status WidgetActivated( Widget *button, Window *win );

  CheckboxWidget *diwidget;  // damage indicators
  CheckboxWidget *repwidget; // turn replays
  CheckboxWidget *qrepwidget; // quick turn replays
  MapView *mv;
};

class VideoOptionsWindow : public GenericOptionsWindow {
public:
  VideoOptionsWindow( View *view );
  ~VideoOptionsWindow( void );

private:
  GUI_Status WidgetActivated( Widget *button, Window *win );
  short AddMode( SDL_Rect *res );

  TextListWidget *modewidget;
  CheckboxWidget *fswidget;

  List modes;
};

class Game;
class LocaleOptionsWindow : public GenericOptionsWindow {
public:
  LocaleOptionsWindow( Game *game, View *view );
  ~LocaleOptionsWindow( void );

private:
  GUI_Status WidgetActivated( Widget *widget, Window *win );

  short ReadLocales( void );

  TLWList locales;
  TextListWidget *locwidget;
  Game *game;
};

# ifndef DISABLE_SOUND
class SoundOptionsWindow : public GenericOptionsWindow {
public:
  SoundOptionsWindow( View *view );
  void Draw( void );

private:
  GUI_Status WidgetActivated( Widget *button, Window *win );

  static const short B_ID_SFX;
  static const short B_ID_MUSIC;
  static const short S_ID_VOL_SFX;
  static const short S_ID_VOL_MUSIC;

  SliderWidget *sfxvol;
  SliderWidget *musicvol;

  Image volgfx;
};
# endif

#endif	/* _INCLUDE_INITWINDOW_H */

