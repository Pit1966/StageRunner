#ifndef FXLISTWIDGETITEM_H
#define FXLISTWIDGETITEM_H

#include "ui_fxlistwidgetitem.h"
#include "commandsystem.h"

class FxItem;

class FxListWidgetItem : public QWidget, public Ui::FxListWidgetItem
{
	Q_OBJECT
public:
	enum ColumnType {
		CT_UNDEF,
		CT_NAME,
		CT_ID,
		CT_KEY,
		CT_FX_TYPE,
		CT_FADEIN_TIME,
		CT_FADEOUT_TIME,
		CT_PRE_DELAY,
		CT_POST_DELAY,
		CT_HOLD_TIME,
		CT_MOVE_TIME
	};

	FxItem *linkedFxItem;
	QString itemText;
	ColumnType columnType;

	int myRow;
	int myColumn;

private:
	bool is_editable_f;
	bool is_never_editable_f;
	bool is_selected_f;
	bool is_marked_f;

	int current_button;
	int activation_indicator_a;
	int activation_indicator_b;
	QPoint drag_begin_pos;
	QPalette org_palette;
	QColor indicator_a_color;
	QColor indicator_b_color;
	QColor marked_color;

	bool seek_mode_f;

public:
	FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype = CT_UNDEF);
	FxListWidgetItem(const FxListWidgetItem & other);
	~FxListWidgetItem();
	void setText(const QString &txt);
	inline QString text() const {return itemText;}
	QPoint dragBeginPos() const {return drag_begin_pos;}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;


private:
	void init();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);


public slots:
	void setEditable(bool state);
	void setNeverEditable(bool state);
	void setSelected(bool state);
	void setMarked(bool state);
	void setActivationProgress(int perMilleA, int perMilleB);
	void setActivationProgressB(int perMilleB);
	void setActivationProgressA(int perMilleA);

private slots:
	void if_edit_item_clicked();
	void if_edit_item_doubleclicked();
	void if_edit_item_edited(const QString & text);
	void if_edit_item_tab_pressed();
	void if_edit_item_enter_pressed();
	void if_label_item_doubleclicked();

signals:
	void itemClicked(FxListWidgetItem *);
	void itemDoubleClicked(FxListWidgetItem *);
	void itemTextEdited(FxListWidgetItem *, const QString &);
	void draged(FxListWidgetItem * item);
	void seekToPerMille(FxListWidgetItem * item, int perMille);
	void tabPressedInEdit(FxListWidgetItem * item);
	void enterPressedInEdit(FxListWidgetItem * item);
};

#endif // FXLISTWIDGETITEM_H
