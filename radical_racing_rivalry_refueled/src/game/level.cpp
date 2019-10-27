// Copyright 2018 Catalin G. Manciu

#include "level.h"
#include "car.h"
#include "enemycar.h"
#include "animatedgameobject.h"
#include "../res/stringmgr.h"
#include "../engine/renderer.h"
#include "../res/sprites.h"
#include "../game.h"

Level::BackgroundLayer* Level::bgLayers[] = {
    new BackgroundGradient(0, 28, 5, 9, 7, 26, 20, 7),
    new BackgroundSprite(28, 0, Defs::BackgroundSun, 0),
    new BackgroundSprite(28, 150, Defs::BackgroundLayer1, 10),
    new BackgroundSprite(28, 200, Defs::BackgroundLayer2, 40),
    new BackgroundSprite(28, 220, Defs::BackgroundLayer3, 75),
    new BackgroundGrid(28, 40, 8, 80),
    new BackgroundLineScroll(41, 0, 250),
    new BackgroundSprite(42, 240, Defs::BackgroundLayer4, 250),
    new BackgroundBlur(0, 40)
};

Level::BackgroundBlur* Level::backgroundBlur = static_cast<BackgroundBlur*>(
                                            bgLayers[BlurBackgroundIndex]);

int16_t Level::BackgroundLayer::camPosToOffset(const FP32& cameraPosition) {
    FP32 camPosScaled = (cameraPosition * offsetFactor) / 1000;
    return -(Utils::metersToPixels(camPosScaled).getInt());
}


void Level::BackgroundGrid::drawSingleLine(SpriteRenderer* renderer,
                                           int16_t x, int16_t yTop,
                                           int16_t yBot) {
    FP32 lineX(x);
    FP32 lineXBottom = lineX * FP32(3.0f);
    Line current(lineX + Defs::FPHalfScrW, FP32(yTop),
                 lineXBottom + Defs::FPHalfScrW, FP32(yBot));
    renderer->getClip().clipLineX(&current);
    if (current.exists()) {
        renderer->drawLine(current.start.x.getInt(),
                           current.start.y.getInt(),
                           current.end.x.getInt(),
                           current.end.y.getInt(),
                           color);
    }
}

void Level::BackgroundGrid::draw(SpriteRenderer* renderer,
                                 const FP32& cameraPosition) {
    int16_t offset = camPosToOffset(cameraPosition);
    offset %= density;
    for (int16_t line_x_top = offset; line_x_top < (Defs::ScreenW / 2);
         line_x_top += density) {
        drawSingleLine(renderer, line_x_top, yTop, yBot);
    }
    for (int16_t line_x_top = offset - density;
         line_x_top > -(Defs::ScreenW / 2); line_x_top -= density) {
        drawSingleLine(renderer, line_x_top, yTop, yBot);
    }
    int8_t mid = ((yTop + yBot) >> 1);
    int8_t values[4] = { yTop, yBot, mid, ((yTop + mid) >> 1) };
    for (auto y : values) {
        renderer->drawLine(0, y, Defs::ScreenW - 1, y, color);
    }
}


void Level::BackgroundSprite::draw(SpriteRenderer* renderer,
                                   const FP32& cameraPosition) {
    int16_t offset = (Defs::ScreenW / 2);
    if (offsetFactor != 0) {
        int16_t maxOffset = Defs::ScreenW + (width >> 1);
        offset += (camPosToOffset(cameraPosition) % width);
        while (offset > (width >> 1)) {
            offset -= width;
        }
        do {
            GetSprite(Defs::SpriteEnv)->drawAnimationFrame(
                                            renderer, Defs::AnimBackgrounds,
                                            frame, offset, yPos, 0);
            offset += width;
        } while (offset < maxOffset);
    } else {
            GetSprite(Defs::SpriteEnv)->drawAnimationFrame(
                                            renderer, Defs::AnimBackgrounds,
                                            frame, offset, yPos, 0);
    }
}


