//
//  Player.hpp
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

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include <string>
#include <vector>

/**
 * Match Color Info
 */
namespace MatchColor {
/**
 * A simple color for a match
 */
enum Color {
    WHITE, BLACK, NO_COLOR
};
/**
 * Color preference for a match
 */
enum ColorPreference {
    NO_PREFERENCE, MILD, ABSOLUTE, ALTERNATION
};
}

/**
 * Core player
 */
class Player {
private:
    /**
     * Player name
     */
    std::string name;
    /**
     * Rating of player
     */
    int rating;
    /**
     * Id
     */
    int id;
    /**
     * Number of points the player has
     */
    double points;
    /**
     * Opponents played
     */
    std::vector<int> opp_played;
    /**
     * History of colors played with
     */
    std::vector<MatchColor::Color> color_hist;
    /**
     * Ratings of each opponent
     */
    std::vector<int> opp_rating;
    /**
     * Number of opponents played
     */
    int opp_count = 0;
    /**
     * Number of colors played
     */
    int color_count = 0;
    /**
     * Number of times upfloated
     */
    int numUpfloated = 0;
    /**
     * For easy memory storage of aro when we need it
     */
    double aro = -1.0;
    /**
     * For players who upfloated in the last round
     */
    bool upfloated_prev = false;
    /**
     * If the player had a bye/forfeit win
     */
    bool received_bye = false;
    
public:
    
    /**
     * For quick easy trf parsing
     */
    double trf_pts = 0.0;
    
    Player(std::string name, int rating, int id, double points);
    Player();
    
    /**
     * A getter for player name
     */
    std::string getName() {return name;}
    /**
     * A getter for rating
     */
    int getRating() {return rating;}
    /**
     * A getter for ID
     */
    int getID() {return id;}
    /**
     * A getter for the number of colors played
     */
    int getNumColors() {return color_count;}
    /**
     * Gets the number of opponents played
     */
    int getOppCount() {return opp_count;}
    /**
     * A simple getter for numUpfloated
     */
    int getNumUpfloat() {return numUpfloated;}
    /**
     * A simple getter for points
     */
    double getPoints() {return points;}
    /**
     * Determines if the two players can play each other
     */
    bool canPlayOpp(Player opp);
    /**
     * Gets the average rating of opponents
     */
    double getARO();
    /**
     * Adds an opponent to the list of played opponents
     */
    void addOpp(int id);
    /**
     * Adds a color to the list of colors
     */
    void addColor(MatchColor::Color c);
    /**
     * Gets the due color of the player
     */
    MatchColor::Color getDueColor();
    /**
     * Gets the strength of the due color
     */
    MatchColor::ColorPreference getPreferenceStrength();
    /**
     * Increments upfloat count
     */
    void incrementUpfloat();
    /**
     * Determines if the player can upfloat
     */
    bool canUpfloat(int cr);
    /**
     * A getter for opponents played
     */
    std::vector<int> getOppPlayed() {return opp_played;}
    /**
     * Adds the opponents rating to the list of ratings
     */
    void addOppRating(int r);
    /**
     * Checks up a player upfloated previously
     */
    bool upfloatedPreviously() {return upfloated_prev;}
    /**
     * Sets the status of the player upfloat
     */
    void setUpfloatPrevStatus(bool s);
    /**
     * Adds points to the player
     */
    void addPoints(const double pt);
    /**
     * Checks if the player has received a bye
     */
    bool hasReceievedBye() {return received_bye;}
    /**
     * Sets the bye status of player
     */
    void setByeStatus(bool s) {received_bye = s;}
};

#endif /* Player_hpp */
