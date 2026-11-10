#include <QApplication>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QMainWindow>
#include <QWidget>
#include "bird.h"
#include <QMenuBar>
#include <QMenu>
#include <array>
#include <cstdlib>
#include <qcoreapplication.h>
#include <QStatusBar>
#include <QStyle>
#include <QStyleHints>
#include <QFile>
#include <qnamespace.h>
#include <QRandomGenerator>
#include "pipe.h"
#include <QList>
#include <qrandom.h>
#include "api.h"

constexpr std::string windowTitle = "Qt-flap";

class Game : public QWidget {
    public:
        explicit Game(QStatusBar *status, int birdCount = 1) : status(status){
            setFocusPolicy(Qt::StrongFocus);
            setStyleSheet("Game {background-color: #ffffff;}");
            
            for (int i = 0; i < birdCount; i++){
                birds.append(new Bird(this));
            }
            aliveCount = birdCount;

            clock.start();

            auto *t = new QTimer(this);
            connect(t, &QTimer::timeout, this, [this]{
                qreal dt = qMin(clock.restart() / 1000.0, 0.05);
                if (dead){
                    return;
                }

                acc += dt * speed;
                for (int i = 0; acc >= fixedDt && i < 2000 && !dead; i++){
                    acc -= fixedDt;
                    simStep(fixedDt);
                }
            });
            t->setTimerType(Qt::PreciseTimer);
            t->start(16);
        }
        void restart(){
            qDeleteAll(pipes);
            pipes.clear();
            spawnIn = 1.0;
            score = 0;
            dead = false;
            for (Bird *b : birds){
                b->reset();
            }
            aliveCount = birds.size();
            acc = 0;
            clock.restart();
            status->showMessage(QStringLiteral("Press space to flap"));
            setFocus();
        }
        void simStep(qreal dt){
            if (hooks.brain){
                for (int i = 0; i < birds.size(); i++){
                    Bird *b = birds[i];
                    if (b->alive && hooks.brain(i, observe(b))){
                        b->flap();
                    }
                }
            }

            for (Bird *b : birds){
                if (b->alive){
                    b->step(dt);
                    b->age += dt;
                }
            }
            
            updatePipes(dt);

            for (Bird *b : birds){
                if (b->alive && (b->onGround() || hitPipe(b))){
                    b->pipesPassed = score;
                   b->kill();
                   --aliveCount; 
                }
            }
            if (aliveCount == 0){
                die();
            }
        }
        void setHooks(flap::Hooks h){
            hooks = std::move(h);
        }
        void setSpeed(int s){
            speed = qBound(1, s, 64);
        }
        void setSeed(quint32 s){
            rng.seed(s);
        }
    protected:
        void keyPressEvent(QKeyEvent *e) override{
            switch (e->key()){
                case Qt::Key_Space:
                    if (dead){
                        restart();
                    }
                    else {
                        for (Bird *b : birds) {
                            if (b->alive){
                                b->flap();
                            }
                        }
                    }
                    break;
                case Qt::Key_Plus: case Qt::Key_Equal:
                    speed = qMin(speed * 2, 64);
                    break;
                case Qt::Key_Minus:
                    speed = qMax(speed / 2, 1);
                    break;
            }
        }
    private:
        QList<Bird*> birds;
        int aliveCount = 0;
        QStatusBar *status;
        bool dead = false;
        QElapsedTimer clock;

        QList<Pipe*> pipes;
        int score = 0;
        qreal spawnIn = 1.0;

        flap::Hooks hooks;
        QRandomGenerator rng = QRandomGenerator::securelySeeded();

        static constexpr qreal pipeSpeed = 180;
        static constexpr qreal spawnInterval = 1.6;

        static constexpr qreal fixedDt = 1.0 / 60;
        qreal acc = 0;
        int speed = 1;

        int lastGapTop = -1;
        static constexpr int maxGapShift = 150;

        void die(){
            dead = true;
            status->showMessage(QStringLiteral("Score: %1, Press flap to space again").arg(score));
        
            if (hooks.onGameOver){
                std::vector<flap::BirdResult> results;
                for (int i=0; i<birds.size(); i++){
                    results.push_back({i, birds[i]->age, birds[i]->pipesPassed});
                }
                hooks.onGameOver(results);
            }
            if (hooks.autoRestart){
                restart();
            }
        }