void Level::BackgroundLineScroll::draw(SpriteRenderer* renderer,
                                       const FP32& cameraPosition) {
    int8_t height = GetSprite(Defs::SpriteEnv)->getElementH(element);
    int8_t width = GetSprite(Defs::SpriteEnv)->getElementW(element);
    int32_t factor = 1000;
    int32_t camPosOffset = camPosToOffset(cameraPosition);
    for (int8_t line = yPos; line < yPos + height; ++line) {
        int32_t crtOffset = ((camPosOffset* factor) / 1000) % width;
        GetSprite(Defs::SpriteEnv)->fillSingleLine(renderer, element,
                                                   crtOffset +
                                                   Defs::ScreenW / 2, line,
                                                   line - yPos);
        factor += 50;
    }
}


void Level::BackgroundBlur::draw(SpriteRenderer* renderer,
                                 const FP32& cameraPosition) {
    if (enabled) {
        renderer->setClip(0, yTop, Defs::ScreenW, height);
        renderer->reasonablyFastBlur();
        renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
    }
}


void Level::BackgroundGradient::draw(SpriteRenderer* renderer,
                                     const FP32& cameraPosition) {
    renderer->setClip(0, yTop, Defs::ScreenW, height);
    renderer->drawVerticalGradient(rTop, gTop, bTop, rBot, gBot, bBot);
    renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
}

void Level::initialize() {
    int16_t carMinX = GET_MINX_FROM_PACK(
                GetSprite(Defs::SpriteCar)->measureAnimationFrame<true>(
                                                    Defs::AnimCarChassis, 0));
    int16_t carMaxX = GET_MAXX_FROM_PACK(
                GetSprite(Defs::SpriteCar)->measureAnimationFrame<true>(
                                                    Defs::AnimCarLights, 0));
    entityRepo[EntityInstance::PlayerCar] = new Car(this, carMinX, carMaxX);
    for (uint8_t idx = EntityInstance::EnemyCar1;
         idx <= EntityInstance::EnemyCar2; ++idx) {
        entityRepo[idx] = new EnemyCar(this, carMinX, carMaxX);
    }
    int32_t barrelBounds =
        GetSprite(Defs::SpriteEnv)->measureAnimationFrame<true>(
                                                    Defs::AnimBarrel, 0);
    for (uint8_t idx = EntityInstance::Barrel1;
         idx <= EntityInstance::Barrel2; ++idx) {
        entityRepo[idx] = new AnimatedGameObject(
                                    this,
                                    GET_MINX_FROM_PACK(barrelBounds),
                                    GET_MAXX_FROM_PACK(barrelBounds),
                                    Defs::SpriteEnv, Defs::AnimBarrel);
    }
    state = Invalid;
}

void Level::restart() {
    nbActiveEntities = 0;
    activeEntities[nbActiveEntities] = entityRepo[EntityInstance::Barrel1];
    activeEntities[nbActiveEntities]->reset(FP32(0), FP32(0));
    barrelOne = nbActiveEntities++;

    if (getGameMode() != TimeAttack) {
        activeEntities[nbActiveEntities] =
                    entityRepo[EntityInstance::EnemyCar1];
        enemyCar = static_cast<Car*>(activeEntities[nbActiveEntities++]);
        enemyCar->setChassis(rivalChassis);
        enemyCar->reset(FP32(0), FP32(0.3f));
    }

    activeEntities[nbActiveEntities] = entityRepo[getGameMode() != DuelDemo ?
                                                    EntityInstance::PlayerCar:
                                                    EntityInstance::EnemyCar2];
    playerCar = static_cast<Car*>(activeEntities[nbActiveEntities++]);
    playerCar->setChassis(mainChassis);
    playerCar->reset(FP32(0), getGameMode() != TimeAttack ?
                     FP32(0.9f) : FP32(0.7f));

    activeEntities[nbActiveEntities] = entityRepo[EntityInstance::Barrel2];
    activeEntities[nbActiveEntities]->reset(FP32(0), FP32(1.0f));
    barrelTwo = nbActiveEntities++;

    levelTimer = 0;
    endResult = NoResult;
    newRecord = false;
    screenAnimType = None;
    mustMoveBarrels = true;
    currentGearShift->reset();
    cameraTarget = playerCar;
    cameraPosition = FP32(-120);
    backgroundBlur->setEnabled(false);
    setState(Countdown);
}

