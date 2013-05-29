#ifndef PSLINEEDIT_H
#define PSLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class PsLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
	Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized)

private:
	bool prop_editable;
	bool prop_minimized;
	bool single_key_mode_f;
	int current_key;
	int current_modifier;

public:
	PsLineEdit(QWidget *parent = 0);
	inline int currentKey() {return current_key;}
	inline int currentModifier() {return current_modifier;}
	void setSingleKeyEditEnabled(bool state);
	void setEditable(bool state);
	bool isEditable() const {return prop_editable;}
	void setMinimized(bool state);
	bool isMinimized() const {return prop_minimized;}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);


signals:
	void clicked();
	void doubleClicked();

public slots:

};

#endif // PSLINEEDIT_H
