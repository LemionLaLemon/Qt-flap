#pragma once
#include <array>
#include <functional>
#include <vector>
#include <QtGlobal>

namespace flap {
    struct Observation {
        bool pipeAhead = false;
        qreal birdY = 0;
        qreal birdVy = 0;
        qreal dxLeft = 0; // delt x pipe birdR to pipeL
        qreal dxRight = 0; // delt x birdL to pipeR
        qreal gapTop = 0;
        qreal gapBottom = 0;
        qreal worldWidth = 0; // in case hyprland tries to pull more bs and not float the window anymore
        qreal worldHeight = 0; 
        qreal gapCenter() const {
            return (gapTop + gapBottom) / 2.0;
        }
    };

    inline std::array<float, 4> inputs(const Observation &o){
        return {
            float(o.birdY / o.worldHeight),
            float(o.birdVy / 600.0),
            float ((o.gapCenter() - o.birdY) / o.worldHeight),
            float(o.dxRight / o.worldWidth),
        };
    }

    struct BirdResult { // I LOVE PLANNING AHEAD!!
        int index = 0;
        qreal age = 0;
        int pipesPassed = 0;
    };

    using Brain = std::function<bool(int birdIndex, const Observation &)>;
    using GameOverHandler = std::function<void(const std::vector<BirdResult> &)>;

    struct Hooks{
        Brain brain;
        GameOverHandler onGameOver;
        bool autoRestart = false;
    };
}