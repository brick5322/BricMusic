#pragma once

#include <QLabel>

class BPrettyButton : public QLabel
{
	Q_OBJECT

public:
	BPrettyButton(QColor edge, QColor background, QColor highlight, QWidget *parent = Q_NULLPTR);
	~BPrettyButton();
    void setTSVGpic(const QByteArray& TSVGdata);
    void setTSVGpic(const char*);
    void resize(int w);
signals:
    void enter(bool);
    void clicked();
protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void paintEvent(QPaintEvent*);
private:
    bool is_pressed;
    bool is_inside;
    QPixmap pic;
    QColor edge;
    QColor highlight;
    QColor background;
    QBrush normalbrush;
    QBrush gradientbrush;
    QRadialGradient gra;
};
