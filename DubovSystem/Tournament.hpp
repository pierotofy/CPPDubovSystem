//
//  Tournament.hpp
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

#ifndef Tournament_hpp
#define Tournament_hpp

#include <stdio.h>
#include "Player.hpp"
#include "LinkedList.hpp"
#include "trf util/trf.hpp"
#include "trf util/rtg.hpp"
#include <vector>


/**
 * A simple match
 */
class Match {
public:
    Player white;
    Player black;
    bool is_bye;
    Match(Player white, Player black, bool is_bye);
};

/**
 * For evaluating matches
 */
class MatchEval: public Match {
private:
    bool problem;
public:
    MatchEval(Player white, Player black, bool problem);
    
    /**
     * Sets the problem
     */
    void setProblem(bool p);
    /**
     * Checks if the board has a problem
     */
    bool isProblem() {return problem;}
};

namespace PlayerDivider {
void mergeGroupARO(std::vector<Player> *group, int const left, int const mid, int const right);
/**
 * For merge sort (sorting players by rating)
 */
void playersRatingMerge(std::vector<Player> *players, int const left, int const mid, int const right);
void mergeGroupAROG(std::vector<Player> *group, int const left, int const mid, int const right);
/**
 * Sorts a group by ARO (average rating of opponent)
 */
void sortGroupARO(std::vector<Player> *group, int const begin, int const end);
/**
 * For sorting players by rating
 */
void sortPlayersRating(std::vector<Player> *players, int const begin, int const end);
/**
 * Sorts a group by ARO from greatest to least
 */
void sortGroupAROG(std::vector<Player> *group, int const begin, int const end);
void splitGroups(std::vector<Player> *white_seekers, std::vector<Player> *black_seekers, std::vector<Player> group);
void pairGroup(std::vector<Player> white_seekers, std::vector<Player> black_seekers, std::vector<Match> *games);
namespace Utils {
/**
 * A raw TRF match with read data and information
 */
class TRFMatch {
private:
    Player *white;
    Player *black;
    unsigned int target_round;
public:
    
    double white_pts;
    double black_pts;
    bool is_bye = false;
    
    TRFMatch(Player *white, Player *black, const double white_pts, const double black_pts, unsigned const int target_round);
    TRFMatch();
    
    /**
     * Sets player white
     */
    void setWhite(Player *w) {white = w;}
    /**
     * Sets player black
     */
    void setBlack(Player *b) {black = b;}
    /**
     * Sets white points
     */
    void setWhitePts(double w) {white_pts = w;}
    /**
     * Sets black points
     */
    void setBlackPts(double b) {black_pts = b;}
    /**
     * Gets the target round the match is registered to
     */
    int getTargetRound() {return target_round;}
    
    /**
     * Distributes the point information to the players
     */
    void distributePoints();
    /**
     * Distributes float info to the players
     */
    void distributeFloat();
    
