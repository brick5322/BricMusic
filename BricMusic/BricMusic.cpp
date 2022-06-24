#include "BricMusic.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QIcon>
#ifdef _DEBUG
#include <QDebug>
#endif
BricMusic::BricMusic(Controller& ctrler,const QColor& color, QWidget* parent)
	: QWidget(parent),ctrler(ctrler),
	vol_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
	hide_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
	prev_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
	next_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
	mode_btn(*new BPrettyButton(color, QColor(color.red(), color.green(), color.blue(), 0), color, this)),
	ani(nullptr),
	albumslider(new BAlbumSlider(BAlbumSlider::loadSVG(":/img/handle-press.tsvg", 20, 20, QColor("#E8E8E8")), BAlbumSlider::loadSVG(":/img/handle-press.tsvg", 20, 20, color), 50, 500, this)),
	btns_hidden(true),is_lrc_on(false),is_vol_on(true),is_playing(true), ani_current_btn(-1),
	mode(Controller::loopPlayBack),vol(127)
{
	btns[0] = &prev_btn;
	btns[1] = &hide_btn;
	btns[2] = &vol_btn;
	btns[3] = &mode_btn;
	btns[4] = &next_btn;

	resize(275, 275);

	albumslider->move(100, 100);
	albumslider->setFrontPen(color, 3);
	albumslider->setBackPen(Qt::white, 3);
	
	albumslider->installEventFilter(this);

	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_QuitOnClose);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	setFocusPolicy(Qt::StrongFocus);
	setWindowIcon(QIcon(":/img/icon.png"));

	hideBtn_delay.setInterval(500);
	hideBtn_delay.setSingleShot(true);

	QPoint center = albumslider->mapToParent(QPoint(25, 25));

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
	vol_btn.setTSVGpic(":/img/vol.tsvg");
	hide_btn.setTSVGpic(":/img/vision-disable.tsvg");
	prev_btn.setTSVGpic(":/img/prev.tsvg");
	next_btn.setTSVGpic(":/img/next.tsvg");
	mode_btn.setTSVGpic(":/img/loopplayback.tsvg");
	ani.setDuration(100);
	ani.setPropertyName("pos");

	QObject::connect(&mode_btn, &BPrettyButton::clicked, this, &BricMusic::on_mode_btn_clicked);
	QObject::connect(&hide_btn, &BPrettyButton::clicked, this, &BricMusic::on_hide_btn_clicked);
	QObject::connect(&vol_btn, &BPrettyButton::clicked, this, &BricMusic::on_vol_btn_clicked);
	QObject::connect(&prev_btn, &BPrettyButton::clicked, this, &BricMusic::on_prev_btn_clicked);
	QObject::connect(&next_btn, &BPrettyButton::clicked, this, &BricMusic::on_next_btn_clicked);
	QObject::connect(&ani, &QAbstractAnimation::finished, this, &BricMusic::on_ani_finished);
	QObject::connect(albumslider, &BAlbumSlider::clicked, this, &BricMusic::on_albumslider_clicked);

	QObject::connect(this, &BricMusic::setPic, albumslider, &BAlbumSlider::setAlbumPic);
	QObject::connect(&hideBtn_delay, &QTimer::timeout, [&]() 
		{	
			btns_hidden = true;
			if (ani.state() != QAbstractAnimation::Running)
				on_ani_finished(); 
		});

	ctrler.setMode(mode);

	QObject::connect(&ctrler, &Controller::timestampChanged, albumslider, &BAlbumSlider::setValue);
	QObject::connect(&ctrler, &Controller::setDuration, albumslider, &BAlbumSlider::setMaximum);
	QObject::connect(&ctrler, &Controller::paused, albumslider, &BAlbumSlider::pauseRotate);

	QObject::connect(&ctrler, &Controller::playTaskReady, this, &BricMusic::on_playtask_ready);
	QObject::connect(&ctrler, &Controller::playTaskFinished, this, &BricMusic::on_playtask_finished);

	QObject::connect(albumslider, &BAlbumSlider::sliderMoved, &ctrler, &Controller::posChange);
}

bool BricMusic::eventFilter(QObject* obj, QEvent* e)
{
	static QPoint pos;
	static bool presslock = true;
	if (e->type() == QEvent::MouseButtonPress)
	{
		presslock = false;
		pos = this->pos() - mapToGlobal(static_cast<QMouseEvent*>(e)->pos());
	}
	else if (e->type() == QEvent::MouseMove)
	{
		if (presslock)
			return QWidget::eventFilter(obj, e);
		albumslider->clickInterupt();
		this->move(pos + mapToGlobal(static_cast<QMouseEvent*>(e)->pos()));
		return true;
	}
	else if (e->type() == QEvent::MouseButtonRelease)
	{
		this->move(pos + mapToGlobal(static_cast<QMouseEvent*>(e)->pos()));
		presslock = true;
	}

	return QWidget::eventFilter(obj, e);
}

