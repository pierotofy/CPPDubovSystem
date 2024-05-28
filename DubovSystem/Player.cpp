//
//  Player.cpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/11/24.
//

// Copyright 2024 Michael Shapiro
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Player.hpp"

Player::Player(std::string name, int rating, int id, double points) {
    this->name = name;
    this->rating = rating;
    this->id = id;
    this->points = points;
}

Player::Player() {
    this->name = "UNNAMED PLAYER";
    this->rating = 0;
    this->id = -1;
    this->points = 0.0;
}

MatchColor::Color Player::getDueColor() {
    int num_white = 0;
    int num_black = 0;
    for(int i = 0; i < this->getNumColors(); i++) {
        if(this->color_hist[i] == MatchColor::Color::WHITE) {
            num_white += 1;
        } else if(this->color_hist[i] == MatchColor::Color::BLACK) {
            num_black += 1;
        }
    }
    if(num_white == 0 && num_black == 0) {
        return MatchColor::Color::NO_COLOR;
    }
    if(num_white == num_black) {
        MatchColor::Color last_color = this->color_hist[this->getNumColors() - 1];
        if(last_color == MatchColor::Color::WHITE) {
            return MatchColor::Color::BLACK;
        }
        return MatchColor::Color::WHITE;
    } else if(num_white > num_black) {
        return MatchColor::Color::BLACK;
    }
    return MatchColor::Color::WHITE;
}

MatchColor::ColorPreference Player::getPreferenceStrength() {
    // same operation as getDueColor
    int num_white = 0;
    int num_black = 0;
    for(int i = 0; i < this->getNumColors(); i++) {
        if(this->color_hist[i] == MatchColor::Color::WHITE) {
            num_white += 1;
        } else if(this->color_hist[i] == MatchColor::Color::BLACK) {
            num_black += 1;
        }
    }
    if(num_white == 0 && num_black == 0) {
        return MatchColor::ColorPreference::NO_PREFERENCE;
    }
    if(abs(num_white - num_black) == 1) {
        return MatchColor::ColorPreference::MILD;
    }
    if((num_white - num_black) == 0) {
        return MatchColor::ColorPreference::ALTERNATION;
    }
    // should be at least +2
    return MatchColor::ColorPreference::ABSOLUTE;
}

void Player::addOpp(int id) {
    this->opp_played.push_back(id);
    this->opp_count += 1;
}

void Player::addColor(MatchColor::Color c) {
    this->color_count += 1;
    this->color_hist.push_back(c);
}

bool Player::canPlayOpp(Player opp) {
    // check if both players have played each other before
    if(std::find(this->opp_played.begin(), this->opp_played.end(), opp.getID()) != this->opp_played.end()) {
        return false;
    }
    // check if the absolute color preference is present in both players and both players need that color
    MatchColor::Color c1 = this->getDueColor();
    MatchColor::Color c2 = opp.getDueColor();
    if(c1 == c2) {
        MatchColor::ColorPreference s1 = this->getPreferenceStrength();
        MatchColor::ColorPreference s2 = opp.getPreferenceStrength();
        
        if(s1 == MatchColor::ColorPreference::ABSOLUTE && s2 == MatchColor::ColorPreference::ABSOLUTE) {
            return false;
        }
    }
    return true;
}

double Player::getARO() {
    if(this->aro > -1) {
        return this->aro;
    }
    int tot = 0;
    for(int i = 0; i < this->opp_count; i++) {
        tot += this->opp_rating[i];
    }
    this->aro = (tot / this->opp_count);
    return (tot / this->opp_count);
}

void Player::addOppRating(int r) {
    this->opp_rating.push_back(r);
}

void Player::incrementUpfloat() {
    this->numUpfloated += 1;
}

bool Player::canUpfloat(int cr) {
    int max_allowed = 2 + (floor(cr / 5));
    if(this->numUpfloated >= max_allowed) {
        return false;
    }
    return true;
}

void Player::setUpfloatPrevStatus(bool s) {
    this->upfloated_prev = s;
}

void Player::addPoints(const double pt) {
    this->points += pt;
}