void Level::startScreenAnim(uint8_t x, uint8_t y, ScreenAnimType type,
                            uint8_t sprite, uint8_t anim, bool loop) {
    scrAnimX = x;
    scrAnimY = y;
    screenAnimType = type;
    if (type == Screen_Sprite) {
        screenAnim.init(GetSprite(sprite));
        screenAnim.setAnimation(anim, 0, loop);
    }
}

void Level::setState(LevelState newState) {
    if (newState != state) {
        switch (newState) {
            case Countdown:
                startScreenAnim(Defs::ScreenW / 2, Defs::ScreenH / 2,
                                Screen_Sprite, Defs::SpriteHud,
                                Defs::AnimHUDCountdown);
            break;
            case Result:
                stateCounter = 0;
                if (endResult < RaceEndLose) {
                    startScreenAnim(82, 64,
                                    Screen_Sprite, Defs::SpriteCar,
                                    Defs::AnimCarExplosion_Sm);
                    maxStateCounter = 120;
                } else {
                    if (endResult != RaceEndLose) {
                        checkRecord();
                        if (getGameMode() == Duel) {
                            R3R::getInstance().increaseDuelWins(getGearMode());
                        }
                        R3R::getInstance().saveSave();
                    }
                    startScreenAnim((Defs::ScreenW - Defs::EndFlagW) / 2,
                                    Defs::ResultTextY - (Defs::EndFlagH / 2),
                                    Screen_Flag);
                    maxStateCounter = 240;
                }
                backgroundBlur->setEnabled(true);
            break;
        }
        state = newState;
    }
}

template<typename F>
void Level::foreachGameObject(F func) {
    for (uint8_t idx = 0; idx < nbActiveEntities; ++idx) {
        func(activeEntities[idx]);
    }
}

void Level::draw(SpriteRenderer* renderer) {
    if (state == Invalid) return;
    for (auto layer : bgLayers) {
        layer->draw(renderer, cameraPosition);
    }
    foreachGameObject([&](GameObject* obj) {
                       if (obj->isVisible()) obj->draw(renderer);
                      });

    if (state == Race && getGameMode() == Duel && !enemyCar->isVisible()) {
        drawDistanceToRival(renderer, 96, 32);
    }

    bool drawHud = ((state == Race && getGameMode() != DuelDemo) ||
                    (state == Result && endResult < RaceEndLose &&
                     screenAnim.getCurrentFrame() == 0));
    if (drawHud) {
        drawHUD(renderer);
    }

    switch (screenAnimType) {
        case Screen_Sprite:
            screenAnim.draw(renderer, scrAnimX, scrAnimY); break;
        case Screen_Flag:
            drawEndFlag(renderer, scrAnimX, scrAnimY, Defs::EndFlagW); break;
    }

    if (state == Result) {
        drawResult(renderer, Defs::ScreenW / 2, Defs::ResultTextY);
    }

    if (getGameMode() == DuelDemo &&
        (R3R::getInstance().getFrameCounter() & 63) < 32) {
        GetFont(Defs::FontMain)->drawString(renderer, getString(Strings::Demo),
                                            Defs::ScreenW - 1,
                                            Defs::ScreenH -1,
                                            ANCHOR_BOTTOM| ANCHOR_RIGHT, 2);
    }
}

#define DEBUG_ENEMY_SPEED   (0)

