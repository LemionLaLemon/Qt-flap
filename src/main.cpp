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

constexpr std::string windowTitle = "Qt-flap";

class Game : public QWidget {
    public:
        explicit Game(QStatusBar *status) : status(status){
            setFocusPolicy(Qt::StrongFocus);
            setStyleSheet("Game {background-color: #ffffff;}");
            bird = new Bird(this);
            clock.start();

            auto *t = new QTimer(this);
            connect(t, &QTimer::timeout, this, [this]{
                qreal dt = clock.restart() / 1000.0;
                if (dead) {
                    return;
                }

                bird->step(dt);
                updatePipes(dt);
                if (bird->onGround() || hitPipe()){
                    die();
                }
            });
            // for (QWidget *pipe : pipes){
            //     if (bird->hitbox().intersects(pipe->geometry())){
            //         //bird kaputs
            //     }
            // }
            t->start(16);
        }
        void restart(){
            qDeleteAll(pipes);
            pipes.clear();
            spawnIn = 1.0;
            score = 0;
            dead = false;
            bird->reset();
            clock.restart();
            status->showMessage(QStringLiteral("Press space to flap"));
            setFocus();
        }
    protected:
        void keyPressEvent(QKeyEvent *e) override{
            if (dead){
                if (e->key() == Qt::Key_Space){
                    restart();
                    return;
                }
            }
            if (e->key() == Qt::Key_Space){
                bird->flap();
            }
        }
    private:
        Bird *bird;
        QStatusBar *status;
        bool dead = false;
        QElapsedTimer clock;

        QList<Pipe*> pipes;
        int score = 0;
        qreal spawnIn = 1.0;

        static constexpr qreal pipeSpeed = 180;
        static constexpr qreal spawnInterval = 1.6;

        void die(){
            dead = true;
            status->showMessage(QStringLiteral("Score: %1, Press flap to space again").arg(score));
        }

        void updatePipes(qreal dt){
            spawnIn -= dt;
            if (spawnIn <= 0){
                spawnIn = spawnInterval;
                const int margin = 50;
                const int gapTop = QRandomGenerator::global()->bounded(margin, qMax(margin + 1, height() - Pipe::gap - margin));
                pipes.append(new Pipe(this, width(), gapTop));
                bird->raise();
            }
            
            const int birdLeft = bird->hitbox().left();
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

        bool hitPipe() const{
            const QRect hb = bird->hitbox().adjusted(4, 4, -4, -4);
            for (Pipe *p : pipes){
                if (p->hits(hb)) {
                    return true;
                }
            }
            return false;
        }

};

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

    Game *game = new Game(window.statusBar());
    window.setCentralWidget(game);

    QMenu *fileMenu = window.menuBar()->addMenu(QStringLiteral("&File"));
    fileMenu->addAction(QStringLiteral("&Restart"), game, &Game::restart);

    window.statusBar()->showMessage(QStringLiteral("Press space to flap"));

    window.show();
    game->setFocus();

    return app.exec();
}
