package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net/http"
)

// Request structures for JSON parsing
type TournamentRequest struct {
	Rounds  int             `json:"rounds"`
	Players []PlayerRequest `json:"players"`
	Games   [][]GameResult  `json:"games,omitempty"`
}

type PlayerRequest struct {
	Name string `json:"name"`
	Elo  int    `json:"elo"`
}

type GameResult struct {
	White  string   `json:"white"`
	Black  string   `json:"black,omitempty"`
	Result *float64 `json:"result,omitempty"`
	Bye    bool     `json:"bye,omitempty"`
}

// Response structures for JSON output
type PairingResponse struct {
	White string `json:"white"`
	Black string `json:"black,omitempty"`
	Bye   bool   `json:"bye,omitempty"`
}

type ErrorResponse struct {
	Error string `json:"error"`
}

type PingResponse struct {
	Swisser string `json:"swisser"`
}

func main() {
	var host = flag.String("host", "0.0.0.0", "Host to bind to")
	var port = flag.Int("port", 8080, "Port to bind to")
	flag.Parse()

	http.HandleFunc("/", handleRoot)
	http.HandleFunc("/ping", handlePing)
	http.HandleFunc("/round", handleRound)

	addr := fmt.Sprintf("%s:%d", *host, *port)
	fmt.Printf("Running swisser on %s\n", addr)
	log.Fatal(http.ListenAndServe(addr, nil))
}

func handleRoot(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/plain")
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("Swisser is running"))
}

func handlePing(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	response := PingResponse{Swisser: "running"}
	json.NewEncoder(w).Encode(response)
}

func handleRound(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	// Parse form data to get the 'data' parameter
	err := r.ParseForm()
	if err != nil {
		sendError(w, "Failed to parse form data: "+err.Error(), http.StatusBadRequest)
		return
	}

	data := r.FormValue("data")
	if data == "" {
		sendError(w, "Missing 'data' parameter", http.StatusBadRequest)
		return
	}

	// Parse JSON data
	var req TournamentRequest
	err = json.Unmarshal([]byte(data), &req)
	if err != nil {
		sendError(w, "Invalid JSON: "+err.Error(), http.StatusBadRequest)
		return
	}

	// Create tournament
	tournament := NewTournament(req.Rounds)

	// Add players
	playerMap := make(map[string]*Player)
	id := 1
	for _, p := range req.Players {
		player := NewPlayer(p.Name, p.Elo, id, 0.0)
		tournament.AddPlayer(player)
		playerMap[p.Name] = player
		id++
	}

	// Replay game history if provided
	if req.Games != nil {
		for _, roundGames := range req.Games {
			for _, game := range roundGames {
				white, whiteExists := playerMap[game.White]
				if !whiteExists {
					sendError(w, "Unknown player: "+game.White, http.StatusBadRequest)
					return
				}

				// Handle bye games
				if game.Bye {
					white.SetByeStatus(true)
					white.AddPoints(1.0) // Bye gives 1 point
					continue
				}

				// Handle regular games
				if game.Black == "" {
					sendError(w, "Black player missing for non-bye game", http.StatusBadRequest)
					return
				}

				black, blackExists := playerMap[game.Black]
				if !blackExists {
					sendError(w, "Unknown player: "+game.Black, http.StatusBadRequest)
					return
				}

				// Record colors played
				white.AddColor(White)
				black.AddColor(Black)

				// Record opponents
				white.AddOpp(black.ID)
				white.AddOppRating(black.Rating)
				black.AddOpp(white.ID)
				black.AddOppRating(white.Rating)

				// Record results
				if game.Result != nil {
					result := *game.Result
					if result == 1.0 {
						// White won
						white.AddPoints(1.0)
					} else if result == 0.5 {
						// Draw
						white.AddPoints(0.5)
						black.AddPoints(0.5)
					} else if result == 0.0 {
						// Black won
						black.AddPoints(1.0)
					}
				}
			}
		}
	}

	// Generate pairings for next round
	nextRound := len(req.Games) + 1
	pairings, err := tournament.GeneratePairings(nextRound)
	if err != nil {
		sendError(w, "Failed to generate pairings: "+err.Error(), http.StatusInternalServerError)
		return
	}

	// Convert to response format
	response := make([]PairingResponse, 0, len(pairings))
	for _, match := range pairings {
		pairing := PairingResponse{
			White: match.White.Name,
		}

		if match.IsBye {
			pairing.Bye = true
		} else {
			pairing.Black = match.Black.Name
		}

		response = append(response, pairing)
	}

	// Send response
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(response)
}

func sendError(w http.ResponseWriter, message string, statusCode int) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(statusCode)
	response := ErrorResponse{Error: message}
	json.NewEncoder(w).Encode(response)
}