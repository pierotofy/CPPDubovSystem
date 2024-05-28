//
//  fpc.cpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/23/24.
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

#include "fpc.hpp"
#include <map>

fpc::PairingsChecker::PairingsChecker(std::string rawTournament, int targetRound) {
    this->rawTournamentData = rawTournament;
    this->target_round = targetRound;
    if(targetRound < 0) {
        throw std::invalid_argument("Target round cannot be negative!");
    }
}

fpc::RawMatch::RawMatch(Player white, Player black, bool is_bye) {
    this->white = white;
    this->black = black;
    this->is_bye = is_bye;
}

void fpc::PairingsChecker::scan() {
    // rebuild tournament
    TRFUtil::TRFFile trf_read(this->rawTournamentData);
    TRFUtil::TRFData raw_data = trf_read.read();
    
    int next_round = 0; // dummy variable
    Tournament rawTournament = Tournament::makeTournament(raw_data, &next_round, this->target_round);
    
    // make sure targetRound is valid
    std::vector<PlayerDivider::Utils::TRFMatch> raw_matches = rawTournament.getRawMatches();
    this->match_check = raw_matches;
    if(((int) raw_matches.size()) == 0) {
        throw std::length_error("Pairings Checker cannot check pairings because invalid round was given to check. Tournament has not reached round " + std::to_string(this->target_round) + " yet.");
    }
    
    // set up player map for comparisons
    std::map<int, Player> comp;
    for(int i = 0; i < rawTournament.getPlayerCount(); i++) {
        comp[rawTournament.getPlayers()[i].getID()] = rawTournament.getPlayers()[i];
    }
    
    // quickly scan current pairings for comparison
    
    for(int i = 0; i < raw_matches.size(); i++) {
        // check if both players have played each other before
        if(!comp[raw_matches[i].getWID()].canPlayOpp(comp[raw_matches[i].getBID()])) {
            bad_opp += 1;
            continue;
        }
        
        // check due colors
        MatchColor::Color wd = comp[raw_matches[i].getWID()].getDueColor();
        MatchColor::Color bd = comp[raw_matches[i].getBID()].getDueColor();
        
        if(wd == bd) {
            // check strengths
            MatchColor::ColorPreference wp = comp[raw_matches[i].getWID()].getPreferenceStrength();
            MatchColor::ColorPreference bp = comp[raw_matches[i].getBID()].getPreferenceStrength();
            
            if(wp == MatchColor::ColorPreference::ABSOLUTE && bp == MatchColor::ColorPreference::ABSOLUTE) {
                bad_color += 1;
                continue;
            }
        }
    }
    
    // make pairings for expected
    this->games_expected = rawTournament.generatePairings(this->target_round);
    this->players_scaned = comp;
}

std::string fpc::PairingsChecker::outputReport() {
    std::string output = "----------CPP FIDE DUBOV SYSTEM PAIRINGS CHECKER REPORT----------\n";
    output += "----------CHECKING FOR ROUND: " + std::to_string(this->target_round) + "--------\n";
    output += "----------RUNNING SCANS--------------------------\n";
    
    this->scan();
    
    output += "CHECKER PAIRINGS (EXPECTED):\n\n";
    for(int i = 0; i < this->games_expected.size(); i++) {
        output += this->games_expected[i].white.getName() + " - ";
        if(this->games_expected[i].is_bye) {
            output += "BYE\n";
        } else {
            output += this->games_expected[i].black.getName() + "\n";
        }
    }
    output += "\nTOURNAMENT PAIRINGS (TO CHECK):\n\n";
    for(int i = 0; i < this->match_check.size(); i++) {
        output += this->players_scaned[this->match_check[i].getWID()].getName() + " - ";
        //TODO: BYE CHECK
        output += this->players_scaned[this->match_check[i].getBID()].getName() + "\n";
    }
    
    output += "ERRORS TOTAL DETECTED IN TOURNAMENT PAIRINGS: " + std::to_string(this->bad_opp + this->bad_color);
    output += "\nNUMBER OF PAIRINGS THAT VIOLATE C1 (TWO PLAYERS SHALL NOT MEET TWICE): " + std::to_string(this->bad_opp);
    output += "\nNUMBER OF PAIRINGS THAT VIOLATE C3 (COLOR PREF.): " + std::to_string(this->bad_color);
    output += "\n\nPAIRINGS CHECKER REPORT DONE";
    
    return output;
}
