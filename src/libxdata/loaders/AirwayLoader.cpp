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
#include "AirwayLoader.h"
#include <iostream>
#include <sstream>

namespace xdata {

AirwayLoader::AirwayLoader(const std::string& file):
    parser(file)
{
    parser.parseHeader();
}

std::string AirwayLoader::getHeader() const {
    return header;
}

void AirwayLoader::setAcceptor(Acceptor a) {
    acceptor = a;
}

void AirwayLoader::loadAirways() {
    using namespace std::placeholders;
    parser.eachLine(std::bind(&AirwayLoader::parseLine, this));
}

void AirwayLoader::parseLine() {
    std::string firstWord = parser.parseWord();

    if (firstWord == "99" || firstWord.empty()) {
        return;
    }

    AirwayData awy {};

    awy.beginID = firstWord;
    awy.beginIcaoRegion = parser.parseWord();
    awy.beginType = parseNavType(parser.parseInt());

    awy.endID = parser.parseWord();
    awy.endIcaoRegion = parser.parseWord();
    awy.endType = parseNavType(parser.parseInt());

    awy.dirRestriction = parseDirectionalRestriction(parser.parseWord());
    awy.level = parseLevel(parser.parseInt());

    awy.base = parser.parseInt();
    awy.top = parser.parseInt();
    awy.name = parser.parseWord();

    if (!awy.name.empty()) {
        std::stringstream nameStream(awy.name);
        std::string name;
        while (std::getline(nameStream, name, '-')) {
            awy.name = name;
            acceptor(awy);
        }
    }
}

AirwayData::AltitudeLevel AirwayLoader::parseLevel(int num) {
    switch (num) {
    case 1: return AirwayData::AltitudeLevel::LOW;
    case 2: return AirwayData::AltitudeLevel::HIGH;
    default:  throw std::runtime_error("Unknown High / Low type: " + std::to_string(num));
    }
}

AirwayData::NavType AirwayLoader::parseNavType(int type) {
    switch (type) {
    case 11:  return AirwayData::NavType::FIX;
    case 2:   return AirwayData::NavType::NDB;
    case 3:   return AirwayData::NavType::VHF;
    default:  throw std::runtime_error("Unknown segment type: " + std::to_string(type));
    }
}

AirwayData::DirectionRestriction AirwayLoader::parseDirectionalRestriction(const std::string& dir) {
    if (dir == "N") {
        return AirwayData::DirectionRestriction::NONE;
    } else if (dir == "F") {
        return AirwayData::DirectionRestriction::FORWARD;
    } else if (dir == "B") {
        return AirwayData::DirectionRestriction::BACKWARD;
    } else {
        throw std::runtime_error("Unknown direction restriction: " + dir);
    }
}

}
/* namespace xdata */
