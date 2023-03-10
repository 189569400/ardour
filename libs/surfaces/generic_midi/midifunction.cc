/*
 * Copyright (C) 2009-2016 Paul Davis <paul@linuxaudiosystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cstring>

#include "midi++/port.h"

#include "midifunction.h"
#include "generic_midi_control_protocol.h"

#include "pbd/compose.h"

#include "ardour/debug.h"
#include "ardour/stripable.h"
#include "ardour/session.h"

using namespace MIDI;
using namespace PBD;

MIDIFunction::MIDIFunction (MIDI::Parser& p)
	: MIDIInvokable (p)
{
}

MIDIFunction::~MIDIFunction ()
{
}

int
MIDIFunction::setup (GenericMidiControlProtocol& ui, const std::string& invokable_name, const std::string& arg, MIDI::byte* msg_data, size_t data_sz)
{
        MIDIInvokable::init (ui, invokable_name, msg_data, data_sz);

	_argument = arg;

	if (strcasecmp (_invokable_name.c_str(), "transport-stop") == 0) {
		_function = TransportStop;
	} else if (strcasecmp (_invokable_name.c_str(), "transport-roll") == 0) {
		_function = TransportRoll;
	} else if (strcasecmp (_invokable_name.c_str(), "transport-zero") == 0) {
		_function = TransportZero;
	} else if (strcasecmp (_invokable_name.c_str(), "transport-start") == 0) {
		_function = TransportStart;
	} else if (strcasecmp (_invokable_name.c_str(), "transport-end") == 0) {
		_function = TransportEnd;
	} else if (strcasecmp (_invokable_name.c_str(), "loop-toggle") == 0) {
		_function = TransportLoopToggle;
	} else if (strcasecmp (_invokable_name.c_str(), "toggle-rec-enable") == 0) {
		_function = TransportRecordToggle;
	} else if (strcasecmp (_invokable_name.c_str(), "rec-enable") == 0) {
		_function = TransportRecordEnable;
	} else if (strcasecmp (_invokable_name.c_str(), "rec-disable") == 0) {
		_function = TransportRecordDisable;
	} else if (strcasecmp (_invokable_name.c_str(), "next-bank") == 0) {
		_function = NextBank;
	} else if (strcasecmp (_invokable_name.c_str(), "prev-bank") == 0) {
		_function = PrevBank;
	} else if (strcasecmp (_invokable_name.c_str(), "set-bank") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = SetBank;
	} else if (strcasecmp (_invokable_name.c_str(), "select") == 0 || strcasecmp (_invokable_name.c_str(), "select-set") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = SelectSet;
	} else if (strcasecmp (_invokable_name.c_str(), "select-remove") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = SelectRemove;
	} else if (strcasecmp (_invokable_name.c_str(), "select-add") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = SelectAdd;
	} else if (strcasecmp (_invokable_name.c_str(), "select-toggle") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = SelectToggle;
	} else if (strcasecmp (_invokable_name.c_str(), "track-set-solo") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = TrackSetSolo;
	} else if (strcasecmp (_invokable_name.c_str(), "track-set-mute") == 0) {
		if (_argument.empty()) {
			return -1;
		}
		_function = TrackSetMute;
	} else {
		return -1;
	}

	return 0;
}

void
MIDIFunction::execute ()
{
	switch (_function) {
	case NextBank:
		_ui->next_bank();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: next_bank\n");
		break;

	case PrevBank:
		_ui->prev_bank();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: prev_bank\n");
		break;

	case SetBank:
		if (!_argument.empty()) {
			uint32_t bank;
			sscanf (_argument.c_str(), "%d", &bank);
			_ui->set_current_bank (bank);
			DEBUG_TRACE (DEBUG::GenericMidi, string_compose ("Function: set_current_bank = %1\n", (int) bank));
		}
		break;

	case TransportStop:
		_ui->transport_stop ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: transport_stop\n");
		break;

	case TransportRoll:
		_ui->transport_play ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: transport_play\n");
		break;

	case TransportStart:
		_ui->goto_start ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: goto_start\n");
		break;

	case TransportZero:
		// need this in BasicUI
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: goto_zero-not implemented\n");
		break;

	case TransportEnd:
		_ui->goto_end ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: goto_end\n");
		break;

	case TransportLoopToggle:
		_ui->loop_toggle ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: loop_toggle\n");
		break;

	case TransportRecordToggle:
		_ui->rec_enable_toggle ();
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: toggle_record_enable\n");
		break;

	case TransportRecordEnable:
		_ui->set_record_enable (true);
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: set_record_enable = true\n");
		break;

	case TransportRecordDisable:
		_ui->set_record_enable (false);
		DEBUG_TRACE (DEBUG::GenericMidi, "Function: set_record_enable = false\n");
		break;

	case SelectSet:
		if (!_argument.empty()) {
			uint32_t rid;
			sscanf (_argument.c_str(), "%d", &rid);
			_ui->set_rid_selection (rid);
			DEBUG_TRACE (DEBUG::GenericMidi, string_compose ("Function: SelectSet = %1\n", rid));
		}
		break;
	case SelectAdd:
		if (!_argument.empty()) {
			uint32_t rid;
			sscanf (_argument.c_str(), "%d", &rid);
			_ui->add_rid_to_selection (rid);
			DEBUG_TRACE (DEBUG::GenericMidi, string_compose ("Function: SelectAdd = %1\n", rid));
		}
		break;
	case SelectRemove:
		if (!_argument.empty()) {
			uint32_t rid;
			sscanf (_argument.c_str(), "%d", &rid);
			_ui->remove_rid_from_selection (rid);
			DEBUG_TRACE (DEBUG::GenericMidi, string_compose ("Function: SelectRemove = %1\n", rid));
		}
		break;
	case SelectToggle:
		if (!_argument.empty()) {
			uint32_t rid;
			sscanf (_argument.c_str(), "%d", &rid);
			_ui->toggle_rid_selection (rid);
			DEBUG_TRACE (DEBUG::GenericMidi, string_compose ("Function: SelectToggle = %1\n", rid));
		}
		break;

	case TrackSetMute:
		break;
	case TrackSetSolo:
		break;
	case TrackSetSoloIsolate:
		break;
	case TrackSetGain:
		break;
	case TrackSetRecordEnable:
		break;
	default:
		break;
	}
}

XMLNode&
MIDIFunction::get_state () const
{

	XMLNode* node = new XMLNode ("MIDIFunction");
	return *node;
}

int
MIDIFunction::set_state (const XMLNode& /*node*/, int /*version*/)
{
	return 0;
}

