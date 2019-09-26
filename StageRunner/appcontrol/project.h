//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef PROJECT_H
#define PROJECT_H

#include "../tool/varset.h"
#include "system/importfxmaster/fxMaster.h"

class FxList;
class FxItem;


class Project : public QObject, public VarSet
{
	Q_OBJECT

public:
	class EXPORT_RESULT {
	public:
		bool wasCompletelySuccessful;
		bool allowUserInteraction;
		bool setModified;
		bool exportWithRelativeFxFilePaths;
		bool forceKeepFxItemIDs;
		QStringList resultMessageList;
		QStringList errorMessageList;
		int audioFileCopyCount;
		int audioFileExistCount;
		int clipFileCount;
	public:
		EXPORT_RESULT()
			: wasCompletelySuccessful(true)
			, allowUserInteraction(false)
			, setModified(false)
			, exportWithRelativeFxFilePaths(false)
			, forceKeepFxItemIDs(false)
			, audioFileCopyCount(0)
			, audioFileExistCount(0)
			, clipFileCount(0)
		{}
	};

public:
	pbool pAutoProceedSequence;
	QString curProjectFilePath;					///< Stores the location of the project file, is previously saved or loaded from disk (used for project-save)

	int loadErrorLineNumber;					///< this holds the line number after load project has failed due to a script error
	QString loadErrorLineString;
	bool loadErrorFileNotExisting;

protected:
	pint64 pProjectId;
	pint32 pProjectFormat;
	pstring pProjectName;
	pstring pComment;
	pstring pProjectBaseDir;					///< this is the base directory for relative path names in FXs

private:
	FxList *fxList;
	QStringList m_mediaFileSearchDirs;			///< these are temporary entries for directories which will be scanned, if a file is not found on load

public:
	Project();
	Project(const Project &o, bool keepAllFxIitemIDs = false);
	~Project();

	inline QString projectName() const {return pProjectName;}

	bool cloneProjectFrom(const Project &o, bool keepAllFxIitemIDs = false);

	void clear();

	bool saveToFile(const QString & path);
	bool loadFromFile(const QString & path);
	bool isModified();
	void setModified(bool state);

	void postLoadProcessFxList();
	bool postLoadResetFxScenes();

	inline FxList *mainFxList() {return fxList;}

	bool consolidateToDir(const QString &exportProName, const QString &dirname, EXPORT_RESULT &result, QWidget *parentWid);
	bool copyAllAudioItemFiles(FxList * srcFxList, const QString & destDir, EXPORT_RESULT &result);
	bool checkFxItemList(FxList * srcFxList, EXPORT_RESULT &result);

	bool askForFindFileDialog(FxItem *fx, int *ret);
	bool showFindFileDialog(FxItem *fx);

	bool loadFxMasterProject(const QString &path);

private:
	bool fxmLoadChunkName(QDataStream &in, FXM::FX &mem);
	bool fxmLoadChunkAudio(QDataStream &in, quint32 size);
	bool fxmLoadChunkUserName(QDataStream &in, quint32 size);
	bool fxmLoadChunkAudioSetting(QDataStream &in, quint32 size);
	bool fxmLoadChunkDMXSetting(QDataStream &in, quint32 size);

	void init();
	bool generateProjectNameFromPath();

signals:
	void projectLoadedOrSaved(const QString &path, bool ok);
	void progressWanted(int loRange, int hiRange);
	void progressed(int val);
};

#endif // PROJECT_H
