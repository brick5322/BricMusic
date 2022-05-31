#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>
#include <QPropertyAnimation>
#include "BPrettyButton.h"
#include "BAlbumSlider.h"
#include "AudioFileManager.h"
#include "Controller.h"

class BricMusic : public QWidget
{
    Q_OBJECT

public:
    BricMusic(const QColor& color,QWidget *parent = Q_NULLPTR);
    Controller& controller();
protected:
    bool eventFilter(QObject* obj, QEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void wheelEvent(QWheelEvent*);
    void focusOutEvent(QFocusEvent*);
signals:
    void setVolume(int vol);
private slots:
    void on_ani_finished();
    void on_vol_btn_clicked();
    void on_lrc_btn_clicked();
    void on_pre_btn_clicked();
    void on_next_btn_clicked();
    void on_mode_btn_clicked();
    void on_albumslider_clicked();
private:
    Controller ctrler;
    BPrettyButton& vol_btn;
    BPrettyButton& lrc_btn;
    BPrettyButton& pre_btn;
    BPrettyButton& next_btn;
    BPrettyButton& mode_btn;
    BPrettyButton* btns[5];
    QPoint btns_pos[5];
    bool btns_hidden;

    BAlbumSlider* albumslider;
    QPropertyAnimation ani;

    bool is_vol_on;
    bool is_lrc_on;
    int vol;
    int timestamp;
    int time;
    AudioFileManager::PlayBackMode mode;
};
