#include "mixergroup.h"
#include "mixerchannel.h"

#include <QHBoxLayout>

MixerGroup::MixerGroup(QWidget *parent) :
	QWidget(parent)
{
	default_min = 0;
	default_max = 100;

	QHBoxLayout * mixerlayout = new QHBoxLayout;
	mixerlayout->setSpacing(0);
	mixerlayout->setMargin(0);
	setLayout(mixerlayout);

	setMixerCount(12);
}

/**
 * @brief Set Number of Mixers to be displayed
 * @param number
 *
 * This function deletes all current mixers if there numbe is lower
 * than new one
 */
void MixerGroup::setMixerCount(int number)
{
	while (mixerlist.size() > number) {
		delete mixerlist.takeLast();
	}

	while (mixerlist.size() < number) {
		appendMixer();
	}
}

void MixerGroup::clear()
{
	while (mixerlist.size()) {
		delete mixerlist.takeFirst();
	}
}

MixerChannel *MixerGroup::appendMixer()
{
	MixerChannel *mixer = new MixerChannel;
	mixer->setId(mixerlist.size());
	mixer->setRange(default_min,default_max);
	mixerlist.append(mixer);
	layout()->addWidget(mixer);
	connect(mixer,SIGNAL(mixerMoved(int,int)),this,SLOT(on_mixer_moved(int,int)));

	return mixer;
}

void MixerGroup::setRange(int min, int max)
{
	default_min = min;
	default_max = max;

	for (int t=0; t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		mixer->setRange(min,max);
		mixer->update();
	}
}

void MixerGroup::on_mixer_moved(int val, int id)
{
	if (id >= 0) {
		emit mixerMoved(val, id);
	}
}


void MixerGroup::notifyChangedDmxUniverse(int universe, const QByteArray &dmxValues)
{
	for (int t=0;t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		if (mixer->dmxUniverse() == universe) {
			int target_value = quint8(dmxValues[mixer->dmxChannel()]) * mixer->maximum() / 255;
			if (mixer->refValue() != target_value) {
				mixer->setRefValue(target_value);
				mixer->update();
			}
		}
	}
}

void MixerGroup::setRefSliderColorIndex(int colidx)
{
	for (int t=0;t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		mixer->setRefSliderColorIndex(colidx);
	}
}
