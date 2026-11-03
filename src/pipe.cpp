#include "pipe.h"
#include <qhashfunctions.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <QRandomGenerator>

const PipeBuilder &Pipe::returnRandomPipe(){
    static const QList<PipeBuilder> variants = {
        [](QWidget *p) -> QWidget* {
            auto *w = new QPushButton(p);
            w->setStyleSheet("background-color: #3a9d3a; border: 2px solid #1e5e1e;");
            return w;
        }
        //todo: add more pipe types
    };
    return variants.at(QRandomGenerator::global()->bounded(int(variants.size())));
}

Pipe::Pipe(QWidget *parent, qreal x, int gapTop) :x(x), gapTop(gapTop){
    const PipeBuilder &build = returnRandomPipe();
    top = build(parent);
    bottom = build(parent);

    for (QWidget *w : {top, bottom}){
        w->setMinimumSize(0, 0);
        w->setFocusPolicy(Qt::NoFocus);
        w->setAttribute(Qt::WA_TransparentForMouseEvents);
        w->show();
    }
    sync(parent->height());
}

Pipe::~Pipe(){
    top->deleteLater();
    bottom->deleteLater();
}

void Pipe::advance(qreal dx, int parentH){
    x -= dx;
    sync(parentH);
}

bool Pipe::offscreen() const {
    return x + width < 0;
}
qreal Pipe::right() const {
    return x + width;
}

qreal Pipe::left() const{
    return x;
}

int Pipe::gapTopY() const {
    return gapTop;
}

int Pipe::gapBottomY() const {
    return gapTop + gap;
}

bool Pipe::hits(const QRect &r) const {
    return top->geometry().intersects(r) || bottom->geometry().intersects(r);
}

void Pipe::sync(int parentH){
    top->setGeometry(int(x), 0, width, gapTop);
    bottom->setGeometry(int(x), gapTop + gap, width, qMax(0, parentH - gapTop - gap));
}