#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>
#include <QTimer>
#include <QPropertyAnimation>
#include "BPrettyButton.h"
#include "BAlbumSlider.h"
#include "Controller.h"

class BricMusic : public QWidget
{
    Q_OBJECT

public:
    BricMusic(const QColor& color,QWidget *parent = Q_NULLPTR);
    ~BricMusic() {}
    Controller& controller();
protected:
    bool eventFilter(QObject* obj, QEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void wheelEvent(QWheelEvent*);
    void closeEvent(QCloseEvent*);
signals:
    void setVolume(int vol);
    void setPic(uchar* picdata, int size);
private slots:
    void on_ani_finished();
    void on_vol_btn_clicked();
    void on_hide_btn_clicked();
    void on_prev_btn_clicked();
    void on_next_btn_clicked();
    void on_mode_btn_clicked();
public slots:
    void on_playtask_ready();
    void on_playtask_finished();
    void on_albumslider_clicked();

private:
    Controller ctrler;

    BPrettyButton& vol_btn;
    BPrettyButton& hide_btn;
    BPrettyButton& prev_btn;
    BPrettyButton& next_btn;
    BPrettyButton& mode_btn;
    BPrettyButton* btns[5];
    QPoint btns_pos[5];
    QTimer hideBtn_delay;
    int ani_current_btn;
    bool btns_hidden;

    BAlbumSlider* albumslider;
    QPropertyAnimation ani;

    bool is_vol_on;
    bool is_lrc_on;
    bool is_playing;
    int vol;
    int timestamp;
    int time;

    Controller::PlayBackMode mode;
};
