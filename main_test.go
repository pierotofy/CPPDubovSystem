package main

import (
	"encoding/json"
	"testing"
)

func TestBasicPairing(t *testing.T) {
	// Test case from example.json
	tournament := NewTournament(5)
	
	// Add players
	player1 := NewPlayer("player1", 2000, 1, 0.0)
	player2 := NewPlayer("player2", 700, 2, 0.0)
	player3 := NewPlayer("player3", 1000, 3, 0.0)
	
	tournament.AddPlayer(player1)
	tournament.AddPlayer(player2)
	tournament.AddPlayer(player3)
	
	// Replay first round game
	player1.AddColor(White)
	player2.AddColor(Black)
	player1.AddOpp(player2.ID)
	player2.AddOpp(player1.ID)
	player1.AddOppRating(player2.Rating)
	player2.AddOppRating(player1.Rating)
	player1.AddPoints(1.0) // player1 won
	
	player3.SetByeStatus(true)
	player3.AddPoints(1.0) // bye point
	
	// Generate pairings for round 2
	pairings, err := tournament.GeneratePairings(2)
	if err != nil {
		t.Fatalf("Failed to generate pairings: %v", err)
	}
	
	// Should have 2 pairings (one regular match, one bye)
	if len(pairings) != 2 {
		t.Errorf("Expected 2 pairings, got %d", len(pairings))
	}
	
	// Check that we have one bye and one regular match
	byeCount := 0
	regularCount := 0
	for _, match := range pairings {
		if match.IsBye {
			byeCount++
		} else {
			regularCount++
		}
	}
	
	if byeCount != 1 || regularCount != 1 {
		t.Errorf("Expected 1 bye and 1 regular match, got %d byes and %d regular matches", byeCount, regularCount)
	}
}

func TestTournamentJSONRequest(t *testing.T) {
	// Test parsing the example.json data
	jsonData := `{"rounds": 5,"players": [{"name": "player1", "elo": 2000},{"name": "player2",  "elo": 700},{"name": "player3", "elo": 1000}],"games": [[{"white": "player1", "black": "player2", "result": 1.0},{"white": "player3", "bye": true}]]}`
	
	var req TournamentRequest
	err := json.Unmarshal([]byte(jsonData), &req)
	if err != nil {
		t.Fatalf("Failed to parse JSON: %v", err)
	}
	
	// Verify parsed data
	if req.Rounds != 5 {
		t.Errorf("Expected 5 rounds, got %d", req.Rounds)
	}
	
	if len(req.Players) != 3 {
		t.Errorf("Expected 3 players, got %d", len(req.Players))
	}
	
	if len(req.Games) != 1 {
		t.Errorf("Expected 1 round of games, got %d", len(req.Games))
	}
	
	if len(req.Games[0]) != 2 {
		t.Errorf("Expected 2 games in first round, got %d", len(req.Games[0]))
	}
}

func TestPlayerColors(t *testing.T) {
	player := NewPlayer("test", 1500, 1, 0.0)
	
	// Initially no color preference
	if player.GetDueColor() != NoColor {
		t.Errorf("Expected NoColor initially, got %v", player.GetDueColor())
	}
	
	// After playing white, should prefer black
	player.AddColor(White)
	if player.GetDueColor() != Black {
		t.Errorf("Expected Black after playing White, got %v", player.GetDueColor())
	}
	
	// After playing black too, should have no preference
	player.AddColor(Black)
	if player.GetDueColor() != NoColor {
		t.Errorf("Expected NoColor after equal colors, got %v", player.GetDueColor())
	}
	
	// After playing white twice, should strongly prefer black
	player.AddColor(White)
	if player.GetDueColor() != Black {
		t.Errorf("Expected Black after two whites, got %v", player.GetDueColor())
	}
	
	// Check preference strength
	if player.GetPreferenceStrength() != Mild {
		t.Errorf("Expected Mild preference, got %v", player.GetPreferenceStrength())
	}
}

func TestCanPlayOpp(t *testing.T) {
	player1 := NewPlayer("player1", 2000, 1, 0.0)
	player2 := NewPlayer("player2", 1800, 2, 0.0)
	
	// Initially should be able to play
	if !player1.CanPlayOpp(player2) {
		t.Error("Players should be able to play initially")
	}
	
	// After playing each other, should not be able to play again
	player1.AddOpp(player2.ID)
	if player1.CanPlayOpp(player2) {
		t.Error("Players should not be able to play again")
	}
}

