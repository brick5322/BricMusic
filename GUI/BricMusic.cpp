#include "BricMusic.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFocusEvent>

BricMusic::BricMusic(const QColor& color, QWidget* parent)
    : QWidget(parent),
    vol_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    lrc_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    pre_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    next_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    mode_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    btns_hidden(true)
{
    btns[0] = &pre_btn;
    btns[1] = &lrc_btn;
    btns[2] = &vol_btn;
    btns[3] = &mode_btn;
    btns[4] = &next_btn;

    albumslider = new BAlbumSlider(BAlbumSlider::loadSVG(":/img/handle-press.tsvg", 20, 20, QColor("#E8E8E8")), BAlbumSlider::loadSVG(":/img/handle-press.tsvg", 20, 20, color), 50, 500, this);
    albumslider->move(100, 100);
    albumslider->setFrontPen(color,3);
    albumslider->setBackPen(Qt::white,3);
    albumslider->start();

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);

    QPoint center = albumslider->mapToParent(QPoint(25,25));

    btns_pos[0] = (center + QPointF(-1, 0) * 50 * 1.75).toPoint();
    btns_pos[1] = (center + QPointF(-0.707, -0.707) * 50 * 1.75).toPoint();
    btns_pos[2] = (center + QPointF(0, -1) * 50 * 1.75).toPoint();
    btns_pos[3] = (center + QPointF(0.707, -0.707) * 50 * 1.75).toPoint();
    btns_pos[4] = (center + QPointF(1, 0) * 50 * 1.75).toPoint();

    for (int i = 0; i < 5; i++)
    {
        btns[i]->resize(50);
        btns[i]->move(center);
        btns[i]->hide();
    }
    vol_btn.setTSVGpic(":/img/play.tsvg");
    lrc_btn.setTSVGpic(":/img/lrc-able.tsvg");
    pre_btn.setTSVGpic(":/img/prev.tsvg");
    next_btn.setTSVGpic(":/img/next.tsvg");
    mode_btn.setTSVGpic(":/img/loopplayback.tsvg");
    ani.setDuration(150);
    ani.setPropertyName("pos");
    QObject::connect(&ani, &QAbstractAnimation::finished, this, &BricMusic::on_ani_finished);
}

bool BricMusic::eventFilter(QObject* obj,QEvent* e)
{
    if (e->type() == QEvent::MouseMove)
    {
        return true;
    }
    return QWidget::eventFilter(obj,e);
}

void BricMusic::enterEvent(QEvent*)
{
    this->activateWindow();
    this->raise();
    btns_hidden = false;
    if(ani.state()!=QAbstractAnimation::Running)
        on_ani_finished();
}

void BricMusic::leaveEvent(QEvent*)
{
    if (ani.state() == QAbstractAnimation::Running)
        btns_hidden = true;
}

void BricMusic::focusOutEvent(QFocusEvent*)
{
    btns_hidden = true;
    if (ani.state() != QAbstractAnimation::Running)
    {
        ani.setTargetObject(btns[0]);
        ani.setStartValue(btns_pos[0]);
        ani.setEndValue(albumslider->mapToParent(QPoint(25, 25)));
        ani.start();
    }
}

void BricMusic::on_ani_finished()
{
    int i = 0;
    QPoint center = albumslider->mapToParent(QPoint(25, 25));
    if (btns_hidden)
    {
        for (i = 0; i < 5; i++)
            if (!btns[i]->isHidden())
            {
                btns[i++]->hide();
                if (i == 5)
                    return;
                ani.setTargetObject(btns[i]);
                ani.setStartValue(btns_pos[i]);
                ani.setEndValue(center);
                ani.start();
                return;
            }
    }
    else
    {
        for (i = 4; i >= 0; i--)
            if (btns[i]->isHidden())
            {
                btns[i]->show();
                ani.setTargetObject(btns[i]);
                ani.setEndValue(btns_pos[i]);
                ani.setStartValue(center);
                ani.start();
                return;
            }
    }
}