#pragma once
#include <QPixmap>
#include <QWidget>

class Bird : public QWidget {
    public:
        explicit Bird(QWidget *parent = nullptr);

        void flap();
        void step(qreal dt);

        QRect hitbox() const;
        void reset();

        bool onGround() const;
    protected:
        void paintEvent(QPaintEvent*) override;
    private:
        static constexpr int boxSize = 100;
        static constexpr qreal initialBirdY = 200;
        qreal y = initialBirdY;
        qreal vy = 0; 
        qreal angle = 0;
        QPixmap sprite;

        qreal floorY() const;
        qreal ceilingY() const;
};