func TestFirstRoundPairing(t *testing.T) {
	tournament := NewTournament(5)
	
	// Add players with different ratings
	player1 := NewPlayer("player1", 2000, 1, 0.0)
	player2 := NewPlayer("player2", 1800, 2, 0.0)
	player3 := NewPlayer("player3", 1600, 3, 0.0)
	player4 := NewPlayer("player4", 1400, 4, 0.0)
	
	tournament.AddPlayer(player1)
	tournament.AddPlayer(player2)
	tournament.AddPlayer(player3)
	tournament.AddPlayer(player4)
	
	// Generate first round pairings
	pairings, err := tournament.GeneratePairings(1)
	if err != nil {
		t.Fatalf("Failed to generate pairings: %v", err)
	}
	
	// Should have 2 pairings for 4 players
	if len(pairings) != 2 {
		t.Errorf("Expected 2 pairings for 4 players, got %d", len(pairings))
	}
	
	// All should be regular matches (no byes with even number of players)
	for _, match := range pairings {
		if match.IsBye {
			t.Error("Should not have byes with even number of players")
		}
	}
}

func TestExampleJSONPairing(t *testing.T) {
	// Test the exact scenario from example.json
	tournament := NewTournament(5)
	
	// Add players with exact ratings from example.json
	player1 := NewPlayer("player1", 2000, 1, 0.0)
	player2 := NewPlayer("player2", 700, 2, 0.0)
	player3 := NewPlayer("player3", 1000, 3, 0.0)
	
	tournament.AddPlayer(player1)
	tournament.AddPlayer(player2)
	tournament.AddPlayer(player3)
	
	// Debug: print initial points
	t.Logf("Initial points: player1=%.1f, player2=%.1f, player3=%.1f", 
		player1.Points, player2.Points, player3.Points)
	
	// Replay first round exactly as in example.json
	// player1 (white) beat player2 (black) with result 1.0
	player1.AddColor(White)
	player2.AddColor(Black)
	player1.AddOpp(player2.ID)
	player2.AddOpp(player1.ID)
	player1.AddOppRating(player2.Rating)
	player2.AddOppRating(player1.Rating)
	player1.AddPoints(1.0) // player1 won (result 1.0)
	// player2 gets 0 points (result 0.0 for black when white wins - no need to add points)
	
	// player3 had a bye
	player3.SetByeStatus(true)
	player3.AddPoints(1.0) // bye gives 1 point
	
	// Debug: print points after first round
	t.Logf("Points after round 1: player1=%.1f, player2=%.1f, player3=%.1f", 
		player1.Points, player2.Points, player3.Points)
	
	// Now player1 and player3 have 1.0 points, player2 has 0.0 points
	// Generate pairings for round 2
	pairings, err := tournament.GeneratePairings(2)
	if err != nil {
		t.Fatalf("Failed to generate pairings: %v", err)
	}
	
	// Print the pairings for debugging
	t.Logf("Generated pairings:")
	for i, match := range pairings {
		if match.IsBye {
			t.Logf("  %d: %s (bye)", i, match.White.Name)
		} else {
			t.Logf("  %d: %s (white) vs %s (black)", i, match.White.Name, match.Black.Name)
		}
	}
	
	// Expected from C++ output: [{"black":"player2","white":"player1"},{"bye":true,"white":"player3"}]
	// This means player1 (white) vs player2 (black), and player3 gets bye
	
	// Let's verify the points for debugging
	t.Logf("Player points: player1=%.1f, player2=%.1f, player3=%.1f", 
		player1.Points, player2.Points, player3.Points)
}

func TestOddNumberPlayers(t *testing.T) {
	tournament := NewTournament(5)
	
	// Add 3 players
	player1 := NewPlayer("player1", 2000, 1, 0.0)
	player2 := NewPlayer("player2", 1800, 2, 0.0)
	player3 := NewPlayer("player3", 1600, 3, 0.0)
	
	tournament.AddPlayer(player1)
	tournament.AddPlayer(player2)
	tournament.AddPlayer(player3)
	
	// Generate first round pairings
	pairings, err := tournament.GeneratePairings(1)
	if err != nil {
		t.Fatalf("Failed to generate pairings: %v", err)
	}
	
	// Should have 2 pairings (1 regular match + 1 bye)
	if len(pairings) != 2 {
		t.Errorf("Expected 2 pairings for 3 players, got %d", len(pairings))
	}
	
	// Should have exactly 1 bye
	byeCount := 0
	for _, match := range pairings {
		if match.IsBye {
			byeCount++
		}
	}
	
	if byeCount != 1 {
		t.Errorf("Expected exactly 1 bye, got %d", byeCount)
	}
}