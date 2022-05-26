#include "BricMusic.h"

BricMusic::BricMusic(const QColor& color, QWidget* parent)
    : QWidget(parent),
    vol_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    lrc_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    pre_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    next_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
    mode_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this))
{
    btns[0] = &vol_btn;
    btns[1] = &lrc_btn;
    btns[2] = &pre_btn;
    btns[3] = &next_btn;
    btns[4] = &mode_btn;

    albumslider = new BAlbumSlider(BAlbumSlider::loadSVG("./img/handle-press.tsvg", 20, 20, QColor("#E8E8E8")), BAlbumSlider::loadSVG("./img/handle-press.tsvg", 20, 20, color), 50, 500, this);
    albumslider->move(100, 100);
    QPoint center = albumslider->mapToParent(albumslider->center());
    btns_pos[0] = (center + QPointF(-1, 0) * 50 * 1.5).toPoint();
    btns_pos[1] = (center + QPointF(-0.707, -0.707) * 50 * 1.5).toPoint();
    btns_pos[2] = (center + QPointF(0, -1) * 50 * 1.5).toPoint();
    btns_pos[3] = (center + QPointF(0.707, -0.707) * 50 * 1.5).toPoint();
    btns_pos[4] = (center + QPointF(1, 0) * 50 * 1.5).toPoint();

    for (int i = 0; i < 5; i++)
        btns[i]->move(btns_pos[i]);
}
