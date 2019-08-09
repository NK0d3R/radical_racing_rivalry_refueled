// Copyright 2018 Catalin G. Manciu

#include "level.h"
#include "car.h"
#include "enemycar.h"
#include "../res/stringmgr.h"
#include "../engine/renderer.h"
#include "../res/sprites.h"

Level::BackgroundLayer* Level::bgLayers[] = {
    new BackgroundSprite(25, 0, Defs::BackgroundSun, 0),
    new BackgroundSprite(25, 190, Defs::BackgroundLayer1, 25),
    new BackgroundSprite(25, 190, Defs::BackgroundLayer2, 100),
    new BackgroundGrid(25, 40, 10, 135),
    new BackgroundSprite(42, 240, Defs::BackgroundLayer3, 430)
};

int16_t Level::BackgroundLayer::camPosToOffset(const FP32& cameraPosition) {
    FP32 camPosScaled = (cameraPosition * offsetFactor) / 1000;
    return -(Utils::metersToPixels(camPosScaled).getInt());
}

void Level::BackgroundGrid::drawSingleLine(SpriteRenderer* renderer,
                                           int16_t x, int16_t yTop,
                                           int16_t yBot) {
    FP32 lineX(x);
    FP32 lineXBottom = lineX * FP32(3.25f);
    Line current(lineX + Defs::FPHalfScrW, FP32(yTop),
                 lineXBottom + Defs::FPHalfScrW, FP32(yBot));
    renderer->getClip().clipLineX(&current);
    if (current.exists()) {
        renderer->drawLine(current.start.x.getInt(),
                           current.start.y.getInt(),
                           current.end.x.getInt(),
                           current.end.y.getInt());
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
        renderer->drawLine(0, y, Defs::ScreenW - 1, y);
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


Level::BackgroundChopper::BackgroundChopper() : BackgroundLayer(0) {
    chopperAnim.init(GetSprite(Defs::SpriteEnv));
    xSpeed = FP32(random(100) & 1 ? 1 : -1);
    wait(true);
}

void Level::BackgroundChopper::wait(bool isWaiting) {
    waiting = isWaiting;
    if (waiting) {
        timer = random(Defs::BgChopperMinWaitTime, Defs::BgChopperMaxWaitTime);
    } else {
        timer = Defs::BgChopperDecisionTime;
    }
}

void Level::BackgroundChopper::restart() {
    int32_t direction = -Utils::sgn(xSpeed.getInt());
    if (direction < 0) {
        xPos = FP32(Defs::ScreenW + Defs::BgChopperMarginOffset);
    } else {
        xPos = FP32(-Defs::BgChopperMarginOffset);
    }
    xSpeed = FP32(direction * random(20, 40));
    chopperAnim.setAnimation(Defs::AnimChopper,
                             direction == -1 ? ARD_FLAGS_FLIP_X : 0,
                             true);
    yPos = random(10, 15);
}

void Level::BackgroundChopper::update(int16_t dt) {
    if (waiting == false) {
        xPos += (xSpeed * FP32(dt)) / 1000;
        if (xPos.getInt() < -Defs::BgChopperMarginOffset ||
            xPos.getInt() > Defs::ScreenW + Defs::BgChopperMarginOffset) {
            wait(true);
        } else {
            timer -= dt;
            if (timer < 0) {
                timer = Defs::BgChopperDecisionTime;
                if (random(100) < 60) {
                    yPos += random(-1, 3);
                }
                if (random(100) < 10) {
                    xSpeed *= -1;
                    chopperAnim.setAnimation(Defs::AnimChopper,
                                             xSpeed.getInt() < 0 ?
                                             ARD_FLAGS_FLIP_X : 0,
                                             true);
                }
            }
        }
        chopperAnim.update(dt);
    } else {
        timer -= dt;
        if (timer < 0) {
            wait(false);
            restart();
        }
    }
}

void Level::BackgroundChopper::draw(SpriteRenderer* renderer,
                                   const FP32& cameraPosition) {
    if (waiting == false) {
        chopperAnim.draw(renderer, xPos.getInt(), yPos);
    }
}

Level::Level() {
    playerCar = new Car(this, 43);
    enemyCar = new EnemyCar(this, 43);
    screenAnim.init(GetSprite(Defs::SpriteCar));
    state = Invalid;
}

void Level::restart() {
    playerCar->reset(getGameMode() == Duel ? FP32(0.9f) : FP32(0.6f));
    if (getGameMode() == Duel) {
        enemyCar->reset(FP32(0.3f));
    }
    levelTimer = 0;
    endResult = NoResult;
    newRecord = false;
    screenAnimType = None;
    currentGearShift->reset();
    setState(Countdown);
}

void Level::startScreenAnim(uint8_t x, uint8_t y, ScreenAnimType type,
                            uint8_t anim, bool loop) {
    scrAnimX = x;
    scrAnimY = y;
    screenAnimType = type;
    if (type == Sprite) {
        screenAnim.setAnimation(anim, 0, loop);
    }
}

void Level::setState(LevelState newState) {
    if (newState != state) {
        switch (newState) {
            case Countdown:
                startScreenAnim(Defs::ScreenW / 2, Defs::ScreenH / 2,
                                Sprite, Defs::AnimCarCountdown);
                cameraPosition = -200;
            break;
            case Result:
                stateCounter = 0;
                if (endResult < RaceEndLose) {
                    startScreenAnim(120, 68,
                                    Sprite, Defs::AnimCarExplosion);
                    maxStateCounter = 120;
                } else {
                    if (endResult != RaceEndLose) {
                        checkRecord();
                    }
                    startScreenAnim((Defs::ScreenW - Defs::EndFlagW) / 2,
                                     Defs::ResultTextY - (Defs::EndFlagH / 2),
                                     Flag);
                    maxStateCounter = 240;
                }
            break;
        }
        state = newState;
    }
}

void Level::foreachGameObject(auto func) {
    if (getGameMode() == Duel) {
        func(enemyCar);
    }
    func(playerCar);
}

void Level::draw(SpriteRenderer* renderer) {
    if (state == Invalid) return;
    for (auto layer : bgLayers) {
        layer->draw(renderer, cameraPosition);
    }
    drawGameMarkers(renderer);
    foreachGameObject([&](GameObject* obj) {
                       if (obj->isVisible()) obj->draw(renderer);
                      });

    if (state == Race) {
        drawHUD(renderer);
        if (getGameMode() == Duel && !enemyCar->isVisible()) {
            drawDistanceToRival(renderer, 127, 32);
        }
    }

    switch (screenAnimType) {
        case Sprite: screenAnim.draw(renderer, scrAnimX, scrAnimY); break;
        case Flag: drawEndFlag(renderer, scrAnimX, scrAnimY,
                               Defs::EndFlagW); break;
    }

    if (state == Result) {
        drawResult(renderer, Defs::ScreenW / 2, Defs::ResultTextY);
    }
}

#define DEBUG_ENEMY_SPEED   (0)

void Level::drawCarHUD(SpriteRenderer* renderer, uint8_t x, uint8_t y) {
// Draw RPM Bar background
    GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                    renderer, Defs::AnimCarRPMHud,
                                    Defs::HUDFrameRPM, x, y, 0);
// Draw speed
    static constexpr uint8_t kNbDigitsSpeed = 3;
    uint8_t crtX = x + 23;
    uint8_t crtY = y - 5;
    int32_t speed = Utils::mpsToKph(playerCar->getSpeed()).getInt();
    for (uint8_t digit = 0; digit < kNbDigitsSpeed; ++digit) {
        GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                        renderer, Defs::AnimCarSpeedFont,
                                        (speed % 10), crtX, crtY, 0);
        speed /= 10;
        crtX -= Defs::CarSpeedFontW;
    }
// Draw gear
    GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                    renderer, Defs::AnimCarGearsAuto,
                                    playerCar->getGear(), x - 4, y - 3, 0);
