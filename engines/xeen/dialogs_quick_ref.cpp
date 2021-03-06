/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "xeen/dialogs_quick_ref.h"
#include "xeen/resources.h"
#include "xeen/xeen.h"

namespace Xeen {

void QuickReferenceDialog::show(XeenEngine *vm) {
	QuickReferenceDialog *dlg = new QuickReferenceDialog(vm);
	dlg->execute();
	delete dlg;
}

void QuickReferenceDialog::execute() {
	Combat &combat = *_vm->_combat;
	EventsManager &events = *_vm->_events;
	Party &party = *_vm->_party;
	Screen &screen = *_vm->_screen;
	Common::String lines[8];

	events.setCursor(0);

	for (uint idx = 0; idx < (combat._globalCombat == 2 ? combat._combatParty.size() :
			party._activeParty.size()); ++idx) {
		Character &c = combat._globalCombat == 2 ? *combat._combatParty[idx] :
			party._activeParty[idx];
		Condition condition = c.worstCondition();
		lines[idx] = Common::String::format(QUICK_REF_LINE,
			idx * 10 + 24, idx + 1, c._name.c_str(),
			CLASS_NAMES[c._class][0], CLASS_NAMES[c._class][1], CLASS_NAMES[c._class][2],
			c.statColor(c.getCurrentLevel(), c._level._permanent), c._level._permanent,
			c.statColor(c._currentHp, c.getMaxHP()), c._currentHp,
			c.statColor(c._currentSp, c.getMaxSP()), c._currentSp,
			c.statColor(c.getArmorClass(), c.getArmorClass(true)), c.getArmorClass(),
			CONDITION_COLORS[condition],
			CONDITION_NAMES[condition][0], CONDITION_NAMES[condition][1],
			CONDITION_NAMES[condition][2], CONDITION_NAMES[condition][3]
		);
	}

	int food = (party._food / party._activeParty.size()) / 3;
	Common::String msg = Common::String::format(QUICK_REFERENCE,
		lines[0].c_str(), lines[1].c_str(), lines[2].c_str(),
		lines[3].c_str(), lines[4].c_str(), lines[5].c_str(),
		lines[6].c_str(), lines[7].c_str(),
		party._gold, party._gems,
		food, food == 1 ? "" : "s"
	);

	Window &w = screen._windows[24];
	bool windowOpen = w._enabled;
	if (!windowOpen)
		w.open();
	w.writeString(msg);
	w.update();

	// Wait for a key/mouse press
	events.clearEvents();
	while (!_vm->shouldQuit() && !events.isKeyMousePressed())
		events.pollEventsAndWait();
	events.clearEvents();

	if (!windowOpen)
		w.close();
}

} // End of namespace Xeen