    /**
     * Gets the ID of white player
     */
    int getWID();
    /**
     * Gets the ID of black player
     */
    int getBID();
};
/**
 * For merge sort. Sorts the raw matches
 */
void mergeRawMatches(std::vector<PlayerDivider::Utils::TRFMatch> *raw_games, int const left, int const mid, int const right);
/**
 * Sorts the raw TRF matches by round
 */
void sortRawMatches(std::vector<PlayerDivider::Utils::TRFMatch> *raw_games, const int begin, const int end);
/**
 * Merges matches in the recursive call
 */
void mergeMatches(std::vector<Match> m1, std::vector<Match> *main);
/**
 * For merging upfloat wrappers
 */
void mergeUpfloaterWrappers(std::vector<Player> wrong_colors, std::vector<Player> max_upfloat, std::vector<Player> *upfloaters);
/**
 * Finds a list of upfloaters for a given imbalanced group
 */
std::vector<Player> findUpfloaters(LinkedListNode next_group, int imbalance, int max_rounds, int pairing_round);
/**
 * Moves players from one group to the other
 */
void migratePlayers(std::vector<Player> *g1, std::vector<Player> *g2, bool *error);
/**
 * Migrates players in the same way as migratePlayer, except moves players who don't have a due color (0 white, 0 black) into the smaller score group
 */
void migratePlayersNoColor(std::vector<Player> *g1, std::vector<Player> *g2);
/**
 * Shifts the index of the exchange
 */
int shiftIndexExchange(std::vector<MatchEval> games, int start, int shift, int problem, bool *error);
/**
 * Similar to shiftIndexExchange except this focuses on s1 primarily
 */
int shiftIndexExchangeS1(std::vector<MatchEval> games, int start, int shift, int problem, bool *error);
/**
 * Finds the first index of an exchange that should be done
 */
int firstIndexExchange(std::vector<MatchEval> games, int problem, bool *error);
/**
 * Makes a swap by exchange
 */
void makeExchangeSwap(std::vector<MatchEval> *games, int problem_board, int target_board);
/**
 * Applies exchanges to the games
 */
void maximizeExchanges(std::vector<MatchEval> *games_eval, bool *error);
/**
 * Maximizes the pairings for a group
 */
std::vector<Match> maximizePairings(std::vector<Player> white_seekers, std::vector<Player> black_seekers, bool *pairing_error);
/**
 * Determines if swapping the two black players in m1 and m2 produces a valid pairing
 */
bool isValidMatch(MatchEval m1, MatchEval m2);
/**
 * Finds the first board where a valid swap can be made
 */
int shiftIndex(MatchEval m1, int start, std::vector<MatchEval> games, int shift_count, bool *error);
/**
 * Swaps two players in a match eval
 */
void swap(MatchEval *m1, MatchEval *m2, bool m1_white, bool m2_white);
/**
 * Evalutes the transpositions
 */
void evaluateTranspositions(std::vector<MatchEval> *games_eval, std::vector<Player> white_seekers, std::vector<Player> black_seekers, bool *error);
/**
 * Converts the match eval games to normal matches
 */
std::vector<Match> castToMatch(std::vector<MatchEval> games_eval);
/**
 * Finds the upfloaters for a group when the group is balanced in terms of colors
 */
std::vector<Player> findUpfloatersBalance(LinkedListNode next_group, LinkedListNode current_group, int max_rounds, int pairing_round);
/**
 * Gets a set of upfloaters
 */
std::vector<Player> trimFromUpfloaters(std::vector<Player> *upfloaters, int amount);
/**
 * Merges two groups of players
 */
void mergePlayers(std::vector<Player> *group_primary, std::vector<Player> players);
/**
 * Runs through the list of floaters an removes them from the lower groups
 */
LinkedListNode flushFloaters(LinkedListNode next_group, std::vector<Player> floaters_flush);
/**
 * Removes the bye player and returns the index of the bye player removed from
 */
int removeByePlayer(std::vector<Player> *players, int bye_remove, Player *p_bye);
/**
 * Optimizes the colors of the matches to make sure that all of criteria E is met
 */
void optimizeColors(std::vector<Match> *games);
}
}

/**
 * A simple tournament running Dubov system pairings
 */
class Tournament {
    
private:
    /**
     * Players in the tournament
     */
    std::vector<Player> players;
    /**
     * Number of players in tournament
     */
    int player_count = 0;
    /**
     * Total rounds in tournament
     */
    int total_rounds;
    /**
     * Determines if a pairing error occured
     */
    bool pairing_error = false;
    /**
     * ID of upfloater to remove
     */
    int upfloater_id = -1;
    /**
     * For merge sort (soring players by points)
     */
    static void playersPointsMerge(std::vector<Player> *players, int const left, int const mid, int const right);
    /**
     * For sorting players by points
     */
    static void sortPlayersPoints(std::vector<Player> *players, int const begin, int const end);
    /**
     * Makes round 1 pairings
     */
    std::vector<Match> makeRound1();
    /**
     * Makes the groups for the players
     */
    LinkedList makeGroups();
    /**
     * Makes a pairing for a group
     */
    std::vector<Match> makePairingForGroup(LinkedListNode *g, int pairing_round);
    /**
     * Makes subsequent round pairings
     */
    std::vector<Match> makeSubsequent(int pairing_round);
    /**
     * For getting matches of a specific round
     */
    std::vector<PlayerDivider::Utils::TRFMatch> extractedMatch;
    /**
     * A player for bye handling
     */
    int bye_player = -1;
    /**
     * The bye stack
     */
    std::vector<int> bye_queue;
    /**
     * Gets the index of the player who should get the bye
     */
    std::vector<int> getByePlayerStack(std::vector<Player> selection, int current_round);
    
public:
    Tournament(int total_rounds);
    
    /**
     * Adds a player to the list of players in the tournament
     */
    void addPlayer(Player p);
    /**
     * Gets all registered players in the tournament
     */
    std::vector<Player> getPlayers() {return players;}
    /**
     * A simple getter for pairing\_error
     */
    bool pairingErrorOccured(){return pairing_error;}
    /**
     * A simple getter for player count
     */
    int getPlayerCount(){return player_count;}
    /**
     * Generates pairings for a given round
     */
    std::vector<Match> generatePairings(int r);
    /**
     * Gets the raw matches extracted from a particular round
     */
    std::vector<PlayerDivider::Utils::TRFMatch> getRawMatches() {return extractedMatch;}
    /**
     * Sets the raw matches
     */
    void setRawMatch(std::vector<PlayerDivider::Utils::TRFMatch> s) {extractedMatch = s;}
    /**
     * Creates a tournament from TRF data
     */
    static Tournament makeTournament(TRFUtil::TRFData from_data, int *next_round, int stop_read = -1);
    /**
     * Simulates a tournament (random tournament generator)
     */
    static std::string simulateTournament(int p_count, int max_rounds);
};

#endif /* Tournament_hpp */
