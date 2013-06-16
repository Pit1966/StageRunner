#ifndef FXLISTWIDGETITEM_H
#define FXLISTWIDGETITEM_H

#include "ui_fxlistwidgetitem.h"

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
		CT_FX_TYPE
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
	int current_button;
	int activation_indicator;
	QPoint drag_begin_pos;
	QPalette org_palette;

public:
	FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype = CT_UNDEF);
	FxListWidgetItem(const FxListWidgetItem & other);
	~FxListWidgetItem();
	void setText(const QString &txt);
	inline QString text() const {return itemText;}
	QPoint dragBeginPos() const {return drag_begin_pos;}

private:
	void init();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);


public slots:
	void setEditable(bool state);
	void setNeverEditable(bool state);
	void setSelected(bool state);
	void setActivationProgress(int perMille);

private slots:
	void if_edit_item_clicked();
	void if_edit_item_doubleclicked();
	void if_edit_item_edited(const QString & text);
	void if_label_item_doubleclicked();

signals:
	void itemClicked(FxListWidgetItem *);
	void itemDoubleClicked(FxListWidgetItem *);
	void itemTextEdited(FxListWidgetItem *, const QString &);
	void draged(FxListWidgetItem * item);

};

#endif // FXLISTWIDGETITEM_H
