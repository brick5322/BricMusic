#pragma once

#include <QWidget>
#include <QLabel>

class BPushButton : public QLabel
{
	Q_OBJECT

public:
	BPushButton(const QPixmap& normal, const QPixmap& hold, const QPixmap& press,QWidget *parent = Q_NULLPTR);
	~BPushButton();
	bool isClicking();
signals:
	void clicked();
	void pressed();
	void doubleclicked();
protected:
	void enterEvent(QEvent*);
	void leaveEvent(QEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
private:
	QPixmap normalPic;
	QPixmap holdPic;
	QPixmap pressPic;

	bool inside;
	bool is_pressed;
};