// Draw unit
    if (playerCar->getOverheat() == 0) {
        GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                        renderer, Defs::AnimCarRPMHud,
                                        Defs::HUDFrameKPH, x, y, 0);
    } else {
        crtX = x + 24;
        crtY = y - 7;
        GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                        renderer, Defs::AnimCarRPMHud,
                                        Defs::HUDFrameWarning, crtX, crtY, 0);
        uint8_t clipHeight = playerCar->getOverheat() * Defs::WarningSignH /
                             Defs::MaxOverheat;
        renderer->setClip(0, crtY - clipHeight, Defs::ScreenW, clipHeight);
        GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                        renderer, Defs::AnimCarRPMHud,
                                        Defs::HUDFrameWarningOver,
                                        crtX, crtY, 0);
    }
// Draw RPM Bar
    uint8_t barLength = ((playerCar->getRPM() * Defs::RPMBarLength) /
                          Defs::MaxRPM).getInt();
    barLength = Utils::upperClamp(barLength, Defs::RPMBarLength);
    renderer->setClip(x + 1, 0, barLength + 2, Defs::ScreenH);
    GetSprite(Defs::SpriteCar)->drawAnimationFrame(
                                    renderer, Defs::AnimCarRPMHud,
                                    Defs::HUDFrameRPMBar, x, y, 0);
