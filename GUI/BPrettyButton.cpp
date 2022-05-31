#include "BPrettyButton.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>

BPrettyButton::BPrettyButton(QColor edge, QColor background, QColor highlight, QWidget *parent)
	: QLabel(parent),edge(edge),background(background),highlight(highlight),pic(size()* 0.8),
	is_pressed(false),is_inside(false),normalbrush(background)
{
	pic.fill(Qt::transparent);
	gra.setCenter(QPointF(10 + width() / 2,10 + height() / 2));
	gra.setRadius((double)width() / 2);
	gra.setColorAt(0, highlight);
	gra.setColorAt(1, background);
	gradientbrush = QBrush(gra);

}

BPrettyButton::~BPrettyButton()
{
}

void BPrettyButton::setTSVGpic(const QByteArray& TSVGdata)
{
	QByteArray SVGdata = TSVGdata;
	SVGdata.replace("&color&", edge.name(QColor::HexRgb).toUtf8());
	pic.fill(Qt::transparent);
	QSvgRenderer renderer(SVGdata);
	QPainter painter(&pic);
	renderer.render(&painter);
	update();
}

void BPrettyButton::setTSVGpic(const char* filepath)
{
	QFile file(filepath);
	file.open(QIODevice::ReadOnly);
	setTSVGpic(file.readAll());
	file.close();
}

void BPrettyButton::resize(int w)
{
	gra.setCenter(QPointF(10 + w / 2,10 + w / 2));
	gra.setRadius((double)w / 2);
	QLabel::resize(w + 20, w + 20);
	pic = QPixmap(w * 0.8, w * 0.8);
	pic.fill(Qt::transparent);
}

void BPrettyButton::enterEvent(QEvent*)
{
	is_inside = true;
	setMouseTracking(true);
	emit enter(true);
	update();
}

void BPrettyButton::leaveEvent(QEvent*)
{
	is_inside = false;
	setMouseTracking(false);
	emit enter(false);
	update();
}

void BPrettyButton::mousePressEvent(QMouseEvent* e)
{
	if (e->button() != Qt::LeftButton)
		return;
	is_pressed = true;
	update();
}

void BPrettyButton::mouseReleaseEvent(QMouseEvent*)
{
	if (is_pressed && is_inside)
		emit clicked();
	is_pressed = false;
	update();
}

void BPrettyButton::mouseMoveEvent(QMouseEvent* e)
{
	gra.setFocalPoint(e->pos());
	gradientbrush = QBrush(gra);
	update();
}

void BPrettyButton::paintEvent(QPaintEvent*)
{
	QBrush* brush = (is_inside && this->isEnabled()) ? &gradientbrush : &normalbrush;


	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QPen edgepen(edge);
	edgepen.setWidthF((double)width() * (is_pressed ? 0.04 : 0.02));

	painter.setPen(edgepen);
	painter.setBrush(*brush);

	painter.drawEllipse(10, 10, width()-20, width()-20);

	int w = (width() - pic.width())/2;
	painter.drawPixmap(w, w, pic.width(), pic.width(), pic);
}


