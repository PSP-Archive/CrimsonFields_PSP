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
// msgs.h - message identifiers
///////////////////////////////////////////////////////////////

#ifndef _INCLUDE_MSGS_H
#define _INCLUDE_MSGS_H

#include "lang.h"
extern Language Lang;

#define MSG(m)			Lang.GetMsg(m)

#define MSG_B_OK		0
#define MSG_B_CANCEL		(MSG_B_OK+1)
#define MSG_B_YES		(MSG_B_CANCEL+1)
#define MSG_B_NO		(MSG_B_YES+1)
#define MSG_B_START		(MSG_B_NO+1)
#define MSG_B_MAP		(MSG_B_START+1)
#define MSG_B_OBJECTIVES	(MSG_B_MAP+1)
#define MSG_B_LEVEL_INFO	(MSG_B_OBJECTIVES+1)
#define MSG_B_LOAD		(MSG_B_LEVEL_INFO+1)
#define MSG_B_SAVE		(MSG_B_LOAD+1)
#define MSG_B_SKIP		(MSG_B_SAVE+1)
#define MSG_B_END_TURN		(MSG_B_SKIP+1)
#define MSG_B_OPTIONS		(MSG_B_END_TURN+1)
#define MSG_B_OPT_GENERAL	(MSG_B_OPTIONS+1)
#define MSG_B_OPT_VIDEO		(MSG_B_OPT_GENERAL+1)
#define MSG_B_OPT_AUDIO		(MSG_B_OPT_VIDEO+1)
#define MSG_B_OPT_FULLSCREEN	(MSG_B_OPT_AUDIO+1)
#define MSG_B_OPT_DAMAGE	(MSG_B_OPT_FULLSCREEN+1)
#define MSG_B_OPT_REPLAYS	(MSG_B_OPT_DAMAGE+1)
#define MSG_B_OPT_REPLAYS_QUICK	(MSG_B_OPT_REPLAYS+1)
#define MSG_B_OPT_SFX		(MSG_B_OPT_REPLAYS_QUICK+1)
#define MSG_B_OPT_MUSIC		(MSG_B_OPT_SFX+1)
#define MSG_B_OPT_LANGUAGE	(MSG_B_OPT_MUSIC+1)
#define MSG_B_GAME_TYPE		(MSG_B_OPT_LANGUAGE+1)
#define MSG_B_MAP_TYPE		(MSG_B_GAME_TYPE+1)
#define MSG_B_SAVE_GAME		(MSG_B_MAP_TYPE+1)
#define MSG_B_MAIN_MENU		(MSG_B_SAVE_GAME+1)
#define MSG_B_QUIT		(MSG_B_MAIN_MENU+1)
#define MSG_B_EXIT		(MSG_B_QUIT+1)
#define MSG_B_HANDICAP		(MSG_B_EXIT+1)
#define MSG_B_REPAIR		(MSG_B_HANDICAP+1)
#define MSG_B_UNIT_INFO		(MSG_B_REPAIR+1)
#define MSG_B_UNIT_CONTENT	(MSG_B_UNIT_INFO+1)
#define MSG_B_UNIT_SWEEP	(MSG_B_UNIT_CONTENT+1)
#define MSG_B_UNIT_UNDO		(MSG_B_UNIT_SWEEP+1)
#define MSG_GAME_HOT_SEAT	(MSG_B_UNIT_UNDO+1)
#define MSG_GAME_PBEM		(MSG_GAME_HOT_SEAT+1)
#define MSG_GAME_AI		(MSG_GAME_PBEM+1)
#define MSG_MAPS_SINGLES	(MSG_GAME_AI+1)
#define MSG_MAPS_CAMPAIGNS	(MSG_MAPS_SINGLES+1)
#define MSG_MAPS_SAVES		(MSG_MAPS_CAMPAIGNS+1)
#define MSG_HANDICAP_NONE	(MSG_MAPS_SAVES+1)
#define MSG_HANDICAP_P1		(MSG_HANDICAP_NONE+1)
#define MSG_HANDICAP_P2		(MSG_HANDICAP_P1+1)
#define MSG_TAG_PBEM		(MSG_HANDICAP_P2+1)
#define MSG_TURN		(MSG_TAG_PBEM+1)
#define MSG_UNITS		(MSG_TURN+1)
#define MSG_SHOPS		(MSG_UNITS+1)
#define MSG_PLAYER_SELECTION	(MSG_SHOPS+1)
#define MSG_DEBRIEFING		(MSG_PLAYER_SELECTION+1)
#define MSG_LVL_INFO		(MSG_DEBRIEFING+1)
#define MSG_OPTIONS_GENERAL	(MSG_LVL_INFO+1)
#define MSG_OPTIONS_VIDEO	(MSG_OPTIONS_GENERAL+1)
#define MSG_OPTIONS_AUDIO	(MSG_OPTIONS_VIDEO+1)
#define MSG_OPTIONS_LANGUAGE	(MSG_OPTIONS_AUDIO+1)
#define MSG_LOAD_GAME		(MSG_OPTIONS_LANGUAGE+1)
#define MSG_SAVE_GAME		(MSG_LOAD_GAME+1)
#define MSG_GAME_SAVED		(MSG_SAVE_GAME+1)
#define MSG_GAME_SAVED_PBEM	(MSG_GAME_SAVED+1)
#define MSG_ENTER_PASSWORD	(MSG_GAME_SAVED_PBEM+1)
#define MSG_CHOOSE_PASSWORD	(MSG_ENTER_PASSWORD+1)
#define MSG_CONFIRM_PASSWORD	(MSG_CHOOSE_PASSWORD+1)
#define MSG_TRANSFER_UNITS	(MSG_CONFIRM_PASSWORD+1)
#define MSG_ASK_SIDE		(MSG_TRANSFER_UNITS+1)
#define MSG_ASK_REPAIR		(MSG_ASK_SIDE+1)
#define MSG_ASK_OVERWRITE	(MSG_ASK_REPAIR+1)
#define MSG_ASK_ABORT		(MSG_ASK_OVERWRITE+1)
#define MSG_ASK_ABORT_PBEM	(MSG_ASK_ABORT+1)
#define MSG_ASK_QUIT		(MSG_ASK_ABORT_PBEM+1)
#define MSG_RESULT_DRAW		(MSG_ASK_QUIT+1)
#define MSG_RESULT_VICTORY	(MSG_RESULT_DRAW+1)
#define MSG_RESULT_DEFEAT	(MSG_RESULT_VICTORY+1)
#define MSG_MESSAGE		(MSG_RESULT_DEFEAT+1)
#define MSG_WARNING		(MSG_MESSAGE+1)
#define MSG_ERROR		(MSG_WARNING+1)
#define MSG_ERR_WRITE		(MSG_ERROR+1)
#define MSG_ERR_SAVE		(MSG_ERR_WRITE+1)
#define MSG_ERR_LOAD_MAP	(MSG_ERR_SAVE+1)
#define MSG_ERR_NO_ACCESS	(MSG_ERR_LOAD_MAP+1)
#define MSG_ERR_NO_BRIEFING	(MSG_ERR_NO_ACCESS+1)
#define MSG_ERR_NO_LVL_INFO	(MSG_ERR_NO_BRIEFING+1)
#define MSG_ERR_MAP_NOT_FOUND	(MSG_ERR_NO_LVL_INFO+1)
#define MSG_ERR_NO_MAP		(MSG_ERR_MAP_NOT_FOUND+1)
#define MSG_ERR_SWEEPER_FULL	(MSG_ERR_NO_MAP+1)
#define MSG_ERR_NO_CRYSTALS	(MSG_ERR_SWEEPER_FULL+1)
#define MSG_ERR_NO_PRODUCTION	(MSG_ERR_NO_CRYSTALS+1)

#define CF_MSGS			(MSG_ERR_NO_PRODUCTION+1)

#endif   /* _INCLUDE_MSGS_H */