// Reset clip
    renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
}

void Level::drawHUD(SpriteRenderer* renderer) {
    if (playerCar->isClutched()) {
        currentGearShift->draw(renderer, 110, 52);
    } else {
        drawCarHUD(renderer, 94, 64);
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
                                    getString(Dead_Gearbox +
                                    static_cast<Strings>(endResult)),
                                    Defs::ScreenW / 2,
                                    y,
                                    ANCHOR_VCENTER | ANCHOR_HCENTER);
    y += 12;
    if (endResult > PlayerDeadEngine) {
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
    static PROGMEM const int8_t displ[] = {
        0, 0, 1, 1, 1, 1, 0, 0, 0, -1, -1, -1
    };
    uint8_t displIndex = (getFrameCounter() >> 1) % sizeof(displ);
    int8_t displY = 0;
    for (int8_t crtX = x; crtX < x + w; ++crtX) {
        if (((crtX - x) & 3) == 0) {
            pattern = ~pattern;
        }
        displY = y + (int8_t)pgm_read_byte(&displ[displIndex]);
        displIndex = (displIndex + 1) % sizeof(displ);
        renderer->fastDrawVerticalPattern(pattern, crtX, displY);
        renderer->fastDrawVerticalPattern(pattern, crtX, displY + 8);
    }
}

void Level::updateControls(uint8_t buttonsState, uint8_t oldButtonsState) {
    uint8_t changedButtons = (buttonsState ^ oldButtonsState);
    if (state != Race) {
        playerCar->pedalToTheMetal(false);
        playerCar->setClutch(false);
        return;
    }
    playerCar->pedalToTheMetal(buttonsState & kAccelButton);
    playerCar->setClutch(buttonsState & kClutchButton);
    if (playerCar->isClutched()) {
        if ((changedButtons & buttonsState & UP_BUTTON)) {
            currentGearShift->onUp();
        }
        if ((changedButtons & buttonsState & DOWN_BUTTON)) {
            currentGearShift->onDown();
        }
        if ((changedButtons & buttonsState & LEFT_BUTTON)) {
            currentGearShift->onLeft();
        }
        if ((changedButtons & buttonsState & RIGHT_BUTTON)) {
            currentGearShift->onRight();
        }
    } else if (changedButtons & kClutchButton) {
        int8_t shiftResult = currentGearShift->getShiftResult();
        if (shiftResult != -1) {
            playerCar->shiftGear(shiftResult);
        } else {
            playerCar->destroy();
            endResult = EndResultType::PlayerDeadGearbox;
        }
    }
}

void Level::checkRecord() {
    uint8_t gearMode = getGearMode();
    uint8_t gameMode = getGameMode();
    int32_t timeRecord = getTimeRecord(gameMode, gearMode);

    if (levelTimer < timeRecord) {
        newRecord = true;
    }

    if (newRecord) {
        updateTimeRecord(gameMode, gearMode, levelTimer);
    }
}

void Level::update(int16_t dt) {
    updateState(dt);
    updateGeneral(dt);
}

void Level::setEndRace(EndResultType type) {
    if (endResult == NoResult) {
        endResult = type;
    }
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
            if (!playerCar->isAlive()) {
                setEndRace(EndResultType::PlayerDeadEngine);
                break;
            }
            bool playerFinished = playerCar->getX() >= Defs::RaceLength;
            bool enemyFinished = getGameMode() == Duel && enemyCar->isAlive()
                                 && enemyCar->getX() >= Defs::RaceLength;
            if (playerFinished && !enemyFinished) {
                setEndRace(EndResultType::RaceEndTimeAttack + getGameMode());
                break;
            } else if (!playerFinished && enemyFinished) {
                setEndRace(EndResultType::RaceEndLose);
                break;
            } else if (playerFinished && enemyFinished) {
                // edge case: both pass finish line on the same frame
                if (playerCar->getX() >= enemyCar->getX()) {
                    setEndRace(
                        EndResultType::RaceEndTimeAttack + getGameMode());
                } else {
                    setEndRace(EndResultType::RaceEndLose);
                }
                break;
            }
            if (playerCar->isClutched()) {
                currentGearShift->update();
            }
            if (screenAnimType != None && playerCar->getSpeed() > 0) {
                screenAnimType = None;
            }
            levelTimer += dt;
            if (levelTimer > 5940000) {     // limit it at 99 min
                levelTimer = 5940000;
            }
        } break;
        case Result:
            if (screenAnimType == Sprite && screenAnim.animPlaying() == false) {
                screenAnimType = None;
            }
            stateCounter++;
            if (stateCounter > maxStateCounter) {
                setAppState(AfterGameMenu);
                setState(Invalid);
            }
        break;
    }
}

