/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2018 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "MapApp.h"
#include "src/Logger.h"

namespace avitab {

MapApp::MapApp(FuncsPtr funcs):
    App(funcs),
    window(std::make_shared<Window>(getUIContainer(), "Maps")),
    updateTimer(std::bind(&MapApp::update, this), 200)
{
    window->setOnClose([this] () { exit(); });
    window->addSymbol(Widget::Symbol::MINUS, std::bind(&MapApp::onMinusButton, this));
    window->addSymbol(Widget::Symbol::PLUS, std::bind(&MapApp::onPlusButton, this));
    window->addSymbol(Widget::Symbol::GPS, std::bind(&MapApp::onTrackButton, this));

    map = std::make_unique<maps::OSMMap>(window->getContentWidth(), window->getContentHeight());
    map->setCacheDirectory(api().getDataPath() + "MapTiles/");
    map->setOverlayDirectory(api().getDataPath() + "icons/");

    mapWidget = std::make_shared<PixMap>(window);
    mapWidget->setClickable(true);
    mapWidget->setClickHandler([this] (int x, int y) { onMapClicked(x, y); });

    map->setRedrawCallback([this] () { onRedrawNeeded(); });

    update();
}

void MapApp::onRedrawNeeded() {
    mapWidget->draw(map->getImage());
}

void MapApp::onMapClicked(int x, int y) {
    trackPlane = false;
    map->moveCenterTo(x, y);
}

void MapApp::onMouseWheel(int dir, int x, int y) {
    if (dir > 0) {
        map->zoomIn();
    } else {
        map->zoomOut();
    }
    update();
}

void MapApp::onPlusButton() {
    map->zoomIn();
    update();
}

void MapApp::onMinusButton() {
    map->zoomOut();
    update();
}

void MapApp::onTrackButton() {
    trackPlane = !trackPlane;
}

bool MapApp::update() {
    double planeLat = api().getDataRef("sim/flightmodel/position/latitude").doubleValue;
    double planeLon = api().getDataRef("sim/flightmodel/position/longitude").doubleValue;
    float planeHeading = api().getDataRef("sim/flightmodel/position/psi").floatValue;

    if (trackPlane) {
        map->centerOnPlane(planeLat, planeLon, planeHeading);
    } else {
        map->setPlanePosition(planeLat, planeLon, planeHeading);
    }

    return true;
}

} /* namespace avitab */