void Level::drawCarHUD(SpriteRenderer* renderer, uint8_t x, uint8_t y) {
    Sprite* hud = GetSprite(Defs::SpriteHud);
// Draw RPM Bar background
    hud->drawAnimationFrame(renderer, Defs::AnimHUDRPM,
                            Defs::HUDFrameRPM, x, y, 0);
// Draw speed
    static constexpr uint8_t kNbDigitsSpeed = 3;
    uint8_t crtX = x - 1;
    int32_t speed = Utils::mpsToKph(playerCar->getSpeed()).getInt();
    for (uint8_t digit = 0; digit < kNbDigitsSpeed; ++digit) {
        hud->drawAnimationFrame(renderer, Defs::AnimHUDSpeedFont,
                                speed % 10, crtX, y - 4, 0);
        speed /= 10;
        crtX -= Defs::CarSpeedFontW;
    }
// Draw gear
    hud->drawAnimationFrame(renderer, Defs::AnimHUDGearsAuto,
                            playerCar->getGear(), x - 8, y - 14, 0);

// Draw overheat
    if (playerCar->getOverheat() > 0) {
        uint8_t clipHeight = (playerCar->getOverheat() * Defs::WarningSignH) /
                              Defs::MaxOverheat;
        if (clipHeight > 0 ||
            (R3R::getInstance().getFrameCounter() & 0xF) < 8) {
            hud->drawAnimationFrame(renderer, Defs::AnimHUDRPM,
                                    Defs::HUDFrameWarning, x, y, 0);
        }
        renderer->setClip(0, y - clipHeight - 10, Defs::ScreenW, clipHeight);
        hud->drawAnimationFrame(renderer, Defs::AnimHUDRPM,
                                Defs::HUDFrameWarningOver, x, y, 0);
    }
// Draw RPM Bar
    uint8_t barLength = ((playerCar->getRPM() * Defs::RPMBarLength) /
                         Defs::MaxRPM).getInt();
    barLength = Utils::upperClamp(barLength, Defs::RPMBarLength);
    renderer->setClip(x - Defs::RPMBarLength - 1, 0, barLength, Defs::ScreenH);
    hud->drawAnimationFrame(renderer, Defs::AnimHUDRPM,
                            Defs::HUDFrameRPMBar, x, y, 0);
// Reset clip
    renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
}

void Level::drawHUD(SpriteRenderer* renderer) {
    if (playerCar->isClutched()) {
        currentGearShift->draw(renderer, 76, 52);
    } else {
        drawCarHUD(renderer, 95, 63);
    }
    if (getGameMode() == TimeAttack) {
        drawTimer(renderer, 0, 64);
    }
}

void Level::drawTimer(SpriteRenderer* renderer, uint8_t x, uint8_t y,
                      uint8_t anchor, bool addSign) {
    char* dest = getStringBuffer();
    Utils::formatTime(levelTimer, dest, addSign);
    GetFont(Defs::FontMain)->drawString(renderer, dest, x, y, anchor);
}

void Level::drawResult(SpriteRenderer* renderer, uint8_t x, uint8_t y) {
    GetFont(Defs::FontMain)->drawString(renderer,
                                    getString(
                                        static_cast<Strings>(
                                            static_cast<int>(Dead_Gearbox) +
                                            static_cast<int>(endResult))),
                                    Defs::ScreenW / 2,
                                    y,
                                    ANCHOR_VCENTER | ANCHOR_HCENTER);
    y += 12;
    if (endResult >= RaceEndTimeAttack) {
        drawTimer(renderer, x, y, ANCHOR_TOP | ANCHOR_HCENTER,
                  getGameMode() == Duel);
    }
    if (newRecord) {
        y += 12;
        Utils::drawBlinkingText(renderer, Strings::NewRecord, x, y);
    }
}

void Level::drawEndFlag(SpriteRenderer* renderer, uint8_t x,
                        uint8_t y, uint8_t w) {
    uint8_t pattern = 0b11110000;
    static const int8_t displ[] = {
        1, 1, 1, 0, 0, -1, -1, -1, 0, 0
    };
    uint8_t displIndex = R3R::getInstance().getFrameCounter() % sizeof(displ);
    int8_t displY = 0;
    uint8_t white = 255;
    uint8_t black = 0;
    int8_t lastChange = 0;
    int8_t incr = -1;
    for (int8_t crtX = x; crtX < x + w; ++crtX) {
        if (((crtX - x) & 3) == 0) {
            pattern = ~pattern;
        }
        displY = y + displ[displIndex];
        displIndex = (displIndex + 1) % sizeof(displ);
        if (displIndex < sizeof(displ) / 2) {
            white = 255 - displIndex * 25;
        } else {
            white = 255 - (sizeof(displ) - displIndex) * 25;
        }
        black = (white >> 2);
        renderer->fastDrawVerticalPattern(pattern, crtX, displY,
                                          Utils::make16BitColor(
                                                        black, black, black),
                                          Utils::make16BitColor(
                                                        white, white, white),
                                          2);
    }
}