void Level::raceStart() {
    foreachGameObject([&](GameObject* obj) { obj->onRaceStart(); });
    setState(Race);
}

void Level::raceEnd() {
    foreachGameObject([&](GameObject* obj) { obj->onRaceEnd(); });
    setState(Result);
}

void Level::updateGeneral(int16_t dt) {
    if (screenAnimType == Sprite) {
        screenAnim.update(dt);
    }
    foreachGameObject([&](GameObject* obj) { obj->update(dt); });
    updateCamera();
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

void Level::drawMarker(SpriteRenderer* renderer, const FP32& worldPos) {
    FP32 x1(worldToScreenX(worldPos, FP32(0)));
    FP32 y1(worldToScreenY(worldPos, FP32(0)));
    FP32 x2(worldToScreenX(worldPos, FP32(1)));
    FP32 y2(worldToScreenY(worldPos, FP32(1)));
    Line current(x1, y1, x2, y2);
    renderer->getClip().clipLineX(&current);
    if (current.exists()) {
        renderer->drawLine(current.start.x.getInt(),
                           current.start.y.getInt(),
                           current.end.x.getInt(),
                           current.end.y.getInt());
    }
}

void Level::drawGameMarkers(SpriteRenderer* renderer) {
    drawMarker(renderer, FP32(0));
    drawMarker(renderer, Defs::RaceLength);
}

int32_t Level::worldToScreenX(const FP32& x, const FP32& y) {
    return (Defs::FPHalfScrW + Utils::metersToPixels(x - cameraPosition) *
            (FP32(0.7f) + y * FP32(0.5f))).getInt();
}

uint8_t Level::worldToScreenY(const FP32& x, const FP32& y) {
    return Defs::LevelActionAreaTop + (y * (Defs::LevelActionAreaBottom -
                                            Defs::LevelActionAreaTop)).getInt();
}

void Level::updateCamera() {
    FP32 target = playerCar->getX();
    if (state == Result && endResult >= RaceEndLose) {
        if (stateCounter < (maxStateCounter >> 2)) {
            target = Defs::RaceLength;
        }
    }
    target += FP32(1.5f);
    cameraPosition += target;
    cameraPosition /= 2;
}
