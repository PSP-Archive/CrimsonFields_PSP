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
// options.h 
// This is used as a persistent options container that holds
// relevant settings for longer than the actual Game object
// exists and can be used to properly reinitialize when a
// new game starts.
///////////////////////////////////////////////////////////////

#ifndef _INCLUDE_OPTIONS_H
#define _INCLUDE_OPTIONS_H

#include <algorithm>
#include "lang.h"

class Options {
public:
  Options( void ) : gametype(0), show_damage(true),
                    replay(true), quick_replay(false),
                    campaign(false), language(CF_LANG_DEFAULT) {}

  void SetDamageIndicator( bool di ) { show_damage = di; }
  void SetTurnReplay( bool rep ) { replay = rep; }
  void SetQuickReplay( bool rep ) { quick_replay = rep; }
  void SetCampaign( bool flag ) { campaign = flag; }
  void SetLanguage( const char *lang ) { language.assign(lang); }
  void SetGameType( unsigned short type ) { gametype = type; }

  bool GetDamageIndicator( void ) const { return show_damage; }
  bool GetTurnReplay( void ) const { return replay; }
  bool GetQuickReplay( void ) const { return quick_replay; }
  bool GetCampaign( void ) const { return campaign; }
  const char *GetLanguage( void ) const { return language.c_str(); }
  unsigned short GetGameType( void ) const { return gametype; }

  bool IsAI( void ) const { return (gametype == 1) || GetCampaign(); }
  bool IsPBEM( void ) const { return (gametype == 2) && !GetCampaign(); }

  bool IsLocked( const string &map ) const
    { return find(unlocked_maps.begin(), unlocked_maps.end(), map)
             == unlocked_maps.end(); }
  void Unlock( const string &map )
    { if (IsLocked(map)) unlocked_maps.push_back( map ); }
  const vector<string> &GetUnlockedMaps( void ) const
    { return unlocked_maps; }

private:
  unsigned short gametype; // hot seat, ai, or pbem
  bool show_damage;   // show damage indicator
  bool replay;        // show turn replays
  bool quick_replay;  // show only combat results
  bool campaign;      // playing a campaign
  string language;
  vector<string> unlocked_maps;
};

#endif	/* _INCLUDE_OPTIONS_H */

