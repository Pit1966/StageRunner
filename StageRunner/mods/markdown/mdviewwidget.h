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
	QByteArray m_mdData;
	QString m_viewHtmlData;

	bool m_mdParseActive	= false;

public:
	explicit MDViewWidget(QWidget *parent = nullptr);
	~MDViewWidget();

	bool loadDocument(const QString &filePath);
	bool renderDocument();

protected:
	static void cbProcessOutput(const char *input, unsigned input_size, void *myobj);

private slots:
	void renderMD(bool keepScrollAnchor = false);

	void on_fileChooserButton_clicked();

	void on_closeButton_clicked();
	void on_showButton_clicked();
	void on_mdEdit_textChanged();
	void on_editButton_clicked();
};

#endif // MDVIEWWIDGET_H
