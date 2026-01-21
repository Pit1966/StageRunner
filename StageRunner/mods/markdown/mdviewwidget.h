#ifndef MDVIEWWIDGET_H
#define MDVIEWWIDGET_H

#include <QWidget>

namespace Ui {
class MDViewWidget;
}

class MDViewWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::MDViewWidget *ui;
	QString m_currentFile;

	QString m_viewHtmlData;

public:
	explicit MDViewWidget(QWidget *parent = nullptr);
	~MDViewWidget();

protected:
	static void cbProcessOutput(const char *input, unsigned input_size, void *myobj);

private slots:
	void on_fileChooserButton_clicked();

	void on_closeButton_clicked();
	void on_showButton_clicked();
};

#endif // MDVIEWWIDGET_H
