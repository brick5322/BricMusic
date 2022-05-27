#pragma once

#include <QWidget>
#include "BPushButton.h"
#include <QPen>
#include <QTimer>

namespace _BAlbumSlider {
    const QString defaultPic = QString(":/img/defaultAlbum.tsvg");
}
class BAlbumSlider : public QLabel
{
	Q_OBJECT

public:
	static constexpr int defaultFrame = 30;
	static const QString& defaultPic;
	BAlbumSlider(const QString& normal, const QString& press, int radius, int maximum, QWidget* parent = nullptr);
	BAlbumSlider(const QPixmap& normal, const QPixmap& press, int radius, int maximum, QWidget* parent = nullptr);
	~BAlbumSlider();

	void setFrontPen(const QColor& color, int width);
	void setBackPen(const QColor& color, int width);
	double value();
	bool isRotated();
	QPoint center();
	static QPixmap loadSVG(const char* filepath, int w, int h, const QColor& color = Qt::transparent);
signals:
	void clicked();
	void handlePressed();
	void longPressed();
	void valueChanged(int value);
	void sliderMoved(int value);
public slots:
	void setAlbumPic(uchar* picdata, int size);
	void setValue(double);
	void setMaximum(double);
	void pauseRotate();
	void startRotate();
	void clickInterupt();
protected:
	bool eventFilter(QObject*, QEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void enterEvent(QEvent*);
	void leaveEvent(QEvent*);
	void paintEvent(QPaintEvent*);
	void timerEvent(QTimerEvent*);

private:
	void PixmapToRound(QPixmap& src);
	double calValue(const QPoint& pos);
	void init();
	QPoint getHandlePos();

	BPushButton handle;
	QPixmap albumPic;
	QPoint handlePoint;

	QPen backPen;
	QPen frontPen;

	const int radius;
	int maximum;

	double length;
	double _value;

	int timerID;
	int flush_interval;
	int image_rotate_angle;

	int longPress_duration;
	bool is_longPress_timing;
	bool is_clicking;
	bool is_handlePressed;
	bool is_entered;
	bool is_rotated;
};
