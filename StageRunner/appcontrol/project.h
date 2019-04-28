#ifndef PROJECT_H
#define PROJECT_H

#include "../tool/varset.h"

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
	Project(const Project &o);
	~Project();

	inline QString projectName() const {return pProjectName;}

	bool cloneProjectFrom(const Project &o);

	void clear();

	bool saveToFile(const QString & path);
	bool loadFromFile(const QString & path);
	bool isModified();
	void setModified(bool state);

	void postLoadProcessFxList();
	bool postLoadResetFxScenes();

	inline FxList *mainFxList() {return fxList;}

	bool consolidateToDir(const QString &exportProName, const QString &dirname, QWidget *parentWid);
	bool copyAllAudioItemFiles(FxList * srcFxList, const QString & destDir, EXPORT_RESULT &result);
	bool checkFxItemList(FxList * srcFxList, EXPORT_RESULT &result);

	bool askForFindFileDialog(FxItem *fx, int *ret);
	bool showFindFileDialog(FxItem *fx);

private:
	void init();
	bool generateProjectNameFromPath();

signals:
	void projectLoadedOrSaved(const QString &path, bool ok);
	void progressWanted(int loRange, int hiRange);
	void progressed(int val);
};

#endif // PROJECT_H
