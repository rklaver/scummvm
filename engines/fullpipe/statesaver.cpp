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

#include "common/memstream.h"

#include "fullpipe/fullpipe.h"

#include "fullpipe/gameloader.h"
#include "fullpipe/objects.h"

namespace Fullpipe {

void GameLoader::writeSavegame(Scene *sc, const char *fname) {
	GameVar *v = _gameVar->getSubVarByName("OBJSTATES")->getSubVarByName("SAVEGAME");

	if (!v) {
		v = _gameVar->getSubVarByName("OBJSTATES")->addSubVarAsInt("SAVEGAME", 0);

		if (!v) {
			warning("No state to save");
			return;
		}
	}

	SaveHeader header;

	v->setSubVarAsInt("Scene", sc->_sceneId);

	saveScenePicAniInfos(sc->_sceneId);
	memset(&header, 0, sizeof(header));

	header.saveSize = 48;
	strcpy(header.magic, "FullPipe Savegame");
	header.updateCounter = _updateCounter;
	header.unkField = 1;

	Common::MemoryWriteStreamDynamic stream;

	MfcArchive *archive = new MfcArchive(&stream);

	v = _gameVar->getSubVarByName("OBJSTATES");

	GameVar *nxt = 0;
	GameVar *prv = 0;
	GameVar *par;
	if (v) {
		nxt = v->_nextVarObj;
		prv = v->_prevVarObj;
		par = v->_parentVarObj;
		v->_parentVarObj = 0;
		v->_nextVarObj = 0;
		v->_prevVarObj = 0;
	}

	archive->writeObject(v);

	if (v) {
		v->_parentVarObj = par;
		v->_nextVarObj = nxt;
		v->_prevVarObj = prv;
	}

	getGameLoaderInventory()->savePartial(*archive);

	archive->writeUint32LE(_sc2array.size());

	for (uint i = 0; i < _sc2array.size(); i++) {
		archive->writeUint32LE(_sc2array[i]._picAniInfosCount);

		for (uint j = 0; j < _sc2array[i]._picAniInfosCount; j++) {
			_sc2array[i]._picAniInfos[j]->save(*archive);
		}
	}

	header.encSize = stream.size();

	// Now obfuscate the data
	for (uint i = 0; i < header.encSize; i++)
		stream.getData()[i] += i & 0x7f;

	if (_savegameCallback)
		_savegameCallback(archive, true);

	// Now dump it into save file
	Common::OutSaveFile *saveFile = g_system->getSavefileManager()->openForSaving(fname);

	saveFile->write(&header, sizeof(header));

	saveFile->write(stream.getData(), stream.size());

	saveFile->finalize();

	delete saveFile;
}


void PicAniInfo::save(MfcArchive &file) {
	debugC(5, kDebugLoading, "PicAniInfo::save()");

	file.writeUint32LE(type);
	file.writeUint16LE(objectId);
	file.writeUint16LE(field_6);
	file.writeUint32LE(field_8);
	file.writeUint16LE(sceneId);
	file.writeUint16LE(field_E);
	file.writeSint32LE(ox);
	file.writeSint32LE(oy);
	file.writeUint32LE(priority);
	file.writeUint16LE(staticsId);
	file.writeUint16LE(movementId);
	file.writeUint16LE(dynamicPhaseIndex);
	file.writeUint16LE(flags);
	file.writeUint32LE(field_24);
	file.writeUint32LE(someDynamicPhaseIndex);
}

void GameVar::save(MfcArchive &file) {
	file.writePascalString(_varName);
	file.writeUint32LE(_varType);

	switch (_varType) {
	case 0:
		file.writeUint32LE(_value.intValue);
		break;
	case 1:
		file.writeUint32LE(_value.intValue); // FIXME
		break;
	case 2:
		file.writePascalString(_value.stringValue);
		break;
	default:
		error("Unknown var type: %d (0x%x)", _varType, _varType);
	}

	file.writeObject(_parentVarObj);
	file.writeObject(_prevVarObj);
	file.writeObject(_nextVarObj);
	file.writeObject(_field_14);
	file.writeObject(_subVars);
}

} // End of namespace Fullpipe
