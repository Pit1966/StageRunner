#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include "varset.h"
#include "varsetlist.h"
#include "messagedialog.h"

#include <QObject>
#include <QString>


class VMsg : public VarSet
{
public:
	enum Type {
		MT_UNDEF
	};


protected:
	Type type;
	QString messageText;
	bool doNotShowAgain;
	Ps::Button doNotShowDefaultButton;

public:
	VMsg();

private:
	void init();

	friend class MessageHub;
};


class MessageHub : public QObject, public VarSet
{
	Q_OBJECT
private:
	static MessageHub *my_instance;
	VarSetList<VMsg*>msglist;

public:
	static MessageHub * instance();
	static void destroy();
	static Ps::Button question(const QString &mainText, const QString &subText, VMsg::Type msgType = VMsg::MT_UNDEF);

	Ps::Button showQuestion(const QString &mainText, const QString &subText, VMsg::Type msgType = VMsg::MT_UNDEF);


private:
	MessageHub();
	~MessageHub();
	void init();
	VMsg *find_message(const QString &text, VMsg::Type msgType);

signals:

public slots:

};

#endif // MESSAGEHUB_H
