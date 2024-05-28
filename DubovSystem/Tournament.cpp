//
//  Tournament.cpp
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

#include "Tournament.hpp"

MatchEval::MatchEval(Player white, Player black, bool problem): Match(white, black, false) {
    this->problem = problem;
}

void MatchEval::setProblem(bool p) {
    this->problem = p;
}

PlayerDivider::Utils::TRFMatch::TRFMatch(Player *white, Player *black, const double white_pts, const double black_pts, unsigned const int target_round) {
    this->white = white;
    this->black = black;
    this->white_pts = white_pts;
    this->black_pts = black_pts;
    this->target_round = target_round;
}

PlayerDivider::Utils::TRFMatch::TRFMatch() {
    this->white = NULL;
    this->black = NULL;
    this->white_pts = 0.0;
    this->black_pts = 0.0;
    this->target_round = -1;
}

void PlayerDivider::Utils::TRFMatch::distributePoints() {
//    this->white->addPoints(this->white_pts);
//    if(this->black != NULL) {
//        this->black->addPoints(this->black_pts);
//    }
    this->white->trf_pts += this->white_pts;
    this->black->trf_pts += this->black_pts;
}

void PlayerDivider::Utils::TRFMatch::distributeFloat() {
    if(this->black != NULL) {
        if(this->white->trf_pts > this->black->trf_pts) {
            this->black->incrementUpfloat();
            this->black->setUpfloatPrevStatus(true);
        } else if(this->white->trf_pts < this->black->trf_pts) {
            this->white->incrementUpfloat();
            this->white->setUpfloatPrevStatus(true);
        } else {
            this->white->setUpfloatPrevStatus(false);
            this->black->setUpfloatPrevStatus(false);
        }
    }
}

int PlayerDivider::Utils::TRFMatch::getWID() {
    if(this->white == NULL) {
        return -1;
    }
    return this->white->getID();
}

int PlayerDivider::Utils::TRFMatch::getBID() {
    if(this->black == NULL) {
        return -1;
    }
    return this->black->getID();
}

