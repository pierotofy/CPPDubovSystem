//
//  trf.cpp
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

#include "trf.hpp"
#include <fstream>

// https://www.fide.com/FIDE/handbook/C04Annex2_TRF16.pdf

void TRFUtil::clearSpaces(std::string *str_clear) {
    std::string str_rep = "";
    for(int i = 0; i < str_clear->size(); i++) {
        std::string ss = str_clear->substr(i, 1);
        if(ss == " ") {
            continue;
        }
        str_rep += ss;
    }
    *str_clear = str_rep;
}

void TRFUtil::TRFData::parsePlayer(std::string line) {
    // we assume the 001 is not present
    
    // make sure at least 89 characters are present
    if(((int) line.size()) < 89) {
        throw std::length_error("player length is not long enough");
    }
    
    std::map<std::string, std::string> player_data;
    
    // pos 5-8 ranking
    player_data["rank"] = line.substr(4, 4);
    TRFUtil::clearSpaces(&player_data["rank"]);
    
    // pos 10 sex
    player_data["sex"] = line.substr(8, 1);
    
    // pos 11-13 title
    player_data["title"] = line.substr(9, 2);
    
    // pos 15-47 name
    player_data["name"] = line.substr(14, 32);
    // remove any spaces from name
//    std::erase(player_data["name"], " ");
    TRFUtil::clearSpaces(&player_data["name"]);
    
    // pos 49-52 rating
    player_data["rating"] = line.substr(48, 4);
    // also remove spaces
//    std::erase(player_data["rating"], " ");
    TRFUtil::clearSpaces(&player_data["rating"]);
    
    // pos 54-56 fed
    player_data["fed"] = line.substr(53, 2);
    
    // pos 58-68 id
    player_data["id"] = line.substr(57, 10);
    
    // 70-79 player birthdate
    player_data["bday"] = line.substr(69, 9);
    
    // pos 81-84 points
    player_data["points"] = line.substr(80, 4);
//    std::erase(player_data["points"], " ");
    TRFUtil::clearSpaces(&player_data["points"]);
    
    // pos 86-89 rank
    player_data["rankpos"] = line.substr(85, 3);
//    std::erase(player_data["rank"], " ");
    TRFUtil::clearSpaces(&player_data["rankpos"]);
    
    if(((int) line.size()) > 89) {
        // pos 92-n results & pairings
        
        int curr_pos = 92;
//        int curr_pos = 91;
        int r_rec = 1;
        while(curr_pos < ((int) line.size())) {
            std::string r_rec_str = std::to_string(r_rec);
            // n-n+5 opp rank
            player_data["r" + r_rec_str + "_opp"] = line.substr(curr_pos, 3); // ORIGINAL 3
            TRFUtil::clearSpaces(&player_data["r" + r_rec_str + "_opp"]);
            
            curr_pos += 4;
            
            // n+6 color | bye res
            player_data["r" + r_rec_str + "_color"] = line.substr(curr_pos, 1);
            curr_pos += 2;
            
            // n+7 result
            player_data["r" + r_rec_str + "_res"] = line.substr(curr_pos, 1);
            curr_pos += 4;
            
            r_rec += 1;
        }
        
        this->rounds_captured = r_rec - 1;
    }
    
    // add data
    this->player_section.push_back(player_data);
}

void TRFUtil::TRFData::parseLine(std::string line) {
    // make sure we have at least 3 chars
    if(((int) line.size()) == 0) {
        return;
    }
    if(((int) line.size()) < 3) {
        throw std::length_error("invalid input given for line");
    }
    // we start with a identification number (first 3 chars)
    std::string line_id = line.substr(0, 3);
    std::map<std::string, std::string> keys;
    keys["012"] = "name";
    keys["022"] = "city";
    keys["032"] = "fed";
    keys["042"] = "start";
    keys["052"] = "end";
    keys["062"] = "player_num";
    keys["072"] = "num_players_rated";
    keys["082"] = "team_num";
    keys["092"] = "type";
    keys["102"] = "arbiter";
    keys["112"] = "deputy_arbiter";
    keys["122"] = "time_control";
    keys["132"] = "dates";
    keys["001"] = "players";
    keys["XXR"] = "extra";
    keys["TNR"] = "rounds";
    
    // make sure line id is valid
    if(keys.find(line_id) == keys.end()) {
        throw std::invalid_argument("Unidentified line " + line_id + " detected");
    }
    
    // we will deal with players separatly
    if(line_id == "001") {
//        this->parsePlayer(line.substr(3));
        this->parsePlayer(line);
        return;
    } else if(line_id == "TNR") {
        this->rounds_tnr = std::stoi(line.substr(3));
    }
    
    // record into table
    this->tournament_section[line_id] = line.substr(3);
}

TRFUtil::TRFFile::TRFFile(const std::string path) {
    this->path = path;
}

TRFUtil::TRFData TRFUtil::TRFFile::read() {
    std::ifstream txt(this->path);
    
    std::string txt_get;
    
    TRFData d;
    
    while (std::getline(txt, txt_get)) {
        d.parseLine(txt_get);
    }
    
    txt.close();
    
    return d;
}

void TRFUtil::TRFFile::write(std::string trf) {
    std::ofstream writer(this->path);
    
    writer << trf;
    
    writer.close();
}
