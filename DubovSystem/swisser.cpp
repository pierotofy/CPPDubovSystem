#include <iostream>
#include <vector>
#include "Tournament.hpp" // we need this to work with tournaments
#include "Player.hpp" // we also need this to add players
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

int main(int argc, char **argv) {
    httplib::Server svr;
    std::string host = "0.0.0.0";
    int port = 8080;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--host" && i + 1 < argc) {
            host = argv[i + 1];
            i++; // Skip next argument since we used it
        } else if (std::string(argv[i]) == "--port" && i + 1 < argc) {
            port = std::stoi(argv[i + 1]);
            i++; // Skip next argument since we used it
        }
    }

    // // first make our tournament
    
    // // now we should add some players
    // CPPDubovSystem::Player player2("Player 2", 1100, 2, 0.0);
    // CPPDubovSystem::Player player3("Player 3", 1000, 3, 0.0);
    // CPPDubovSystem::Player player4("Player 4", 900, 4, 0.0);
    
    // our_tournament.addPlayer(player1); // this is how to add the player to the tournament
    // our_tournament.addPlayer(player2);
    // our_tournament.addPlayer(player3);
    // our_tournament.addPlayer(player4);
    
    // // now to get the pairings
    // std::vector<CPPDubovSystem::Match> pairings = our_tournament.generatePairings(1); // we pass in the round to generate pairings for
    
    // for(int i = 0; i < pairings.size(); i++) {
    //     CPPDubovSystem::Match match = pairings[i];
    //     if(match.is_bye) {
    //         std::cout << "BYE\n";
    //     } else {
    //         std::cout << "White: " << match.white.getName() << std::endl;
    //         std::cout << "Black: " << match.black.getName() << std::endl;
    //     }
    // }

    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Swisser is running", "text/plain");
    });
    svr.Post("/round", [](const httplib::Request &req, httplib::Response &res) {
        std::string data = req.get_param_value("data");
        
        try{
            json j = json::parse(data);
            int rounds = j["rounds"].get<int>();
            std::cout << rounds << std::endl;

            json games = json::array();
            if (j.contains("games")){
                games = j["games"];
            }

            CPPDubovSystem::Tournament tournament(rounds);
            std::unordered_map<std::string, CPPDubovSystem::Player> players;
            int id = 1;

            for (const auto &p : j["players"]){
                std::string name = p["name"].get<std::string>();
                CPPDubovSystem::Player player(name, 
                                              p["elo"].get<int>(), id++, 0.0);
                tournament.addPlayer(player);
                players[name] = player;
            }
            
            // Replay game history (optional)
            for (const auto &results : games){
                for (const auto &r: results){
                    std::string white = r["white"].get<std::string>();
                    
                    std::string black = "";
                    if (r.contains("black")) black = r["black"].get<std::string>();
                    
                    bool bye = r.contains("bye") && r["bye"].get<bool>();
                    
                    float result = 0.0f;
                    if (r.contains("result")) result = r["result"].get<float>();
                    
                    
                    auto w = players[white];
                    // White played white
                    if (!bye) w.addColor(CPPDubovSystem::Color::WHITE);

                    if (!black.empty()){
                        auto b = players[black];

                        // Black played black
                        b.addColor(CPPDubovSystem::Color::BLACK);

                        // They played each other
                        w.addOpp(b.getID());
                        w.addOppRating(b.getRating());

                        b.addOpp(w.getID());
                        b.addOppRating(w.getRating());

                        // White won
                        if (result == 1.0){
                            w.addPoints(1.0);
                        // Draw
                        }else if (result == 0.5){
                            w.addPoints(0.5);
                            b.addPoints(0.5);
                        // Black won
                        }else if (result == 0.0){
                            b.addPoints(1.0);
                        }
                        
                    }

                    if (bye){
                        w.setByeStatus(true);
                    }
                }
            }
            
            int nextRound = games.size() + 1;
            std::vector<CPPDubovSystem::Match> pairings = tournament.generatePairings(nextRound);
            
            json pairs = json::array();

            for(int i = 0; i < pairings.size(); i++) {
                CPPDubovSystem::Match match = pairings[i];
                json m = json::object();
                m["white"] = match.white.getName();

                if(match.is_bye) {
                    m["bye"] = true;
                } else {
                    m["black"] = match.black.getName();
                }
                pairs.push_back(m);
            }

            res.set_content(pairs.dump(), "application/json");
        }catch (const std::exception& e) {
            json j;
            j["error"] = e.what();
            res.status = httplib::StatusCode::BadRequest_400;
            res.set_content(j.dump(), "application/json");
        }
        
    });

    std::cout << "Running swisser on " << host << ":" << port << std::endl;
    svr.listen(host, port);
    
    return 0;
}