void Level::updateControls(uint8_t buttonsState, uint8_t oldButtonsState) {
    uint8_t changedButtons = (buttonsState ^ oldButtonsState);
    if (getGameMode() == DuelDemo) {
        if (changedButtons != 0) {
            exitDemo();
        }
        return;
    }

    if (state != Race) {
        playerCar->pedalToTheMetal(false);
        playerCar->setClutch(false);
        return;
    }
    playerCar->pedalToTheMetal(buttonsState & kAccelButton);
    playerCar->setClutch(buttonsState & kClutchButton);
    if (playerCar->isClutched()) {
        if ((changedButtons & buttonsState & DPAD_UP)) {
            currentGearShift->onUp();
        }
        if ((changedButtons & buttonsState & DPAD_DOWN)) {
            currentGearShift->onDown();
        }
        if ((changedButtons & buttonsState & DPAD_LEFT)) {
            currentGearShift->onLeft();
        }
        if ((changedButtons & buttonsState & DPAD_RIGHT)) {
            currentGearShift->onRight();
        }
    } else if (changedButtons & kClutchButton) {
        int8_t shiftResult = currentGearShift->getShiftResult();
        if (shiftResult != -1) {
            playerCar->shiftGear(shiftResult);
        } else {
            playerCar->destroy();
            setEndRace(EndResultType::PlayerDeadGearbox);
        }
    }
}

void Level::checkRecord() {
    uint8_t gearMode = getGearMode();
    uint8_t gameMode = getGameMode();
    int32_t timeRecord = R3R::getInstance().getTimeRecord(gameMode, gearMode);

    if (levelTimer < timeRecord) {
        newRecord = true;
    }

    if (newRecord) {
        R3R::getInstance().updateTimeRecord(gameMode, gearMode, levelTimer);
    }
}

void Level::update(int16_t dt) {
    updateState(dt);
    updateGeneral(dt);
}

void Level::setEndRace(EndResultType type) {
    endResult = type;
    if (getGameMode() == Duel) {
        FP32 difference(enemyCar->getX() - playerCar->getX());
        difference /= Defs::MaxCarSpeed;
        difference *= 1000;
        levelTimer = difference.getInt();
    }
    raceEnd();
}

void Level::updateState(int16_t dt) {
    switch (state) {
        case Countdown:
            if (screenAnim.animPlaying() == false) {
                raceStart();
            }
        break;
        case Race: {
            if (getGameMode() != DuelDemo) {
                if (!playerCar->isAlive()) {
                    setEndRace(EndResultType::PlayerDeadEngine);
                    break;
                }
                bool playerFinished = playerCar->getX() >= Defs::RaceLength;
                bool enemyFinished = getGameMode() == Duel &&
                                     enemyCar->isAlive() &&
                                     enemyCar->getX() >= Defs::RaceLength;
                if (playerFinished && !enemyFinished) {
                    setEndRace(
                        static_cast<EndResultType>(
                            static_cast<uint8_t>(
                                EndResultType::RaceEndTimeAttack) +
                            getGameMode()));
                    break;
                } else if (!playerFinished && enemyFinished) {
                    setEndRace(EndResultType::RaceEndLose);
                    break;
                } else if (playerFinished && enemyFinished) {
                    // edge case: both pass finish line on the same frame
                    if (playerCar->getX() >= enemyCar->getX()) {
                        setEndRace(
                            static_cast<EndResultType>(
                                static_cast<uint8_t>(
                                    EndResultType::RaceEndTimeAttack) +
                                getGameMode()));
                    } else {
                        setEndRace(EndResultType::RaceEndLose);
                    }
                    break;
                }
                if (playerCar->isClutched()) {
                    currentGearShift->update();
                }
            } else {
                if (!playerCar->isAlive() &&
                    !playerCar->isExplosionVisible()) {
                    cameraTarget = enemyCar;
                }
                if (levelTimer > Defs::DemoLength ||
                    (!playerCar->isAlive() && !enemyCar->isAlive())) {
                    exitDemo();
                }
            }
            if (screenAnimType != None && playerCar->getSpeed() > 0) {
                screenAnimType = None;
            }
            if (mustMoveBarrels &&
                activeEntities[barrelOne]->isVisible() == false &&
                activeEntities[barrelTwo]->isVisible() == false) {
                activeEntities[barrelOne]->reset(Defs::RaceLength, FP32(0));
                activeEntities[barrelTwo]->reset(Defs::RaceLength, FP32(1.0f));
                mustMoveBarrels = false;
            }
            levelTimer += dt;
            if (levelTimer > 5940000) {     // limit it at 99 min
                levelTimer = 5940000;
            }
        } break;
        case Result:
            if (screenAnimType == Screen_Sprite &&
                screenAnim.animPlaying() == false) {
                screenAnimType = None;
            }
            stateCounter++;
            if (stateCounter > maxStateCounter) {
                R3R::getInstance().setState(AfterGameMenu);
                setState(Invalid);
            }
        break;
    }
}

