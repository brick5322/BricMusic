#include "BAlbumSlider.h"

#include <QBitmap>
#include <QPainter>
#include <QSvgRenderer> 
#include <cmath>
#include <QPaintEvent>

#ifdef _DEBUG
#include <QDebug>
#endif
const QString& BAlbumSlider::defaultPic = QString(":/img/defaultAlbum.tsvg");

BAlbumSlider::BAlbumSlider(const QString& normal, const QString& press, int radius, int maximum, QWidget* parent)
	: QLabel(parent), handle(normal, normal, press, this), radius(radius), maximum(maximum),
	is_clicking(false), is_entered(false), is_handlePressed(false), is_longPress_timing(false), is_rotated(false),
	longPress_duration(0), image_rotate_angle(0), _value(0), length(0)
{
	init();
}

BAlbumSlider::BAlbumSlider(const QPixmap& normal, const QPixmap& press, int radius, int maximum, QWidget* parent) :
	QLabel(parent), handle(normal, normal, press, this), radius(radius), maximum(maximum),
	is_clicking(false), is_entered(false), is_handlePressed(false), is_longPress_timing(false), is_rotated(false),
	longPress_duration(0), image_rotate_angle(0), _value(0), length(0)
{
	init();
}

void BAlbumSlider::init()
{
	int hw = handle.width();
	int sz = radius * 2 + hw;

	handle.setGeometry(radius, 0, hw, hw);
	handle.installEventFilter(this);

	setMouseTracking(true);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	resize(sz, sz);

	setAlbumPic(nullptr, 0);

	frontPen.setColor(Qt::black);
	frontPen.setWidth(radius / 5);
	backPen.setColor(Qt::white);
	backPen.setWidth(radius / 5);

	flush_interval = 1000 / defaultFrame;
	setAlbumPic(NULL, 0);
	setMask(QRegion(0, 0, sz, sz, QRegion::Ellipse));
	QObject::connect(&handle, &BPushButton::pressed, [this]() {emit this->handlePressed(); });
	timerID = startTimer(flush_interval);
}

BAlbumSlider::~BAlbumSlider()
{
	killTimer(timerID);
}

void BAlbumSlider::setFrontPen(const QColor& color, int width)
{
	frontPen.setColor(color);
	frontPen.setWidth(width);
}

void BAlbumSlider::setBackPen(const QColor& color, int width)
{
	backPen.setColor(color);
	backPen.setWidth(width);
}

double BAlbumSlider::value()
{
	return _value;
}

void BAlbumSlider::setValue(double value)
{
	if (!handle.isClicking())
	{
		_value = value;
		length = value;
		emit valueChanged(value);
	}
}

void BAlbumSlider::setAlbumPic(uchar* picdata, int size)
{
	if (picdata)
	{
		albumPic.loadFromData(picdata, size);
		albumPic = albumPic.scaled(radius * 2, radius * 2, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		if (albumPic.width() > albumPic.height())
			albumPic = albumPic.copy(albumPic.width() / 2 - radius, 0, radius * 2, radius * 2);
		else if (albumPic.width() < albumPic.height())
			albumPic = albumPic.copy(0, albumPic.height() / 2 - radius, radius * 2, radius * 2);
		PixmapToRound(albumPic);
	}
	else
	{
		albumPic = QPixmap(radius * 2, radius * 2);
		albumPic.fill(Qt::transparent);
		QPainter painter(&albumPic);
		QSvgRenderer(_BAlbumSlider::defaultPic).render(&painter);
	}
	return;
}

void BAlbumSlider::pauseRotate()
{
	is_rotated = false;
}

void BAlbumSlider::startRotate()
{
	is_rotated = true;
}

void BAlbumSlider::resetRotate()
{
	is_rotated = false;
	image_rotate_angle = 0;
}

void BAlbumSlider::clickInterupt()
{
	is_clicking = false;
	longPress_duration = 0;
	is_longPress_timing = false;
}

void BAlbumSlider::setMaximum(double max)
{
	maximum = max;
}

bool BAlbumSlider::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::MouseMove)
	{
		QPoint pos = handle.mapToParent(static_cast<QMouseEvent*>(e)->pos());
		length = calValue(pos);
		return true;
	}
	else if (e->type() == QEvent::MouseButtonRelease)
	{
		length = calValue(handle.mapToParent(static_cast<QMouseEvent*>(e)->pos()));
		setValue(length);
		emit sliderMoved(length);
		return QWidget::eventFilter(obj, e);
	}
	return QWidget::eventFilter(obj, e);
}

