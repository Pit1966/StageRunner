#include "messagehub.h"
#include "config.h"

#include <QDir>

MessageHub *MessageHub::my_instance = 0;


VMsg::VMsg()
{
	init();
}

void VMsg::init()
{
	setClass(PrefVarCore::MESSAGE,"DialogMessage");

	addExistingVar((qint32&)type,"MsgType");
	addExistingVar(messageText,"MsgText");
	addExistingVar(doNotShowAgain,"DoNotShowAgain",false);
	addExistingVar((qint32&)doNotShowDefaultButton,"DoNotShowDefButton");

}


MessageHub::MessageHub() :
	QObject()
  , VarSet()
  , msglist(this)
{
	init();

	QString path = QString("%1/.config/%2/%3.msg")
			.arg(QDir::homePath()).arg(APP_CONFIG_PATH).arg(APPNAME);
	fileLoad(path);

// 	readFromPref();
}

MessageHub::~MessageHub()
{
	QString path = QString("%1/.config/%2/%3.msg")
			.arg(QDir::homePath()).arg(APP_CONFIG_PATH).arg(APPNAME);
	fileSave(path,false,true);

// 	writeToPref();
}

void MessageHub::init()
{
	setClass(PrefVarCore::MESSAGE_HUB,"MessageList");
	registerQSetGlobal("DialogMessages and Default actions");

	addExistingVarSetList(msglist,"Messages",PrefVarCore::MESSAGE);
}

VMsg *MessageHub::find_message(const QString &text, VMsg::Type msgType)
{
	for (int t=0; t<msglist.size(); t++) {
		if (msgType != VMsg::MT_UNDEF) {
			if (msglist.at(t)->type == msgType) return msglist.at(t);
		} else {
			if (msglist.at(t)->messageText == text) return msglist.at(t);
		}
	}

	return 0;
}


MessageHub *MessageHub::instance()
{
	if (!my_instance) {
		my_instance = new MessageHub();
	}
	return my_instance;
}

void MessageHub::destroy()
{
	if (my_instance) {
		delete my_instance;
		my_instance = 0;
	}
}

Ps::Button MessageHub::question(const QString &mainText, const QString &subText, VMsg::Type msgType)
{
	return MessageHub::instance()->showQuestion(mainText, subText, msgType);
}

Ps::Button MessageHub::showQuestion(const QString &mainText, const QString &subText, VMsg::Type msgType)
{
	VMsg *msg = find_message(mainText, msgType);
	if (msg && msg->doNotShowAgain) return Ps::Button(msg->doNotShowDefaultButton | Ps::AUTO);

	MessageDialog dialog;
	Ps::Button but = dialog.execMessage(mainText, subText);

	if (dialog.doNotShowAgain) {
		VMsg *msg = new VMsg;
		msg->messageText = mainText;
		msg->type = msgType;
		msg->doNotShowAgain = true;
		msg->doNotShowDefaultButton = but;
		msglist.append(msg);
	}

	return but;
}


