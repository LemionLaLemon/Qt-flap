#include "bird.h"
#include <QFrame>
#include <QPushButton>
#include <qframe.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <QLabel>
#include <QRadioButton>
#include <QPainter>
#include <qradiobutton.h>

qreal Bird::floorY() const{
    return parentWidget()->height() - boxSize + (boxSize - 40) / 2.0;
}

qreal Bird::ceilingY() const{
    return -(boxSize-40) / 2.0;
}

void Bird::reset(){
    y = initialBirdY;
    vy = 0;
    angle = 0;
    move(140, int(y));
    update();
}

QRect Bird::hitbox() const {
    QRect r(0, 0, 40, 40); //size of bird or die
    r.moveCenter(geometry().center());
    return r;
}

Bird::Bird(QWidget *parent) : QWidget(parent){
    resize(boxSize, boxSize);

    QWidget *s = new QWidget(this);
    s->setObjectName("sprite");
    s->setStyleSheet(QStringLiteral("QWidget#sprite{background: transparent;}"));
    s->setGeometry(0, 0, 52, 40);
    
    QPushButton *body = new QPushButton(s);
    body->resize(40, 40);
    body->setFocusPolicy(Qt::NoFocus);
    body->setStyleSheet(QStringLiteral("background-color: #276F69"));

    QLabel *beak = new QLabel(s);
    beak->setText(QStringLiteral("beak"));
    beak->setStyleSheet(QStringLiteral("color: #000000; background-color: #00000000"));
    beak->move(30, 10);
    beak->setFocusPolicy(Qt::NoFocus);

    QRadioButton *eye = new QRadioButton(s);
    eye->move(35, 5);
    eye->setStyleSheet(QStringLiteral("background: #00000000; color: #000000;"));
    eye->setChecked(true);
    eye->setFocusPolicy(Qt::NoFocus);
    
    sprite = s->grab();
    delete s;

    move(140, int(y));
}

void Bird::flap(){
    vy = -380;
}

void Bird::step(qreal dt){
    vy += 1200 * dt;
    y += vy * dt;

    angle = qBound(-30.0, vy * 0.1, 90.0);

    if (y < ceilingY()){
        y = ceilingY();
        vy = 0;
    }
    if (y > floorY()){
        y = floorY();
        vy = 0;
    }
    
    move(140, int(y));
    update();
}

bool Bird::onGround() const{
    return y >= floorY() - 0.5;
}

void Bird::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.translate(width() / 2.0, height() / 2.0);
    p.rotate(angle);
    const QSizeF sz = sprite.deviceIndependentSize();
    p.drawPixmap(QPointF(-sz.width()/2, -sz.height()/2), sprite);
}