void BricMusic::enterEvent(QEvent*)
{
#ifdef _DEBUG
	qDebug() << "enterEvent";
#endif // _DEBUG
	hideBtn_delay.stop();
	this->activateWindow();
	this->raise();
	btns_hidden = false;
	if (ani.state() != QAbstractAnimation::Running)
		on_ani_finished();
}

void BricMusic::leaveEvent(QEvent*)
{
#ifdef _DEBUG
	qDebug() << "leaveEvent";
#endif // _DEBUG
	if (ani.state() == QAbstractAnimation::Running)
	{
		btns_hidden = true;
		hideBtn_delay.stop();
	}
	else
		hideBtn_delay.start();
}

void BricMusic::wheelEvent(QWheelEvent* e)
{
	vol += e->delta()>>4;
	if (vol < 0)
		vol = 0;
	else if (vol & 0xff80)
		vol = 127;
		if(!vol)
			vol_btn.setTSVGpic(":/img/non-vol.tsvg");
		else if (vol < 128 / 3)
			vol_btn.setTSVGpic(":/img/vol1.tsvg");
		else if (vol < 256 / 3)
			vol_btn.setTSVGpic(":/img/vol2.tsvg");
		else
			vol_btn.setTSVGpic(":/img/vol.tsvg");
		emit setVolume(vol);
}

void BricMusic::closeEvent(QCloseEvent* e)
{
	ctrler.setMode(Controller::quit);
	ctrler.playTaskStop();
	e->accept();
}

void BricMusic::on_vol_btn_clicked()
{
	is_vol_on = !is_vol_on;
	if (is_vol_on&&vol)
	{
		if (vol < 128 / 3)
			vol_btn.setTSVGpic(":/img/vol1.tsvg");
		else if (vol < 256 / 3)
			vol_btn.setTSVGpic(":/img/vol2.tsvg");
		else
			vol_btn.setTSVGpic(":/img/vol.tsvg");
		emit setVolume(vol);
	}
	else
	{
		vol_btn.setTSVGpic(":/img/non-vol.tsvg");
		emit setVolume(0);
	}
}

void BricMusic::on_hide_btn_clicked()
{
	btns_hidden = true;
	ani.stop();
	ani_current_btn = -1;
	QPoint center = albumslider->mapToParent(QPoint(25, 25));
	for (int i = 0; i < 5; i++)
	{
		btns[i]->move(center);
		btns[i]->hide();
	}
	hide();
}

void BricMusic::on_prev_btn_clicked()
{
	ctrler.getPrevAudio();
}

void BricMusic::on_next_btn_clicked()
{
	ctrler.getNextAudio();
}

void BricMusic::on_mode_btn_clicked()
{
	switch (mode)
	{
	case Controller::loopPlayBack:
		mode = Controller::singleTune;
		mode_btn.setTSVGpic(":/img/singletune.tsvg"); 
		break;
	case Controller::singleTune:
		mode = Controller::randomTune;
		mode_btn.setTSVGpic(":/img/randomtune.tsvg");
		break;
	case Controller::randomTune:
		mode = Controller::loopPlayBack;
		mode_btn.setTSVGpic(":/img/loopplayback.tsvg");
		break;
	default:
		break;
	}
	ctrler.setMode(mode);
}

void BricMusic::on_albumslider_clicked()
{
	if (ctrler.isFinishing())
		return;
	is_playing = !is_playing;
	if (is_playing)
	{
		albumslider->startRotate();
		ctrler.setAction(Controller::playing);
		emit ctrler.setPlaying();
	}
	else
	{
		ctrler.setAction(Controller::pausingAudio);
		emit ctrler.setPausing();
	}
}

void BricMusic::on_playtask_ready()
{
	is_playing = true;
	albumslider->startRotate();
	ctrler.playTaskStart();
}

void BricMusic::on_playtask_finished()
{
	is_playing = false;
	ctrler.playTaskInit();
}

void BricMusic::on_ani_finished()
{
	QPoint center = albumslider->mapToParent(QPoint(25, 25));
	if (ani_current_btn < 0)
		if (btns_hidden)
		{
			for (int j = 0; j < 5; j++)
				btns[j]->hide();
			return;
		}
		else
		{
			ani_current_btn = 0;
			for (int j = 0; j < 5; j++)
				btns[j]->show();
		}
	else if (ani_current_btn == 5)
		if (btns_hidden)
			ani_current_btn--;
		else
			return;
	ani.setTargetObject(btns[ani_current_btn]);

	if (btns_hidden)
	{
		ani.setStartValue(btns_pos[ani_current_btn--]);
		ani.setEndValue(center);
	}
	else
	{
		ani.setStartValue(center);
		ani.setEndValue(btns_pos[ani_current_btn++]);
	}
	ani.start();
}
