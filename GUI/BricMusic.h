#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>
#include "BPrettyButton.h"
#include "BAlbumSlider.h"

class BricMusic : public QWidget
{
    Q_OBJECT

public:
    BricMusic(const QColor& color,QWidget *parent = Q_NULLPTR);

private:
    BPrettyButton& vol_btn;
    BPrettyButton& lrc_btn;
    BPrettyButton& pre_btn;
    BPrettyButton& next_btn;
    BPrettyButton& mode_btn;
    BPrettyButton* btns[5];
    QPoint btns_pos[5];

    BAlbumSlider* albumslider;
};
