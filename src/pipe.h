#pragma once
#include <QRect>
#include <functional>

class QWidget;

using PipeBuilder = std::function<QWidget*(QWidget *parent)>;

class Pipe{
    public:
        static constexpr int width = 70;
        static constexpr int gap = 160;

        Pipe(QWidget *parent, qreal x, int gapTop);
        ~Pipe();

        static const PipeBuilder &returnRandomPipe();

        void advance(qreal dx, int parentH);
        bool offscreen() const;
        qreal right() const;
        bool hits(const QRect &r) const;
        bool scored = false;
    private:
        void sync(int parentH);

        QWidget *top;
        QWidget *bottom;
        qreal x;
        int gapTop;
};