void Level::exitDemo() {
    R3R::getInstance().setState((rand() & 63) < 21 ? Logo : Splash);
    setState(Invalid);
}

void Level::raceStart() {
    foreachGameObject([&](GameObject* obj) { obj->onRaceStart(); });
    setState(Race);
}

void Level::raceEnd() {
    foreachGameObject([&](GameObject* obj) { obj->onRaceEnd(endResult); });
    setState(Result);
}

void Level::updateGeneral(int16_t dt) {
    if (screenAnimType == Screen_Sprite) {
        screenAnim.update(dt);
    }
    foreachGameObject([&](GameObject* obj) { obj->update(dt); });
    updateCamera(dt);
    foreachGameObject([&](GameObject* obj) { obj->updateScreenX(); });
    for (auto layer : bgLayers) {
        layer->update(dt);
    }
}

void Level::drawDistanceToRival(SpriteRenderer* renderer, uint8_t x,
                                uint8_t y) {
    FP32 dist(enemyCar->getX() - playerCar->getX());
    if (dist > 0) {
        char* dest = getStringBuffer();
        Utils::formatDistance(dist.getInt(), dest);
        GetFont(Defs::FontMain)->drawString(renderer, dest, x, y,
                                            ANCHOR_RIGHT | ANCHOR_VCENTER);
    }
}

int32_t Level::worldToScreenX(const FP32& x, const FP32& y) {
    return (Defs::FPHalfScrW + Utils::metersToPixels(x - cameraPosition) *
            (FP32(0.24f) + y * FP32(0.40f))).getInt();
}

uint8_t Level::worldToScreenY(const FP32& x, const FP32& y) {
    return Defs::LevelActionAreaTop + (y * (Defs::LevelActionAreaBottom -
                                       Defs::LevelActionAreaTop)).getInt();
}

void Level::updateCamera(int16_t dt) {
    FP32 target = FP32(0.35f);
    FP32 targetCameraSpeed = 0;
    Car* following = cameraTarget;
    if (state == Result && endResult >= RaceEndLose) {
        if (stateCounter < (maxStateCounter >> 2)) {
            target += Defs::RaceLength;
            following = nullptr;
        }
    }

    if (following) {
        target += cameraTarget->getX() -
                  (Defs::CamMaxDistance * cameraTarget->getSpeed() /
                   Defs::MaxCarSpeed);
        targetCameraSpeed = cameraTarget->getSpeed();
    }

    targetCameraSpeed += (target - cameraPosition).fpAbs() *
                          Defs::CameraSpeedMultiplier;
    cameraPosition = Utils::approachWithSpeed(
                                cameraPosition, target, targetCameraSpeed, dt);
}
