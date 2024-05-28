//
//  fpc.hpp
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

#ifndef fpc_hpp
#define fpc_hpp

#include <stdio.h>
#include <vector>
#include "trf util/trf.hpp"
#include "Tournament.hpp"
#include "Player.hpp"

/// Free Pairings Checker
/// FIDE requires this for endorsement
/// The function of this is to simply validate some pairings input

/**
 * Free pairings checker
 */
namespace fpc {
class RawMatch {
public:
    Player white;
    Player black;
    bool is_bye;
    RawMatch(Player white, Player black, bool is_bye);
};
class PairingsChecker {
private:
    std::string rawTournamentData;
    int target_round;
    std::vector<PlayerDivider::Utils::TRFMatch> match_check;
    std::vector<Match> games_expected;
    std::map<int, Player> players_scaned;
    int bad_opp = 0;
    int bad_color = 0;
    /**
     * For evaluating the raw matchs
     */
    void scan();
public:
    /**
     * We initiate the raw tournament read from the TRF file, and we accept the parameter for the target round to check pairings for
     */
    PairingsChecker(std::string rawTournament, int targetRound);
    /**
     * Returns the string representation of the report
     */
    std::string outputReport();
};
}

#endif /* fpc_hpp */
