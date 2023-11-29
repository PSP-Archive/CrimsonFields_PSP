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

///////////////////////////////////////////////////////////////////////
// history.h - constructs to record the important events during a turn
//             and replay them on the next
///////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_HISTORY_H
#define _INCLUDE_HISTORY_H

#include "combat.h"

enum {  HIST_MOVE = 0,
        HIST_ATTACK,
        HIST_COMBAT,
        HIST_TILE,      // map tile change
        HIST_TILE_INTERNAL,
        HIST_MSG,
        HIST_UNIT
};

class HistEvent : public Node {
public:
  HistEvent( void ) {}
  int Load ( File &file );
  int Save( File &file ) const;

  unsigned char type;
  short data[4];
};


class MapWindow;

class History {
public:
  History( void ) {}
  short Load( File &file, Mission &mission );
  int Save( File &file ) const;

  void StartRecording( List &list );

  int RecordAttackEvent( const Unit &u, const Unit &target );
  int RecordCombatEvent( const Combat &combat, unsigned char loss1,
                         unsigned char loss2 );
  int RecordMoveEvent( const Unit &u, Direction dir );
  int RecordMsgEvent( short title, unsigned short msg, short pos );
  int RecordTileEvent( unsigned short tile, unsigned short old,
                       short dx, short dy );
  int RecordUnitEvent( const Unit &u );

  void Replay( MapWindow *mapwin );

  void EraseMoveEvents( const Unit &u );

private:
  void BeginReplay( List &backup, Map *map );
  void EndReplay( List &backup, Map *map );

  void ReplayAttackEvent( const HistEvent &event, MapWindow *mapwin );
  void ReplayCombatEvent( const HistEvent &event, MapWindow *mapwin );
  void ReplayMessageEvent( const HistEvent &event, View *view );
  void ReplayMoveEvent( const HistEvent &event, MapWindow *mapwin );
  void ReplayTileEvent( const HistEvent &event, MapWindow *mapwin );
  void ReplayUnitEvent( const HistEvent &event, MapWindow *mapwin );

  List events;
  List units;

  unsigned short delay;

  Unit *lastunit;
  Point lastpos;
};

#endif	/* _INCLUDE_HISTORY_H */