void BAlbumSlider::mousePressEvent(QMouseEvent* e)
{
	if (e->button() != Qt::LeftButton)
		return;
	if (((double)e->x() - center().x()) * ((double)e->x() - center().x()) + ((double)e->y() - center().y()) * ((double)e->y() - center().y()) < (radius * 0.7) * (radius * 0.7))
	{
		is_longPress_timing = true;
		is_clicking = true;
		e->accept();
		return;
	}
}

void BAlbumSlider::mouseMoveEvent(QMouseEvent* e)
{
	if (e->button() != Qt::LeftButton)
		return;
#ifdef _DEBUG
	qDebug() << "mouseMove" << e->button();
#endif // _DEBUG
}

void BAlbumSlider::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() != Qt::LeftButton)
		return;
	if (is_clicking)
	{
		emit clicked();
		is_longPress_timing = false;
		longPress_duration = 0;
	}
}

void BAlbumSlider::enterEvent(QEvent*)
{
	is_entered = true;
}

void BAlbumSlider::leaveEvent(QEvent*)
{
	is_entered = false;
}

void BAlbumSlider::paintEvent(QPaintEvent*)
{
	int width = handle.width() / 2;
	double angle = length / maximum;
	QPen backPen = this->backPen;
	QPen frontPen = this->frontPen;
	if (is_entered)
	{
		frontPen.setWidth(frontPen.width() * 2);
		backPen.setWidth(backPen.width() * 2);
	}
	constexpr int full_circle = 5760;
	constexpr int top_pos = 1440;
	handle.move(center() - QPoint(radius * sin(angle * 2 * 3.14) + width, radius * cos(angle * 2 * 3.14) + width));
	QPainter albumpic(this);
	albumpic.setRenderHints(QPainter::SmoothPixmapTransform);
	albumpic.translate(center());
	albumpic.rotate(image_rotate_angle);
	albumpic.translate(QPoint(0, 0) - center());
	albumpic.drawPixmap(width, width, albumPic);

	QPainter circle(this);
	circle.setRenderHints(QPainter::Antialiasing);
	circle.setPen(backPen);
	circle.drawArc(width, width, radius * 2, radius * 2, 0, full_circle);
	circle.setPen(frontPen);
	circle.drawArc(width, width, radius * 2, radius * 2, top_pos, angle * full_circle);
}

void BAlbumSlider::timerEvent(QTimerEvent*)
{
	if (is_rotated)
		if (--image_rotate_angle < 0)
			image_rotate_angle = 360;
	longPress_duration += is_longPress_timing;
	if (longPress_duration >= defaultFrame)
	{
		emit longPressed();
		is_longPress_timing = false;
		is_clicking = false;
		longPress_duration = 0;
	}
	this->update();
}

QPoint BAlbumSlider::center()
{
	int w = width() / 2;
	return QPoint(w, w);
}

void BAlbumSlider::PixmapToRound(QPixmap& src)
{
	if (src.isNull())
		return;
	QBitmap mask(radius * 2, radius * 2);
	QPainter painter(&mask);
	painter.fillRect(0, 0, src.width(), src.height(), Qt::white);
	painter.setBrush(Qt::black);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.drawRoundedRect(0, 0, radius * 2, radius * 2, 100, 100);
	src.setMask(mask);

}

double BAlbumSlider::calValue(const QPoint& pos)
{
	QPoint len = pos - center();
	double ret = maximum / 2 + atan2(len.x(), len.y()) / (2 * 3.14) * maximum;
	return ret;
}

QPoint BAlbumSlider::getHandlePos()
{
	return center() - QPoint(radius * sin(_value / maximum), radius * cos(_value / maximum));
}

QPixmap BAlbumSlider::loadSVG(const char* filepath, int w, int h, const QColor& color)
{
	QPixmap ret(w, h);
	ret.fill(Qt::transparent);
	QFile file(filepath);
	file.open(QIODevice::ReadOnly);
	QByteArray arr = file.readAll();
	if (color != Qt::transparent)
		arr.replace("&color&", color.name(QColor::HexRgb).toUtf8());
	QPainter painter(&ret);
	QSvgRenderer(arr).render(&painter);
	return ret;
}

bool BAlbumSlider::isRotated()
{
	return is_rotated;
}
