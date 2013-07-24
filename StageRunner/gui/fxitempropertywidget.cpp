#include "fxitempropertywidget.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "usersettings.h"
#include "appcentral.h"
#include "qtstatictools.h"

#include <QFileDialog>
#include <QFileInfo>

FxItemPropertyWidget::FxItemPropertyWidget(QWidget *parent) :
	QWidget(parent)
{
	cur_fx = 0;
	cur_fxa = 0;
	cur_fxs = 0;

	once_edit_f = false;

	setupUi(this);
	setWindowTitle("Fx Editor");
	setObjectName("DockFxEditor");

	keyEdit->setSingleKeyEditEnabled(true);

	connect(nameEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(keyEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(faderCountEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(audioFilePathEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(fadeInTimeEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(fadeOutTimeEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));

}

bool FxItemPropertyWidget::setFxItem(FxItem *fx)
{
	if (!FxItem::exists(fx)) {
		cur_fx = 0;
		return false;
	}
	cur_fx = fx;

	nameEdit->setText(fx->name());
	idEdit->setText(QString::number(fx->id()));
	keyEdit->setText(QtStaticTools::keyToString(fx->keyCode()));

	if (fx->fxType() == FX_AUDIO) {
		cur_fxa = static_cast<FxAudioItem*>(fx);
		initialVolDial->setValue(cur_fxa->initialVolume);
		audioFilePathEdit->setText(cur_fxa->filePath());
		audioLoopsSpin->setValue(cur_fxa->loopTimes);
		audioGroup->setVisible(true);
	} else {
		cur_fxa = 0;
		audioGroup->setVisible(false);
	}

	if (fx->fxType() == FX_AUDIO_PLAYLIST) {
		cur_fxa = static_cast<FxAudioItem*>(fx);
		initialVolDial->setValue(cur_fxa->initialVolume);
		audioFilePathEdit->clear();
		audioLoopsSpin->setValue(cur_fxa->loopTimes);
		audioGroup->setVisible(true);
	} else {
		cur_fxa = 0;
		audioGroup->setVisible(false);
	}


	if (fx->fxType() == FX_SCENE) {
		cur_fxs = static_cast<FxSceneItem*>(fx);
		faderCountEdit->setText(QString::number(cur_fxs->tubeCount()));
		fadeInTimeEdit->setText(QtStaticTools::msToTimeString(cur_fxs->defaultFadeInTime));
		fadeOutTimeEdit->setText(QtStaticTools::msToTimeString(cur_fxs->defaultFadeOutTime));

		sceneGroup->setVisible(true);
	} else {
		cur_fxs = 0;
		sceneGroup->setVisible(false);
	}


	return true;
}

void FxItemPropertyWidget::setEditable(bool state, bool once)
{
	QList<PsLineEdit*>childs = findChildren<PsLineEdit*>();
	foreach(PsLineEdit* edit, childs) {
		edit->setEditable(state);
		edit->setWarnColor(true);
	}
	once_edit_f = once;
	if (!state) {
		editOnceButton->setText(tr("Edit once"));

	}
}


void FxItemPropertyWidget::on_initialVolDial_sliderMoved(int position)
{
	if (FxItem::exists(cur_fxa)) {
		cur_fxa->initialVolume = position;
		cur_fxa->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_nameEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fx)) {
		cur_fx->setName(arg1);
		cur_fx->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_keyEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fx)) {
		QString norm = arg1.trimmed().toUpper();
		if (norm != arg1) keyEdit->setText(norm);

		if (norm.size()) {
			cur_fx->setKeyCode(QtStaticTools::stringToKey(norm));
		} else {
			cur_fx->setKeyCode(0);
		}
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_faderCountEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fxs)) {
		bool ok;
		int num = arg1.toInt(&ok);
		if (ok && num != cur_fxs->tubeCount()) {
			cur_fxs->setTubeCount(num);
		}
	}
}

void FxItemPropertyWidget::on_audioFilePathEdit_doubleClicked()
{
	QString path = QFileDialog::getOpenFileName(this
												,tr("Choose Audio File")
												,AppCentral::instance()->userSettings->pLastAudioFxImportPath);
	if (path.size()) {
		AppCentral::instance()->userSettings->pLastAudioFxImportPath = path;

		if (FxItem::exists(cur_fxa)) {
			cur_fxa->setFilePath(path);
			cur_fxa->setModified(true);
			audioFilePathEdit->setText(cur_fxa->filePath());
			emit modified();
		}
	}
}

void FxItemPropertyWidget::on_fadeInTimeEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fxs)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxs->defaultFadeInTime != time_ms) {
		cur_fxs->defaultFadeInTime = time_ms;
		cur_fxs->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_fadeOutTimeEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fxs)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxs->defaultFadeOutTime != time_ms) {
		cur_fxs->defaultFadeOutTime = time_ms;
		cur_fxs->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_keyClearButton_clicked()
{
	keyEdit->clear();

	if (!FxItem::exists(cur_fx)) return;

	if (cur_fx->keyCode()) {
		cur_fx->setKeyCode(0);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_editOnceButton_clicked()
{
	if (AppCentral::instance()->isEditMode()) return;

	if (!once_edit_f) {
		editOnceButton->setText(tr("Cancel Edit Mode"));
	}

	setEditable(!once_edit_f,!once_edit_f);
}

void FxItemPropertyWidget::finish_edit()
{
	setEditable(false,false);
}

void FxItemPropertyWidget::on_audioLoopsSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxa)) return;

	if (cur_fxa->loopTimes != arg1) {
		cur_fxa->loopTimes = arg1;
		cur_fxa->setModified(true);
	}

}