        void updatePipes(qreal dt){
            spawnIn -= dt;
            if (spawnIn <= 0){
                spawnIn = spawnInterval * (0.9 + QRandomGenerator::global()->generateDouble() * 0.4);
                const int margin = 50;
                int lo = margin;
                int hi = qMax(margin + 1, height() - Pipe::gap - margin);
                if (lastGapTop >= 0){
                    lo = qMax(lo, lastGapTop - maxGapShift);
                    hi = qMin(hi, lastGapTop + maxGapShift + 1);
                }
                const int gapTop = QRandomGenerator::global()->bounded(lo, qMax(lo + 1, hi));
                lastGapTop = gapTop;
                pipes.append(new Pipe(this, width(), gapTop));
                for (Bird *b : birds){
                    b->raise();
                }
            }
            
            const int birdLeft = birds.first()->hitbox().left();
            for (Pipe *p : pipes){
                p->advance(pipeSpeed * dt, height());
                if (!p->scored && p->right() < birdLeft){
                    p->scored = true;
                    ++score;
                    status->showMessage(QStringLiteral("Score: %1").arg(score));
                }
            }
            while (!pipes.isEmpty() && pipes.first()->offscreen()){
                delete pipes.takeFirst();
            }
        }

        bool hitPipe(const Bird *b) const{
            const QRect hb = b->hitbox().adjusted(4, 4, -4, -4);
            for (Pipe *p : pipes){
                if (p->hits(hb)) {
                    return true;
                }
            }
            return false;
        }
        flap::Observation observe(const Bird *b) const{
            flap::Observation o;
            const QRect hb = b->hitbox();
            o.worldWidth = width();
            o.worldHeight = height();
            o.birdY = hb.center().y();
            o.birdVy = b->velocity();
            o.gapTop = height() / 2.0 - Pipe::gap / 2.0;
            o.gapBottom = o.gapTop + Pipe::gap;
            o.dxLeft = width();
            o.dxRight = width();

            for (Pipe *p : pipes){
                if (p->right() > hb.left()){
                    o.pipeAhead = true;
                    o.dxLeft = p->left() - hb.right();
                    o.dxRight = p->right() - hb.left();
                    o.gapTop = p->gapTopY();
                    o.gapBottom = p->gapBottomY();
                    break;
                }
            }
            return o;
        }

};

float randomFloat(float min, float max){
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

static constexpr int BIRD_COUNT = 50;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QString::fromUtf8(windowTitle));
    app.setStyle("Windows");
    
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    QPalette palette = app.palette();

    palette.setColor(QPalette::Window, QColor("#dfe8f6"));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor("#edf3fc"));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor("#dfe8f6"));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Highlight, QColor("#316ac5"));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    QFile stylesheet(":/style.qss");
    if (stylesheet.open(QIODevice::ReadOnly)){
        app.setStyleSheet(QString::fromUtf8(stylesheet.readAll()));
    }

    QMainWindow window;
    window.setWindowTitle(QString::fromUtf8(windowTitle));
    window.setFixedSize(800, 600);

    Game *game = new Game(window.statusBar(), BIRD_COUNT);
    window.setCentralWidget(game);

    QMenu *fileMenu = window.menuBar()->addMenu(QStringLiteral("&File"));
    fileMenu->addAction(QStringLiteral("&Restart"), game, &Game::restart);

    window.statusBar()->showMessage(QStringLiteral("Press space to flap"));

    window.show();
    game->setFocus();

    flap::Hooks h;
    h.autoRestart = true;

    struct genome {
        float hidden[6];
    };

    genome birdBrain[BIRD_COUNT]; // https://www.youtube.com/watch?v=0iVlSNpq8i8

    for (int i = 0; i < BIRD_COUNT; i++){
        for (int j = 0; j < 6; j++){
            birdBrain[i].hidden[j] = randomFloat(-2, 2);
        }
    }

    h.brain = [&birdBrain](int bird, const flap::Observation &o){
        const std::array<float, 4> input = inputs(o);

        float sum = birdBrain[bird].hidden[4];
        
        for (int i = 0; i < 4; i++){
            sum += input[i] * birdBrain[bird].hidden[i];
        }

        return sum > 0.0f;
    };
    h.onGameOver = [&birdBrain](const std::vector<flap::BirdResult> &r){
        qreal best = 0;
        for (const auto &b : r){
            best = qMax(best, b.age);
        }
        qDebug() << "best survived" << best << "s";
    };
    game->setHooks(h);
    game->setSpeed(8);

    return app.exec();
}
