#include "BPushButton.h"
#include <QString>
#include <QMouseEvent>

BPushButton::BPushButton(const QPixmap& normal, const QPixmap& hold, const QPixmap& press, QWidget *parent)
	: QLabel(parent), normalPic(normal), holdPic(hold), pressPic(press),inside(false), is_pressed(false)
{
	setFixedSize(normalPic.size());
	setPixmap(normalPic);
}

BPushButton::~BPushButton()
{
}

bool BPushButton::isClicking()
{
    return is_pressed;
}

void BPushButton::enterEvent(QEvent*)
{
    if (!isEnabled())
        return;

    setPixmap(holdPic);
    inside = true;
}

void BPushButton::leaveEvent(QEvent*)
{
    setPixmap(normalPic);
    inside = false;
    is_pressed = false;
}

void BPushButton::mousePressEvent(QMouseEvent* e)
{
	if (!isEnabled())
		return;
    emit pressed();
	setPixmap(pressPic);
    is_pressed = true;
    e->accept();
}

void BPushButton::mouseMoveEvent(QMouseEvent* e)
{
    e->accept();
}

void BPushButton::mouseReleaseEvent(QMouseEvent* e)
{
    if (!isEnabled() || !is_pressed || !inside)
        return;

    setPixmap(holdPic);
    is_pressed = false;
    emit clicked();
    e->accept();
}

void BPushButton::mouseDoubleClickEvent(QMouseEvent* e)
{
    emit doubleclicked();
}
