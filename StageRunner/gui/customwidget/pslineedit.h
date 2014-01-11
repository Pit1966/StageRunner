#ifndef PSLINEEDIT_H
#define PSLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QPalette>

class PsLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
	Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized)
	Q_PROPERTY(bool eventsShared READ eventsShared WRITE setEventsShared)
	Q_PROPERTY(bool warnColor READ warnColor WRITE setWarnColor)

private:
	bool prop_editable;
	bool prop_minimized;
	bool prop_events_shared;
	bool prop_warn_color;
	bool single_key_mode_f;
	int current_key;
	int current_modifier;
	QPalette org_palette;

public:
	PsLineEdit(QWidget *parent = 0);
	inline int currentKey() {return current_key;}
	inline int currentModifier() {return current_modifier;}
	void setSingleKeyEditEnabled(bool state);
	void setEditable(bool state);
	void setDefaultPalette();
	bool isEditable() const {return prop_editable;}
	void setMinimized(bool state);
	bool isMinimized() const {return prop_minimized;}
	void setEventsShared(bool state) {prop_events_shared = state;}
	bool eventsShared() {return prop_events_shared;}
	void setWarnColor(bool state);
	bool warnColor() const {return prop_warn_color;}

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	void updateEditWarnColor();

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);


signals:
	void clicked();
	void doubleClicked();
	void tabPressed();
	void enterPressed();

public slots:

};

#endif // PSLINEEDIT_H
