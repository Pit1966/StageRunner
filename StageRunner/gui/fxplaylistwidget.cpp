#include "fxplaylistwidget.h"
#include "fxplaylistitem.h"
#include "fxlist.h"

FxPlayListWidget::FxPlayListWidget(FxPlayListItem *play_list_item, QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	setFxPlayListItem(play_list_item);
}

void FxPlayListWidget::setFxPlayListItem(FxPlayListItem *fxplay)
{
	playlist_item = fxplay;

	playListWidget->clear();

	for (int t=0; t<fxplay->size(); t++) {
		FxAudioItem *fxa = static_cast<FxAudioItem*>(fxplay->fxPlayList->at(t));

		QListWidgetItem *item = new QListWidgetItem(fxa->name());
		playListWidget->insertItem(t,item);
	}
}

void FxPlayListWidget::closeEvent(QCloseEvent *)
{
}

void FxPlayListWidget::on_closeButton_clicked()
{
	close();
}
