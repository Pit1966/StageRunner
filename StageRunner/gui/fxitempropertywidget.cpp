#include "fxitempropertywidget.h"
#include "fxitem.h"
#include "fxaudioitem.h"

FxItemPropertyWidget::FxItemPropertyWidget(QWidget *parent) :
	QWidget(parent)
{
	cur_fx = 0;
	cur_fxa = 0;

	setupUi(this);
	setWindowTitle("Fx Editor");
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
	keyEdit->setText(QChar(fx->keyCode()));

	if (fx->type() == FX_AUDIO) {
		cur_fxa = static_cast<FxAudioItem*>(fx);
		initialVolDial->setValue(cur_fxa->initialVolume);
		audioGroup->setVisible(true);
	} else {
		cur_fxa = 0;
		audioGroup->setVisible(false);
	}

	sceneGroup->setVisible(false);

	return true;
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
		if (arg1.size()) {
			cur_fx->setKeyCode(arg1.at(0).toUpper().unicode());
		} else {
			cur_fx->setKeyCode(0);
		}
		cur_fx->setModified(true);
		emit modified();
	}
}