void PlayerDivider::mergeGroupARO(std::vector<Player> *group, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*group)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*group)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getARO() <= (*rightVector)[indexOfVectorTwo].getARO()) {
            if((*leftVector)[indexOfVectorOne].getARO() == (*rightVector)[indexOfVectorTwo].getARO()) {
                if((*leftVector)[indexOfVectorOne].getRating() < (*rightVector)[indexOfVectorTwo].getRating()) {
                    (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                    indexOfVectorOne += 1;
                } else {
                    (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
                    indexOfVectorTwo += 1;
                }
            } else {
                (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                indexOfVectorOne += 1;
            }
        } else {
            (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void PlayerDivider::mergeGroupAROG(std::vector<Player> *group, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*group)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*group)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getARO() >= (*rightVector)[indexOfVectorTwo].getARO()) {
            (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void PlayerDivider::Utils::mergeRawMatches(std::vector<PlayerDivider::Utils::TRFMatch> *raw_games, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<PlayerDivider::Utils::TRFMatch>(), *rightVector = new std::vector<PlayerDivider::Utils::TRFMatch>();
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*raw_games)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*raw_games)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getTargetRound() <= (*rightVector)[indexOfVectorTwo].getTargetRound()) {
            (*raw_games)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*raw_games)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*raw_games)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*raw_games)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void PlayerDivider::sortGroupARO(std::vector<Player> *group, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    PlayerDivider::sortGroupARO(group, begin, mid);
    PlayerDivider::sortGroupARO(group, mid + 1, end);
    PlayerDivider::mergeGroupARO(group, begin, mid, end);
}

void PlayerDivider::sortGroupAROG(std::vector<Player> *group, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    PlayerDivider::sortGroupAROG(group, begin, mid);
    PlayerDivider::sortGroupAROG(group, mid + 1, end);
    PlayerDivider::mergeGroupAROG(group, begin, mid, end);
}

void PlayerDivider::Utils::sortRawMatches(std::vector<PlayerDivider::Utils::TRFMatch> *raw_games, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    PlayerDivider::Utils::sortRawMatches(raw_games, begin, mid);
    PlayerDivider::Utils::sortRawMatches(raw_games, mid + 1, end);
    PlayerDivider::Utils::mergeRawMatches(raw_games, begin, mid, end);
}

void PlayerDivider::splitGroups(std::vector<Player> *white_seekers, std::vector<Player> *black_seekers, std::vector<Player> group) {
    for(int i = 0; i < group.size(); i++) {
        if(group[i].getDueColor() == MatchColor::Color::WHITE) {
            white_seekers->push_back(group[i]);
        } else {
            black_seekers->push_back(group[i]);
        }
    }
}

void PlayerDivider::pairGroup(std::vector<Player> white_seekers, std::vector<Player> black_seekers, std::vector<Match> *games) {
    for(int i = 0; i < ((int) white_seekers.size()); i++) {
        games->push_back(Match(white_seekers[i], black_seekers[i], false));
    }
}

void PlayerDivider::Utils::mergeMatches(std::vector<Match> m1, std::vector<Match> *main) {
    // merge both vectors into main
    for(int i = 0; i < m1.size(); i++) {
        main->push_back(m1[i]);
    }
}

void PlayerDivider::Utils::mergeUpfloaterWrappers(std::vector<Player> wrong_colors, std::vector<Player> max_upfloat, std::vector<Player> *upfloaters) {
    for(int i = 0; i < wrong_colors.size(); i++) {
        upfloaters->push_back(wrong_colors[i]);
    }
    
    for(int i = 0; i < max_upfloat.size(); i++) {
        upfloaters->push_back(max_upfloat[i]);
    }
}

LinkedListNode PlayerDivider::Utils::flushFloaters(LinkedListNode next_group, std::vector<Player> upfloaters) {
    // set up map for O(1) player finding
    std::map<int, bool> upfloaters_map;
    int marked = (int) upfloaters.size();
    for(int i = 0; i < upfloaters.size(); i++) {
        upfloaters_map[upfloaters[i].getID()] = true;
    }
    LinkedListNode *current = &next_group;
    while(current != NULL) {
        for(int i = 0; i < current->data.size(); i++) {
            if(upfloaters_map[current->data[i].getID()]) {
                upfloaters_map[current->data[i].getID()] = false;
                current->data.erase(std::next(current->data.begin(), i));
                marked -= 1;
                if(marked == 0) {
                    break;
                }
            }
        }
        if(marked == 0) {
            break;
        }
        current = current->next;
    }
    return next_group;
}

std::vector<Player> PlayerDivider::Utils::findUpfloaters(LinkedListNode next_group, int imbalance, int max_rounds, int pairing_round) {
    std::vector<Player> upfloaters;
    
    // look through sorted partition and evaluate with given score
    unsigned int current_score = 1;
    LinkedListNode* curr = &next_group;
    while(curr != NULL) {
        std::vector<Player> upfloater_max;
        std::vector<Player> wrong_color; // C.7 minimize players who don't get their color preference
        std::vector<Player> upfloated_previous; // C.10 minimize upfloaters who upfloated previously
        for(int i = 0; i < curr->data.size(); i++) {
            // we can ignore all the float history if it is the last round
            if(!curr->data[i].canUpfloat(max_rounds) && pairing_round != max_rounds) {
                upfloater_max.push_back(curr->data[i]);
                continue;
            }
            // C.10 minimize upfloaters who upfloated previously
            if(curr->data[i].upfloatedPreviously() && pairing_round != max_rounds) {
                upfloated_previous.push_back(curr->data[i]);
                continue;
            }
            if(imbalance == 1) {
                if(curr->data[i].getDueColor() == MatchColor::Color::WHITE) {
                    wrong_color.push_back(curr->data[i]);
                    continue;
                }
                upfloaters.push_back(curr->data[i]);
                current_score += 1;
            } else {
                if(curr->data[i].getDueColor() == MatchColor::Color::BLACK) {
                    wrong_color.push_back(curr->data[i]);
                    continue;
                }
                upfloaters.push_back(curr->data[i]);
                current_score += 1;
            }
        }
        
        // merge wrong color and max upfloaters with upfloaters
        PlayerDivider::Utils::mergeUpfloaterWrappers(wrong_color, upfloater_max, &upfloaters);
        // also merge float prev
        PlayerDivider::Utils::mergeUpfloaterWrappers(upfloated_previous, std::vector<Player>(), &upfloaters);
        
        // shift node
        curr = curr->next;
        
        // C.6 minimize score difference
    }
    
    return upfloaters;
}

std::vector<Player> PlayerDivider::Utils::findUpfloatersBalance(LinkedListNode next_group, LinkedListNode current_group, int max_rounds, int pairing_round) {
    std::vector<Player> upfloaters;
    
    LinkedListNode* temp = &next_group;
    while(temp != NULL) {
        std::vector<Player> upfloater_max;
        std::vector<Player> upfloated_previous;
        for(int i = 0; i < temp->data.size(); i++) {
            if(!temp->data[i].canUpfloat(max_rounds) && pairing_round != max_rounds) {
                upfloater_max.push_back(temp->data[i]);
                continue;
            }
            // C.10 minimize upfloaters who upfloated previously
            if(temp->data[i].upfloatedPreviously() && pairing_round != max_rounds) {
                upfloated_previous.push_back(temp->data[i]);
                continue;
            }
            
            upfloaters.push_back(temp->data[i]);
        }
        
        PlayerDivider::Utils::mergeUpfloaterWrappers(upfloated_previous, upfloater_max, &upfloaters);
        temp = temp->next;
    }
    
    // now evaluate all collected upfloaters to the group
    // make sure that at least one possible opponent exists
    for(int i = 0; i < upfloaters.size(); i++) {
        int num_invalid = 0;
        for(int z = 0; z < current_group.data.size(); z++) {
            if(!current_group.data[z].canPlayOpp(upfloaters[i])) {
                num_invalid += 1;
            }
            if(current_group.data[z].getDueColor() == upfloaters[i].getDueColor()) {
                if(current_group.data[z].getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE && upfloaters[i].getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE) {
                    num_invalid += 1;
                }
            }
        }
        
        if(num_invalid == ((int) current_group.data.size())) {
            // upfloater has no valid pair
            upfloaters.erase(std::next(upfloaters.begin(), i));
            i -= 1;
        }
    }
    
    return upfloaters;
}

bool PlayerDivider::Utils::isValidMatch(MatchEval m1, MatchEval m2) {
    if(!m1.white.canPlayOpp(m2.black)) {
        return false;
    }
    if(!m2.white.canPlayOpp(m1.black)) {
        return false;
    }
    return true;
}

int PlayerDivider::Utils::shiftIndex(MatchEval m1, int start, std::vector<MatchEval> games, int shift_count, bool *error) {
    if(start < 0) {
        *error = true;
        return 0;
    }
    while(start >= 0 && start < ((int) games.size())) {
//        if(PlayerDivider::Utils::isValidMatch(m1, games[start])) {
//            return start;
//        }
        if(m1.white.canPlayOpp(games[start].black)) {
            return start;
        }
        start += shift_count;
    }
    
    *error = true;
    return 0;
}

void PlayerDivider::Utils::swap(MatchEval *m1, MatchEval *m2, bool m1_white, bool m2_white) {
    if(m1_white && m2_white) {
        Player w1 = m1->white;
        m1->white = m2->white;
        m2->white=  w1;
    } else if(m1_white && !m2_white) {
        Player w1 = m1->white;
        m1->white = m2->black;
        m2->black = w1;
    } else if(!m1_white && m2_white) {
        Player b1 = m1->black;
        m1->black = m2->white;
        m2->white = b1;
    } else {
        Player b1 = m1->black;
        m1->black = m2->black;
        m2->black = b1;
    }
    m1->setProblem(false);
//    m2->setProblem(false);
    if(!m2->white.canPlayOpp(m2->black)) {
        m2->setProblem(true);
    } else {
        m2->setProblem(false);
    }
}

int PlayerDivider::Utils::shiftIndexExchange(std::vector<MatchEval> games, int start, int shift, int problem, bool *error) {
    int i = start;
    MatchEval p = games[problem];
    while(i >= 0 && i < ((int) games.size())) {
        // evaluate game
        if(games[i].black.canPlayOpp(p.black)) {
            if(games[i].white.canPlayOpp(p.white) && p.white.getPreferenceStrength() != MatchColor::ColorPreference::ABSOLUTE) {
                return i;
            }
        }
        i += shift;
    }
    *error = true;
    return -1;
}

int PlayerDivider::Utils::shiftIndexExchangeS1(std::vector<MatchEval> games, int start, int shift, int problem, bool *error) {
    int i = start;
    MatchEval p = games[problem];
    while(i >= 0 && i < ((int) games.size())) {
        // evaluate game
        if(games[i].white.canPlayOpp(p.white)) {
            if(games[i].black.canPlayOpp(p.black) && p.black.getPreferenceStrength() != MatchColor::ColorPreference::ABSOLUTE) {
                return i;
            }
        }
        i += shift;
    }
    *error = true;
    return -1;
}

int PlayerDivider::Utils::firstIndexExchange(std::vector<MatchEval> games, int problem, bool *error) {
    // go from bottom s1 to top
    if(games[problem].black.getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE) {
        // try s1 player
        int highest = 0;
        bool er = false;
        int higher_check = PlayerDivider::Utils::shiftIndexExchangeS1(games, highest, 1, problem, &er);
        if(er) {
            // no more possible exchanges
            *error = true;
            return -1;
        }
        return higher_check;
    }
    
    int lowest = ((int) games.size()) - 1;
    bool err = false;
    int lower_check = PlayerDivider::Utils::shiftIndexExchange(games, lowest, 1, problem, &err);
    if(err) {
        // try again but go up
        err = false;
        lower_check = PlayerDivider::Utils::shiftIndexExchange(games, lowest, -1, problem, &err);
        if(err) {
            *error = true; // no valid opponent exists
            return -1;
        }
    }
    
    // success
    return lowest;
}

void PlayerDivider::Utils::makeExchangeSwap(std::vector<MatchEval> *games, int problem_board, int target_board) {
    // first check what type of exchange this is
    if((*games)[problem_board].black.getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE) {
        // s1-s2 exchange
        Player b_swap =  (*games)[target_board].black;
        (*games)[target_board].black = (*games)[problem_board].white;
        (*games)[problem_board].white = b_swap;
        // unset both boards to no problem
        (*games)[target_board].setProblem(false);
        (*games)[problem_board].setProblem(false);
        return;
    }
    
    // s2-s1 exchange
    Player w_swap = (*games)[target_board].white;
    (*games)[target_board].white = (*games)[problem_board].black;
    (*games)[problem_board].black = w_swap;
    (*games)[target_board].setProblem(false);
    (*games)[problem_board].setProblem(false);
}

void PlayerDivider::Utils::maximizeExchanges(std::vector<MatchEval> *games_eval, bool *error) {
    // identify the problem boards and attempt to fix them by use of exchanges
    for(int i = 0; i < games_eval->size(); i++) {
        Player white = (*games_eval)[i].white;
        Player black = (*games_eval)[i].black;
        if((*games_eval)[i].isProblem()) {
            // check if we can move both these players in any way
            if(white.getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE && black.getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE) {
                // we can do no more from here
                *error = true;
                return;
            }
            
            //TODO: SEARCH EXCHANGES
            // try exchanges from s2-s1(bottom)
            bool err_on_find = false;
            int exchange = PlayerDivider::Utils::firstIndexExchange(*games_eval, i, &err_on_find);
            if(err_on_find) {
                // we failed, nothing more can be done
                *error = true;
                return;
            }
            
            // something was found, perform the swap
            PlayerDivider::Utils::makeExchangeSwap(games_eval, i, exchange);
        }
    }
}

void PlayerDivider::Utils::evaluateTranspositions(std::vector<MatchEval> *games_eval, std::vector<Player> white_seekers, std::vector<Player> black_seekers, bool *error) {
    // first find all the problem boards
    std::vector<int> problem_boards;
    for(int i = 0; i < games_eval->size(); i++) {
        if((*games_eval)[i].isProblem()) {
            problem_boards.push_back(i);
        }
    }
    
    if(problem_boards.size() == 0) {
        return;
    }
    
    // check how many boards we are at
    if(((int) games_eval->size()) == 1) {
        // we can't do anything with just one board!
        *error = true;
        return;
    }
    
    // apply transpositions in s2
    
    bool error_occured = false;
    
    for(int i = 0; i < problem_boards.size(); i++) {
        int problem_index = problem_boards[i];
        
        // check if this is the last index
        if(problem_index == ((int) games_eval->size()) - 1) {
            // check upper
            int first_index = PlayerDivider::Utils::shiftIndex((*games_eval)[problem_index], problem_index - 1, *games_eval, -1, &error_occured);
            if(error_occured) {
//                *error = true;
                //TODO: EXCHANGES
                // since no valid pairing exists, we must apply exchanges
                PlayerDivider::Utils::maximizeExchanges(games_eval, error);
                if(*error) {
                    // we can't do more from here
                    return;
                }
                continue;
            }
            
            // perform swap
            PlayerDivider::Utils::swap(&(*games_eval)[problem_index], &(*games_eval)[first_index], false, false);
            if(!(*games_eval)[first_index].white.canPlayOpp((*games_eval)[first_index].black)) {
                problem_boards.push_back(first_index);
            }
        } else {
            // check lower sets first than upper
            int fix_index = PlayerDivider::Utils::shiftIndex((*games_eval)[problem_index], problem_index + 1, *games_eval, 1, &error_occured);
            if(error_occured) {
                // try upper
                error_occured = false; // reset as needed
                fix_index = PlayerDivider::Utils::shiftIndex((*games_eval)[problem_index], problem_index - 1, *games_eval, -1, &error_occured);
                if(error_occured) {
                    //TODO: EXCHANGES
                    PlayerDivider::Utils::maximizeExchanges(games_eval, error);
                    if(*error) {
                        return;
                    }
                    continue;
                }
            }
            // perform swap
            PlayerDivider::Utils::swap(&(*games_eval)[problem_index], &(*games_eval)[fix_index], false, false);
            if(!(*games_eval)[fix_index].white.canPlayOpp((*games_eval)[fix_index].black)) {
                problem_boards.push_back(fix_index);
            }
        }
    }
}

std::vector<Match> PlayerDivider::Utils::castToMatch(std::vector<MatchEval> games_eval) {
    std::vector<Match> m;
    for(int i = 0; i < games_eval.size(); i++) {
        m.push_back(games_eval[i]);
    }
    
    // also perform optimization
    PlayerDivider::Utils::optimizeColors(&m);
    
    return m;
}

void PlayerDivider::Utils::migratePlayers(std::vector<Player> *g1, std::vector<Player> *g2, bool *error) {
    // sort by rating and move the first set of players from g2 to g1
    // g2 represents the larger group
    // since g2 should already be sorted, we go from bottom to top
//    int last = ((int) g2->size()) - 1;
    int num_moved = 0;
//    int amount_needed = ((int) g2->size()) - ((int) g1->size());
//    if(((int) g1->size()) == 0) {
//        amount_needed = ((int) g2->size()) / 2;
//    }
    
//    int i = last;
    int i = 0;
//    while(i >= 0 && g1->size() != g2->size()) {
    while(i < g2->size() && g1->size() != g2->size()) {
        // make sure we are actually allowed to move the player
        if((*g2)[i].getPreferenceStrength() == MatchColor::ColorPreference::ABSOLUTE) {
            // we cannot move this player
//            i -= 1;
            i += 1;
            continue;
        }
        // we can move this player
        Player move = (*g2)[i];
        g1->push_back(move);
        num_moved += 1;
        g2->erase(std::next(g2->begin(), i));
//        i -= 1;
        i += 1;
        
        // check if we reached our max
//        if(num_moved == amount_needed) {
//            break;
//        }
    }
    
//    for(int i = last; i >= 0; i--) {
//        
//    }
    
    // check if we accomplished the goal
//    if(num_moved < amount_needed) {
    if(g1->size() != g2->size()) {
        // we failed to make the move
        // move the players added back and return error
        int last_g1 = ((int) g1->size()) - 1;
        while(num_moved > 0) {
            Player move = (*g1)[last_g1];
            g2->push_back(move);
            g1->erase(std::next(g1->begin(), last_g1));
            last_g1 -= 1;
            num_moved -= 1;
        }
        *error = true;
    }
}

void PlayerDivider::Utils::migratePlayersNoColor(std::vector<Player> *g1, std::vector<Player> *g2) {
    // look for the players with no due color as much as possible
    for(int i = 0; i < g2->size(); i++) {
        if((*g2)[i].getNumColors() == 0) {
            // no due color!
            g1->push_back((*g2)[i]);
            g2->erase(std::next(g2->begin(), i));
            i -= 1;
            if(g1->size() == g2->size()) {
                break;
            }
        }
    }
    
    // also check players who are receiving the due color via alternation
    // but make sure group sizes are equal
//    if(g1->size() != g2->size()) {
//        for(int i = 0; i < g2->size(); i++) {
//            if((*g2)[i].getPreferenceStrength() == MatchColor::ColorPreference::ALTERNATION) {
//                g1->push_back((*g2)[i]);
//                g2->erase(std::next(g2->begin(), i));
//                i -= 1;
//                if(g1->size() == g2->size()) {
//                    break;
//                }
//            }
//        }
//    }
}

std::vector<Match> PlayerDivider::Utils::maximizePairings(std::vector<Player> white_seekers, std::vector<Player> black_seekers, bool *pairing_error) {
    std::vector<MatchEval> games;
    std::vector<int> board_problems;
    
    bool problem = false;
    
    // pair players while also keeping track of problems
    if(white_seekers.size() == black_seekers.size()) {
        for(int i = 0; i < white_seekers.size(); i++) {
//            games.push_back(Match(white_seekers[i], black_seekers[i], false));
            if(!white_seekers[i].canPlayOpp(black_seekers[i])) {
                problem = true;
                games.push_back(MatchEval(white_seekers[i], black_seekers[i], true));
                board_problems.push_back(((int) games.size()) - 1);
            } else {
                games.push_back(MatchEval(white_seekers[i], black_seekers[i], false));
            }
        }
        
        if(!problem) {
            // no problems we are good
            return PlayerDivider::Utils::castToMatch(games);
        }
        
        // reset games for transpositions
        bool error_occured = false;
        PlayerDivider::Utils::evaluateTranspositions(&games, white_seekers, black_seekers, &error_occured);
        if(error_occured) {
            (*pairing_error) = true;
        }
    } else {
        // we must attempt to move players from the bigger group to the smaller group
        // before migrating, check if we can migrate any players with no due colors
        bool error_on_move = false;
        if(white_seekers.size() > black_seekers.size()) {
            PlayerDivider::Utils::migratePlayersNoColor(&black_seekers, &white_seekers);
            if(white_seekers.size() > black_seekers.size()) {
                // we still need to move some players
                PlayerDivider::Utils::migratePlayers(&black_seekers, &white_seekers, &error_on_move);
            }
            PlayerDivider::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
        } else {
            PlayerDivider::Utils::migratePlayersNoColor(&white_seekers, &black_seekers);
            PlayerDivider::sortGroupAROG(&black_seekers, 0, ((int) black_seekers.size()) - 1);
            if(black_seekers.size() > white_seekers.size()) {
                PlayerDivider::Utils::migratePlayers(&white_seekers, &black_seekers, &error_on_move);
            }
            PlayerDivider::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            PlayerDivider::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            PlayerDivider::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
        }
        
        // check for errors
        if(error_on_move) {
            // we can do no more from here
            *pairing_error = true;
            return PlayerDivider::Utils::castToMatch(games);
        }
        
        // now that sizes are equal, we can re-evaluate
        // we can call this function recursively since the base case was touched
        return PlayerDivider::Utils::maximizePairings(white_seekers, black_seekers, pairing_error);
    }
    
    return PlayerDivider::Utils::castToMatch(games);
}

std::vector<Player> PlayerDivider::Utils::trimFromUpfloaters(std::vector<Player> *upfloaters, int amount) {
    std::vector<Player> trimmed;
    
    for(int i = 0; i < upfloaters->size(); i++) {
        if(((int) trimmed.size()) == amount) {
            break;
        }
        trimmed.push_back((*upfloaters)[0]);
        upfloaters->erase(upfloaters->begin());
        i -= 1;
    }
    
    return trimmed;
}

void PlayerDivider::Utils::mergePlayers(std::vector<Player> *group_primary, std::vector<Player> players) {
    for (int i = 0; i < players.size(); i++) {
        group_primary->push_back(players[i]);
    }
}

int PlayerDivider::Utils::removeByePlayer(std::vector<Player> *players, int bye_remove, Player *p_bye) {
    int r = -1;
    for(int i = 0; i < players->size(); i++) {
        if((*players)[i].getID() == bye_remove) {
            *p_bye = (*players)[i];
            players->erase(std::next(players->begin(), i));
            r = i;
            break;
        }
    }
    return r;
}

void PlayerDivider::Utils::optimizeColors(std::vector<Match> *games) {
    // since the program already makes sure that absolute criteria is met, just make sure equalization is preferred to alternation
    for(int i = 0; i < games->size(); i++) {
        if((*games)[i].is_bye) {
            continue;
        }
        MatchColor::Color gc = (*games)[i].white.getDueColor();
        if(gc == (*games)[i].black.getDueColor()) {
            MatchColor::ColorPreference wp = (*games)[i].white.getPreferenceStrength();
            MatchColor::ColorPreference bp = (*games)[i].black.getPreferenceStrength();
            if(wp == MatchColor::ColorPreference::ALTERNATION && bp == MatchColor::ColorPreference::MILD && gc == MatchColor::Color::WHITE) {
                // bp has higher priority
                (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
            } else if(wp == MatchColor::ColorPreference::MILD && bp == MatchColor::ColorPreference::ALTERNATION && gc == MatchColor::Color::BLACK) {
                (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
            }
        }
    }
}

Tournament::Tournament(int total_rounds) {
    this->total_rounds = total_rounds;
}

void Tournament::addPlayer(Player p) {
    this->player_count += 1;
    this->players.push_back(p);
}

Match::Match(Player white, Player black, bool is_bye) {
    this->is_bye = is_bye;
    this->white = white;
    this->black = black;
}

void PlayerDivider::playersRatingMerge(std::vector<Player> *players, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
//    leftVector->reserve(subVectorOne);
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*players)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*players)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getRating() >= (*rightVector)[indexOfVectorTwo].getRating()) {
            (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void Tournament::playersPointsMerge(std::vector<Player> *players, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
//    leftVector->reserve(subVectorOne);
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*players)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*players)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getPoints() >= (*rightVector)[indexOfVectorTwo].getPoints()) {
            (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void PlayerDivider::sortPlayersRating(std::vector<Player> *players, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    PlayerDivider::sortPlayersRating(players, begin, mid);
    PlayerDivider::sortPlayersRating(players, mid + 1, end);
    PlayerDivider::playersRatingMerge(players, begin, mid, end);
}

void Tournament::sortPlayersPoints(std::vector<Player> *players, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
//    this->sortPlayersPoints(players, begin, mid);
//    this->sortPlayersPoints(players, mid + 1, end);
//    this->playersPointsMerge(players, begin, mid, end);
    sortPlayersPoints(players, begin, mid);
    sortPlayersPoints(players, mid + 1, end);
    playersPointsMerge(players, begin, mid, end);
}

std::vector<int> Tournament::getByePlayerStack(std::vector<Player> selection, int current_round) {
    // find the player with the lowest rank, played the highest number of games, and has not scored a forfeit win or gotten a bye before
    int lowest = ((int) selection.size()) - 1;
    std::vector<int> candidate_player;
    std::vector<int> bye_queue;
    while(lowest >= 0) {
        // make sure player hasn't scored forfeit win and hasn't gotten bye before
        if(selection[lowest].hasReceievedBye()) {
            // invalid
            lowest -= 1;
            continue;
        }
        
        if(selection[lowest].getOppCount() < current_round - 1) {
            // hasn't played highest number of games, but is a candidate
            candidate_player.push_back(lowest);
            lowest -= 1;
            continue;
        }
        
        // player can get bye
        bye_queue.push_back(selection[lowest].getID());
        lowest -= 1;
    }
    
    // merge candidate players
    while(((int) candidate_player.size()) > 0) {
        bye_queue.push_back(candidate_player[0]);
        candidate_player.erase(candidate_player.begin());
    }
    
    // check sizes
    if(bye_queue.size() == 0) {
        // no player can get a bye
        throw std::invalid_argument("all players in tournament are not compatible for bye");
    }
    
    return bye_queue;
}

std::vector<Match> Tournament::makeRound1() {
    std::vector<Match> games;
    int middle = floor(this->player_count / 2);
    int opp = floor(this->player_count / 2);
    MatchColor::Color curr = MatchColor::Color::WHITE;
    PlayerDivider::sortPlayersRating(&this->players, 0, this->player_count - 1);
    for(int i = 0; i < middle; i++) {
        if(curr == MatchColor::Color::WHITE) {
            games.push_back(Match(this->players[i], this->players[opp], false));
            opp += 1;
            curr = MatchColor::Color::BLACK;
        } else {
            games.push_back(Match(this->players[opp], this->players[i], false));
            opp += 1;
            curr = MatchColor::Color::WHITE;
        }
    }
    if(this->player_count % 2 > 0) {
        games.push_back(Match(this->players[this->player_count - 1], Player("", 0, -1, 0.0), true));
    }
    return games;
}

LinkedList Tournament::makeGroups() {
    LinkedList groups;
    // it is assumed that players are already sorted
    std::vector<Player> group;
    group.push_back(this->players[0]);
    double curr_points = this->players[0].getPoints();
    
    for(int i = 1; i < this->player_count; i++) {
        if(this->players[i].getPoints() != curr_points) {
            groups.insertNode(LinkedListNode(group));
            group.clear();
            group.push_back(this->players[i]);
            curr_points = this->players[i].getPoints();
            continue;
        }
        group.push_back(this->players[i]);
    }
    groups.insertNode(LinkedListNode(group));
    
    return groups;
}

std::vector<Match> Tournament::makePairingForGroup(LinkedListNode *g, int pairing_round) {
    if(g == NULL) {
        // base case
        return std::vector<Match>();
    }
    std::vector<Match> games;
    Player* upfloater = NULL; // for storing upfloater in backtracking
    // first check for any upfloaters to remove
    if(this->upfloater_id > -1) {
        // locate upfloater in group (if exists) and remove from pairing selection
        int remove_index = -1;
        for(int i = 0; i < g->data.size(); i++) {
            if(g->data[i].getID() == this->upfloater_id) {
                // target found, remove
                remove_index = i;
                break;
            }
        }
        // check if upfloater exists in this group
        if(remove_index > -1) {
            // exists now remove
            upfloater = &g->data[remove_index];
            g->data.erase(std::next(g->data.begin(), remove_index));
            this->upfloater_id = -1;
        }
    }
    
    std::vector<Player> white_seekers;
    std::vector<Player> black_seekers;
    std::vector<Player> upfloaters;
    std::vector<Player> upfloaters_multi;
    bool floater_required = false;
    
    // make groups
    PlayerDivider::splitGroups(&white_seekers, &black_seekers, g->data);
    
    PlayerDivider::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
    PlayerDivider::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
    
    int num_white = (int) white_seekers.size();
    int num_black = (int) black_seekers.size();
    
    // now check if upfloater is needed for this group in order to complete pairings
    if(g->data.size() % 2 > 0) {
        //TODO: FIND UPFLOATER
        // find upfloater that satisfies pairings
        upfloaters = PlayerDivider::Utils::findUpfloaters(*g->next, num_white - num_black, this->total_rounds, pairing_round);
        
        // make sure there is at least one valid upfloater present
        if(upfloaters.size() == 0) {
            this->pairing_error = true;
            return games;
        }
        floater_required = true;
    }
    
    // attempt to pair the group
    bool pair_complete = false;
    bool pairing_failure_upfloaters = false;
    std::vector<Player> upfloaters_move_container; // for storing tried upfloaters
    unsigned int max_upfloater_move = 0;
    while(!pair_complete) {
        // make copies as needed
        std::vector<Player> w_copy = std::vector<Player>(white_seekers);
        std::vector<Player> b_copy = std::vector<Player>(black_seekers);
        LinkedListNode *next_group_use = g->next;
        // check if we have any floaters
        if(floater_required) {
            if(upfloaters.size() == 0) {
                // no floaters left
                this->pairing_error = true;
                return games;
            }
            
            // dequeue the floater and add to list
//            Player floater = *upfloaters.erase(upfloaters.begin());
            Player floater = upfloaters[0];
            upfloaters.erase(upfloaters.begin());
            this->upfloater_id = floater.getID();
            if(floater.getDueColor() == MatchColor::Color::WHITE) {
                w_copy.push_back(floater);
                // re-sort white seekers as needed
                PlayerDivider::sortGroupARO(&w_copy, 0, ((int) w_copy.size()) - 1);
            } else {
                b_copy.push_back(floater);
                // re-sort black seekers as needed
                PlayerDivider::sortPlayersRating(&b_copy, 0, ((int) b_copy.size()) - 1);
            }
        }
        
        if(pairing_failure_upfloaters) {
            if(upfloaters.size() == 0) {
                // migraate containers and try higher numbers
                upfloaters = upfloaters_move_container;
                upfloaters_move_container.clear();
                max_upfloater_move += 2; // minimize number of upfloaters
                // check if our number completely exceeds the number of available upfloaters
                if(max_upfloater_move > ((int) upfloaters.size())) {
                    // we have exhausted all possible pairings
                    this->pairing_error = true;
                    break;
                }
            }
            
            // force upfloater players
            std::vector<Player> upfloaters_trimmed = PlayerDivider::Utils::trimFromUpfloaters(&upfloaters, max_upfloater_move);
            // add these to the other container
            PlayerDivider::Utils::mergePlayers(&upfloaters_move_container, upfloaters_trimmed);
            // now merge these upfloaters with this group and recreate the match ups
            LinkedListNode new_groups = PlayerDivider::Utils::flushFloaters(*(g->next), upfloaters_trimmed);
            g->next = &new_groups;
            std::vector<Player> p_test = std::vector<Player>(g->data);
            PlayerDivider::Utils::mergePlayers(&p_test, upfloaters_trimmed);
            // split up groups as usual
            w_copy.clear();
            b_copy.clear();
            PlayerDivider::splitGroups(&w_copy, &b_copy, p_test);
            PlayerDivider::sortGroupARO(&w_copy, 0, ((int) w_copy.size()) - 1);
            PlayerDivider::sortPlayersRating(&b_copy, 0, ((int) b_copy.size()) - 1);
        }
        
        // attempt to pair the groups
        bool error_on_pair = false;
        std::vector<Match> games_got = PlayerDivider::Utils::maximizePairings(w_copy, b_copy, &error_on_pair);
        
        // check for errors
        if(error_on_pair) {
            if(floater_required) {
                // try another floater
                this->pairing_error = false; // reset as necessary
                continue;
            }
            // we must extract the same number of upfloaters as there are in the group
            if(upfloaters.size() == 0) {
                // generate them
                upfloaters = PlayerDivider::Utils::findUpfloatersBalance(*(g->next), *g, this->total_rounds, pairing_round);
                //TODO: UPFLOATERS
                pairing_failure_upfloaters = true;
                continue;
            }
            continue;
        }
        
        // merge games
        PlayerDivider::Utils::mergeMatches(games_got, &games);
        
        // try pairing next group lower
//        std::vector<Match> lower = this->makePairingForGroup(g->next, pairing_round);
        std::vector<Match> lower = this->makePairingForGroup(next_group_use, pairing_round);
        
        // check for errors as necessary
        if(this->pairing_error) {
            //TODO: PAIRING ERROR
            // check if this section required a floater previously
            if(floater_required) {
                // we can try another floater, so continue
                this->pairing_error = false;
                continue;
            }
            
//             we need to extract some floaters to complete the pairing
//             C.5 states to minimize the number of upfloaters
//             so gather the next to available upfloaters
            if(upfloaters.empty()) {
                upfloaters = PlayerDivider::Utils::findUpfloatersBalance(*g->next, *g, this->total_rounds, pairing_round);
                pairing_failure_upfloaters = true;
                continue;
            }
            
//             make sure we still have some upfloaters left
//            if(((int) upfloaters.size()) == 1) {
//                // this is a rare case, but if we reach it, we don't have enough upfloaters to satisfy the pairing
//                break;
//            }
            
            if(pairing_failure_upfloaters) {
                // we can continue as upfloaters are already up
                continue;
            }
        }
        
        // merge
        PlayerDivider::Utils::mergeMatches(lower, &games);
        
        pair_complete = true; // mark pairing complete
        this->pairing_error = false;
    }
    
    return games;
}

std::vector<Match> Tournament::makeSubsequent(int pairing_round) {
    std::vector<Match> games;
    // sort players
    PlayerDivider::sortPlayersRating(&this->players, 0, this->player_count - 1);
    this->sortPlayersPoints(&this->players, 0, this->player_count - 1);
    
    // handle byes as necessary
    if(((int) this->players.size()) % 2 > 0) {
        this->bye_queue = this->getByePlayerStack(this->players, pairing_round);
    }
    
    // make groups
    LinkedList groups = this->makeGroups();
    
    // pair each group
    LinkedListNode* curr = groups.getHead();
    /*while(curr != NULL) {
        std::vector<Player> white_seekers;
        std::vector<Player> black_seekers;
        
        // split groups
        PlayerDivider::splitGroups(&white_seekers, &black_seekers, curr->data);
        
        // check if there are an odd number of players in the group
        if(((int) curr->data.size()) % 2 > 0) {
            // find upfloater that allows completion of pairing in group
        }
        
        PlayerDivider::sortGroupARO(white_seekers, 0, ((int) white_seekers.size()) - 1);
        this->sortPlayersRating(black_seekers, 0, ((int) black_seekers.size()) - 1);
        
        if(white_seekers.size() == black_seekers.size()) {
            PlayerDivider::pairGroup(white_seekers, black_seekers, &games);
            curr = curr->next;
            continue;
        }
    }*/
    
    if(this->bye_queue.size() > 0) {
        // since just about anybody can get a bye, we need to constantly keep trying to give different players the bye until we get a valid set of pairings
        while(this->bye_queue.size() > 0) {
            // remove bye player
            Player p_bye;
            int bye_p = PlayerDivider::Utils::removeByePlayer(&this->players, this->bye_queue[0], &p_bye);
            this->bye_queue.erase(this->bye_queue.begin());
            this->player_count -= 1;
            groups = this->makeGroups();
            curr = groups.getHead();
            games = this->makePairingForGroup(curr, pairing_round);
            
            // check for errors
            if(this->pairing_error) {
                // put bye player back and try dequeing another bye player
                this->players.insert(std::next(this->players.begin(), bye_p), p_bye);
                this->pairing_error = false;
                games.clear();
                continue;
            }
            
            // we must have had success
            games.push_back(Match(p_bye, Player("", 0, -1, 0.0), true));
            break;
        }
        
        // check if we got at least one pairing
        if(games.size() == 0) {
            // we failed to find a good pairing
            this->pairing_error = true;
        }
    } else {
        games = this->makePairingForGroup(curr, pairing_round);
    }
    
    return games;
}

std::vector<Match> Tournament::generatePairings(int r) {
    if(r == 1) {
        return this->makeRound1();
    }
    return this->makeSubsequent(r);
}

Tournament Tournament::makeTournament(TRFUtil::TRFData from_data, int *next_round, int stop_read) {
//    Tournament t_main(from_data.getRounds());
    Tournament t_main(from_data.getRoundsTnr());
    
    // add players
    std::vector<std::map<std::string, std::string>> players = from_data.getPlayerSection();
    
    std::string valid_res[13] = {"+", "-", "w", "d", "l", "1", "=", "0", "h", "f", "u", "z", " "}; // valid match results
    std::map<std::string, double> res_pt;
    res_pt["+"] = 1.0;
    res_pt["-"] = 0.0;
    res_pt["="] = 0.5;
    res_pt["w"] = 1.0;
    res_pt["1"] = 1.0;
    res_pt["0"] = 0.0;
    res_pt["d"] = 0.5;
    res_pt["h"] = 0.5;
    res_pt["f"] = 1.0;
    res_pt["u"] = 1.0;
    res_pt["z"] = 0.0;
    res_pt[" "] = 0.0;
    
    *next_round = from_data.getRounds();
    
    std::map<int, int> opp_ratings;
    std::map<int, double> float_info;
    
    std::vector<Player> players_list;
    int player_count = 0;
    
    std::map<std::string, int> match_eval_loc;
    
    std::vector<PlayerDivider::Utils::TRFMatch> match_eval; // for distributing floats
    std::vector<PlayerDivider::Utils::TRFMatch> round_capture; // for capturing rounds
    std::map<int, bool> r_skip; // for capturing rounds
    std::map<std::string, bool> r_skip_hash;
    std::map<int, int> player_pos; // for easy player locating
    
    Player *p_add;
    
    for(int i = 0; i < players.size(); i++) {
        int id = std::stoi(players[i]["rank"]);
        std::string name = players[i]["name"];
        int rating = std::stoi(players[i]["rating"]);
        double points = std::stod(players[i]["points"]);
        
        opp_ratings[id] = rating;
        
//        Player p_add(name, rating, id, points);
        p_add = new Player(name, rating, id, points);
        
        // render results
        for(int z = 1; z <= from_data.getRounds(); z++) {
            // make sure we don't go further than stop read if set
            if(stop_read > -1 && z > stop_read) {
                break;
            }
            std::string as_str = std::to_string(z);
            // check bye
            std::string opp = players[i]["r" + as_str + "_opp"];
            if(opp == "000") {
                // bye
                p_add->setByeStatus(true);
                std::string resb = players[i]["r" + as_str + "_res"];
                PlayerDivider::Utils::TRFMatch tt = PlayerDivider::Utils::TRFMatch(p_add, NULL, res_pt[resb], 0.0, z);
                tt.is_bye = true;
                match_eval.push_back(tt);
                continue;
            }
            int opp_id = std::stoi(opp);
            
            // expect color
            std::string color = players[i]["r" + as_str + "_color"];
            if(color != "w" && color != "b" && color != "-" && color != " ") {
                throw std::invalid_argument("unexpected character given for color for non bye player. Expected 'w' or 'b' but got " + color);
            }
            
            // expect result
            std::string res = players[i]["r" + as_str + "_res"];
            
            char res_as_char = res.at(0);
            res_as_char = std::tolower(res_as_char);
            res = res_as_char;
            
            // make sure result is valid
            if(std::find(std::begin(valid_res), std::end(valid_res), res) == std::end(valid_res)) {
                throw std::invalid_argument("no such result '" + res + "' appears to exist for result of match");
            }
            
            // now that everything is valid we can add it to the list
            // just make sure res is not forfeit
            if(res == "-" || res == "+" || res == "h" || res == "f" || res == "u" || res == "z" || res == " ") {
                if(res == "-" || res == "+") {
                    if(color == "w") {
                        std::string forfeit_raw_hash = players[i]["rank"] + "_" + as_str + "_" + opp;
                        if(match_eval_loc.find(forfeit_raw_hash) != match_eval_loc.end()) {
                            match_eval[match_eval_loc[forfeit_raw_hash]].setWhite(p_add);
                            match_eval[match_eval_loc[forfeit_raw_hash]].setWhitePts(res_pt[res]);
                            if(z == stop_read) {
                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
                                r_skip_hash[forfeit_raw_hash] = true;
                            }
                            continue;
                        } else {
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlack(p_add);
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlackPts(res_pt[res]);
                            if(z == stop_read) {
                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
                                r_skip_hash[forfeit_raw_hash] = true;
                            }
                        }
                        match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
                        r_skip_hash[forfeit_raw_hash] = false;
                        match_eval.push_back(PlayerDivider::Utils::TRFMatch(p_add, NULL, res_pt[res], 0, z));
                    } else {
                        std::string forfeit_raw_hash = opp + "_" + as_str + "_" + players[i]["rank"];
                        if(match_eval_loc.find(forfeit_raw_hash) != match_eval_loc.end()) {
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlack(p_add);
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlackPts(res_pt[res]);
                            if(z == stop_read) {
                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
                                r_skip_hash[forfeit_raw_hash] = true;
                            }
                            continue;
                        } else {
                            match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
                            r_skip_hash[forfeit_raw_hash] = false;
                            match_eval.push_back(PlayerDivider::Utils::TRFMatch(NULL, p_add, 0.0, res_pt[res], z));
                        }
                        match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
                        r_skip[match_eval_loc[forfeit_raw_hash]] = false;
                        r_skip_hash[forfeit_raw_hash] = false;
                        match_eval.push_back(PlayerDivider::Utils::TRFMatch(NULL, p_add, 0, res_pt[res], z));
                    }
                }
                continue;
            }
            
            std::string raw_match_hash = "";
            
//            p_add->addOpp(opp_id);
            if(stop_read == -1) {
                p_add->addOpp(opp_id);
            } else {
                if(z < stop_read) {
                    p_add->addOpp(opp_id);
                }
            }
            if(color == "w") {
                if(stop_read == -1) {
                    p_add->addColor(MatchColor::Color::WHITE);
                } else {
                    if(z < stop_read) {
                        p_add->addColor(MatchColor::Color::WHITE);
                    }
                }
                raw_match_hash = players[i]["rank"] + "_" + as_str + "_" + opp;
                if(match_eval_loc.find(raw_match_hash) != match_eval_loc.end()) {
                    match_eval[match_eval_loc[raw_match_hash]].setWhite(p_add);
                    match_eval[match_eval_loc[raw_match_hash]].setWhitePts(res_pt[res]);
                    if(z == stop_read) {
                        round_capture.push_back(match_eval[match_eval_loc[raw_match_hash]]);
//                        r_skip[match_eval_loc[raw_match_hash]] = true;
                        r_skip_hash[raw_match_hash] = true;
                    }
                    continue;
                }
                match_eval_loc[raw_match_hash] = (int) match_eval.size();
                r_skip[match_eval_loc[raw_match_hash]] = false;
                r_skip_hash[raw_match_hash] = false;
                match_eval.push_back(PlayerDivider::Utils::TRFMatch(p_add, NULL, res_pt[res], 0, z));
            } else {
                if(stop_read == -1) {
                    p_add->addColor(MatchColor::Color::BLACK);
                } else {
                    if(z < stop_read) {
                        p_add->addColor(MatchColor::Color::BLACK);
                    }
                }
                raw_match_hash = opp + "_" + as_str + "_" + players[i]["rank"];
                if(match_eval_loc.find(raw_match_hash) != match_eval_loc.end()) {
                    match_eval[match_eval_loc[raw_match_hash]].setBlack(p_add);
                    match_eval[match_eval_loc[raw_match_hash]].setBlackPts(res_pt[res]);
                    if(z == stop_read) {
                        round_capture.push_back(match_eval[match_eval_loc[raw_match_hash]]);
//                        r_skip[match_eval_loc[raw_match_hash]] = true;
                        r_skip_hash[raw_match_hash] = true;
                    }
                    continue;
                }
                match_eval_loc[raw_match_hash] = (int) match_eval.size();
                r_skip[match_eval_loc[raw_match_hash]] = false;
                r_skip_hash[raw_match_hash] = false;
                match_eval.push_back(PlayerDivider::Utils::TRFMatch(NULL, p_add, 0, res_pt[res], z));
            }
        }
        
        // add player
//        t_main.addPlayer(p_add);
        player_pos[p_add->getID()] = (int) players_list.size();
        players_list.push_back(*p_add);
        player_count += 1;
    }
    
    PlayerDivider::Utils::sortRawMatches(&match_eval, 0, ((int) match_eval.size()) - 1);
    
    // set up floats
    for(int i = 0; i < match_eval.size(); i++) {
        // check if we are skipping
        std::string mh = std::to_string(match_eval[i].getWID()) + "_" + std::to_string(match_eval[i].getTargetRound()) + "_" + std::to_string(match_eval[i].getBID());
//        if(r_skip[i]) {
//            continue;
//        }
        if(r_skip_hash[mh]) {
            continue;
        }
        if(match_eval[i].is_bye) {
            int wib = match_eval[i].getWID();
            int wbw = player_pos[wib];
            players_list[wbw].trf_pts += match_eval[i].white_pts;
            continue;
        }
//        match_eval[i].distributeFloat();
//        match_eval[i].distributePoints();
        //TODO: BYE INF
        int wi = match_eval[i].getWID();
        int bi = match_eval[i].getBID();
        int w = player_pos[wi];
        int b = player_pos[bi];
//        players_list[w] = match_eval[i].getWhite();
//        players_list[b] = match_eval[i].getBlack();
        if(players_list[w].trf_pts > players_list[b].trf_pts) {
            players_list[b].incrementUpfloat();
            players_list[b].setUpfloatPrevStatus(true);
        } else if(players_list[w].trf_pts < players_list[b].trf_pts) {
            players_list[w].incrementUpfloat();
            players_list[w].setUpfloatPrevStatus(true);
        } else {
            players_list[w].setUpfloatPrevStatus(false);
            players_list[b].setUpfloatPrevStatus(false);
        }
        players_list[w].trf_pts += match_eval[i].white_pts;
        players_list[b].trf_pts += match_eval[i].black_pts;
    }
    
    // add opponent ratings to all players
    for(int i = 0; i < player_count; i++) {
        std::vector<int> opp_p = players_list[i].getOppPlayed();
        for(int z = 0; z < players_list[i].getOppCount(); z++) {
            players_list[i].addOppRating(opp_ratings[opp_p[z]]);
        }
        
        // set players points to trf points if stop_read > -1
        if(stop_read > -1) {
            double pt = players_list[i].getPoints();
            double trfpt = players_list[i].trf_pts;
            double add = trfpt - pt;
            players_list[i].addPoints(add);
        }
        
        // add to list
        t_main.addPlayer(players_list[i]);
    }
    
    // set raw capture as necessary
    t_main.setRawMatch(round_capture);
    
    return t_main;
}

std::string Tournament::simulateTournament(int p_count, int max_round) {
    
    std::map<int, int> player_raw_loc;
    std::vector<RandomTournamentGenerator::RawPlayer> player_raw;
    std::vector<Player> normal_player;
    
    std::vector<std::string> match_res;
    match_res.push_back("1");
    match_res.push_back("0");
    match_res.push_back("=");
    match_res.push_back("-");
    match_res.push_back("+");
    
    // highest and lowest rating allowed to be set
    int highest_rating = 3000;
    int lowest_rating = 100;
    
    // add nth players to list and simulate tournament until max_round
    for(int i = 1; i <= p_count; i++) {
        player_raw_loc[i] = (int) player_raw.size();
        
        int rating = (rand() % (highest_rating - lowest_rating + 1) + lowest_rating); // random rating
        
        // add player to both raw and normal
        normal_player.push_back(Player("PLAYER " + std::to_string(i), rating, i, 0.0));
        player_raw.push_back(RandomTournamentGenerator::RawPlayer("PLAYER " + std::to_string(i), rating, i));
    }
    
    // begin simulation by generating round by round
    for(int i = 1; i <= max_round; i++) {
        // make a new tournament and add out player list into that tournament
        Tournament simulated_tournament(max_round);
        
        for(int z = 0; z < normal_player.size(); z++) {
            simulated_tournament.addPlayer(normal_player[z]);
        }
        
        // generate pairings
        std::vector<Match> simulated_output = simulated_tournament.generatePairings(i);
        
        // make sure of course there are no errors
        if(simulated_tournament.pairingErrorOccured()) {
            // hopefully this is unlikely to happen!
            break;
        }
        
        // apply random results
        // then apply those results to the players
        for(int z = 0; z < simulated_output.size(); z++) {
            Player wg = simulated_output[z].white;
            int w_loc = player_raw_loc[wg.getID()];
            
            // check bye inf
            if(simulated_output[z].is_bye) {
                Player p_repw(wg.getName(), wg.getRating(), wg.getID(), wg.getPoints() + 1.0);
                p_repw.setByeStatus(true);
                normal_player[w_loc] = p_repw;
                
                // also reflect in raw
                player_raw[w_loc].incrementRawPoints(1.0);
                player_raw[w_loc].appendRawGame("0000 - +");
                continue;
            }
            
            Player bg = simulated_output[z].black;
            int b_loc = player_raw_loc[bg.getID()];
            
            // recreate player in the same way as bye
            int white_rand_res = (rand() % 5);
            
            switch (white_rand_res) {
                case 0:
                    normal_player[w_loc].addPoints(1.0);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(MatchColor::Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(wg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(MatchColor::Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b 0");
                    break;
                
                case 1:
                    normal_player[b_loc].addPoints(1.0);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(MatchColor::Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(wg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(MatchColor::Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[b_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b 1");
                    break;
                    
                case 2:
                    normal_player[w_loc].addPoints(0.5);
                    normal_player[b_loc].addPoints(0.5);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(MatchColor::Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(wg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(MatchColor::Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(0.5);
                    player_raw[b_loc].incrementRawPoints(0.5);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b =");
                    break;
                    
                case 3:
                    normal_player[w_loc].addPoints(1.0);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w +");
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b -");
                    break;
                    
                case 4:
                    normal_player[b_loc].addPoints(1.0);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w -");
                    
                    // also reflect in raw
                    player_raw[b_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b +");
                    break;
                default:
                    break;
            }
        }
    }
    
    // now that the simulation of rounds is done, sort the players and insert their final rankings
    PlayerDivider::sortPlayersRating(&normal_player, 0, ((int) normal_player.size()) - 1);
    sortPlayersPoints(&normal_player, 0, ((int) normal_player.size()) - 1);
    
    for(int i = 0; i < normal_player.size(); i++) {
        int p = player_raw_loc[normal_player[i].getID()];
        
        player_raw[p].setFinalRank(i + 1);
    }
    
    // now write up the trf data with simulated tournament
    return RandomTournamentGenerator::trfString(p_count, max_round, player_raw);
}
