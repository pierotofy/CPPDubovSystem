//
//  trf.hpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/14/24.
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

#ifndef trf_hpp
#define trf_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

/**
 * For reading & writing TRF data
 */
namespace TRFUtil {
/**
 * Removes all spaces from a given string
 */
void clearSpaces(std::string *str_clear);
/**
 * A sample piece of TRF data
 */
class TRFData {
private:
    std::map<std::string, std::string> tournament_section;
    std::vector<std::map<std::string, std::string>> player_section;
    int rounds_captured;
    int rounds_tnr;
    /**
     * Parses player data
     */
    void parsePlayer(std::string line);
public:
    /**
     * Gets collected data from tournament section
     */
    std::map<std::string, std::string> getTournamentSection() {return tournament_section;}
    /**
     * Gets player section info
     */
    std::vector<std::map<std::string, std::string>> getPlayerSection() {return player_section;}
    /**
     * Gets the number of rounds read
     */
    int getRounds() {return rounds_captured;}
    /**
     * Gets the rounds from the captured TNR code
     */
    int getRoundsTnr() {return rounds_tnr;}
    /**
     * Parses a trf line
     */
    void parseLine(std::string line);
};
class TRFFile {
private:
    std::string path;
public:
    TRFFile(std::string const path);
    /**
     * A simple getter for path
     */
    std::string getPath() {return path;}
    /**
     * Writes the TRF data to the path
     */
    void write(std::string trf);
    /**
     * Reads the TRF data
     */
    TRFData read();
};
}

#endif /* trf_hpp */
