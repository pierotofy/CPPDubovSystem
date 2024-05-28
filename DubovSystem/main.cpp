//
//  main.cpp
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

#include <iostream>
#include "trf util/trf.hpp"
#include "Tournament.hpp"
#include "trf util/rtg.hpp"
#include "fpc.hpp"

#define CPPDUBOVSYSTEM_VERSION 1.0

/**
 * Help info
 */
void helpDisplay() {
    std::cout << "--------FIDE DUBOV SYSTEM PAIRING ENGINE--------" << std::endl;
    std::cout << "--------------------Version 1.0-----------------" << std::endl;
    std::cout << "---------------Author: Michael Shapiro----------" << std::endl;
    std::cout << "-------------------Help Section-----------------" << std::endl;
    std::cout << "\n\n\n\n";
    std::cout << "|Argument     |Description                  |" << std::endl;
    std::cout << "|-------------|-----------------------------|" << std::endl;
    std::cout << "|--help       |Help Page                    |" << std::endl;
    std::cout << "|--pairings   |Do next round from given file|" << std::endl;
    std::cout << "|--sample     |Example Usage                |" << std::endl;
    std::cout << "|--version    |Output Version               |" << std::endl;
    std::cout << "|--rtg        |Random tournament generator  |" << std::endl;
    std::cout << "|--p_count    |(for rtg) player count       |" << std::endl;
    std::cout << "|--rtg_rounds |(for rtg) total rounds       |" << std::endl;
    std::cout << "|--fpc        |Free Pairings Checker        |" << std::endl;
    std::cout << "|--fpc_rounds |(for fpc) Round to check     |" << std::endl;
}

/**
 * Sample usage
 */
void exampleUsage() {
    std::cout << "EXAMPLE USAGE" << std::endl;
    std::cout << "./dubov_pairing_engine --pairings \"path/to/file.trf\"" << std::endl;
    std::cout << "FILE MUST BE IN TRF16 FORMAT!\n\n";
    std::cout << "EXAMPLE USAGE FOR RANDOM TOURNAMENT GENERATOR\n";
    std::cout << "./dubov_pairing_engine --rtg path/to/trf/output.trf --p_count 10 --rtg_rounds 5\n\n";
    std::cout << "EXAMPLE USAGE FOR FREE PAIRINGS CHECKER\n";
    std::cout << "./dubov_pairing_engine --fpc path/to/trf/output.trf --fpc_rounds 2\n";
}

/**
 * Outputs the pairings generated
 */
void outputPairings(std::vector<Match> pairings, int for_round) {
    std::cout << "---------PAIRINGS FOR ROUND " << for_round << "---------" << std::endl;
    std::cout << "|White|Black|" << std::endl;
    std::cout << "|-----|-----|" << std::endl;
    for(int i = 0; i < pairings.size(); i++) {
        std::cout << "|" << pairings[i].white.getName() << "|";
        if(pairings[i].is_bye) {
            std::cout << "BYE|\n";
            continue;
        }
        std::cout << pairings[i].black.getName() << "|\n";
    }
}

int main(int argc, const char * argv[]) {
    if(argc == 1) {
        std::string v = argv[1];
        if(v == "--version") {
            std::cout << "DubovSystem -- Version 1.0\n";
            return 0;
        }
        helpDisplay();
        return 0;
    } /*else if(argc > 3) {
        std::cout << "Too many arguments given. Run --help for a list of available commands" << std::endl;
        return 0;
    }*/
    if(argc == 2) {
        std::string passed = argv[1];
        if(passed == "--help") {
            helpDisplay();
            return 0;
        } else if(passed == "--sample") {
            exampleUsage();
            return 0;
        } else if(passed == "--pairings") {
            std::cout << "Missing TRF file path" << std::endl;
            return 0;
        }
        std::cout << "Unexpected command passed in. Execute --help command or --sample for sample usage" << std::endl;
        return 0;
    }
    // argc = 3
    std::string pair_command = argv[1];
    std::string path = argv[2];
    if(pair_command == "--rtg") {
        // expect p-count and rounds
        if(argc < 7) {
            std::cout << "Too few arguments passed for random tournament generator. Run --sample for example usage\n";
            return 0;
        }
        std::string p1 = argv[3];
        std::string p11 = argv[4];
        std::string p2 = argv[5];
        std::string p3 = argv[6];
        if(p1 != "--p_count") {
            std::cout << "Expected --p_count in call\n";
            return 0;
        }
        if(p2 != "--rtg_rounds") {
            std::cout << "Expected --rtg_rounds in call\n";
            return 0;
        }
        int pc = std::stoi(p11);
        int r = std::stoi(p3);
        std::string trf_output = Tournament::simulateTournament(pc, r);
        TRFUtil::TRFFile file_write(path);
        file_write.write(trf_output);
        return 0;
    } else if(pair_command == "--fpc") {
        // expect fpc_rounds
        if(argc != 5) {
            std::cout << "Invalid arguments passed for free pairings checker. Run --sample for example usage\n";
            return 0;
        }
        std::string p1 = argv[3];
        std::string p2 = argv[4];
        if(p1 != "--fpc_rounds") {
            std::cout << "Missing --fpc_rounds in call";
            return 0;
        }
        int pc = std::stoi(p2);
        fpc::PairingsChecker pc_main(path, pc);
        std::cout << "Initiating Pairings Checker. Note that when reading output here, pay attention primarily to the expected pairings. This tool can only say errors in given pairings. It cannot give any other errors than basic pairing violations such as violation to C1.\n\n";
        std::cout << pc_main.outputReport();
        return 0;
    }
    if(pair_command != "--pairings") {
        std::cout << "Unknown command passed in" << std::endl;
        return 0;
    }
    TRFUtil::TRFFile file(path);
    TRFUtil::TRFData file_read = file.read();
    
    // make sure rounds exist
    if(file_read.getTournamentSection().find("TNR") == file_read.getTournamentSection().end()) {
        std::cout << "Missing tournament number of rounds in TRF file" << std::endl;
        return 2;
    }
    
    int rounds_done = 0;
    
    // get tournament
    Tournament trfTournament = Tournament::makeTournament(file_read, &rounds_done);
    
    // do next round pairings
    std::vector<Match> m = trfTournament.generatePairings(rounds_done + 1);
    
    // check for errors as needed
    if(trfTournament.pairingErrorOccured()) {
        std::cout << "Pairings cannot be generated!" << std::endl;
        return -1;
    }
    
    outputPairings(m, rounds_done + 1);
    
    //MARK: TEST INFO
    /// r2 (o) -> p4-p1,p2-p3,p6-p5,p10-p7,p8-p9
    /// r3 (o) -> p1-p2,p5-p4,p7-p8,p3-p10,p9-p6
    /// r4 (o) -> p1-p3,p2-p5,p4-p7,p6-p8,p10-p9
    /// r5 (o) -> p5-p1,p2-p4,p7-p6,p3-p9,p8-p10
    //MARK: TEST INFO END
    //MARK: TEST INFO2
    /// r2 (o) -> p11-p1,p2-p5,p4-p3,p6-p9,p8-p7,p10-B
    /// r3 (o) -> p3-p2,p1-p4,p5-p6,p7-p11,p10-p8,p9-B
    /// r4 (o) -> p2-p1,p3-p5,p6-p7,p4-p11,p9-p10,p8-B
    /// r5 (o) -> p1-p3,p8-p2,p6-p4,p5-p9,p11-p10,p7-B
    //MARK: TEST INFO2 END
    
    return 0;
}
