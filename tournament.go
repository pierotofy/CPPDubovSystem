package main

import (
	"sort"
)

// Match represents a pairing between two players or a bye
type Match struct {
	White *Player
	Black *Player
	IsBye bool
}

// NewMatch creates a new match
func NewMatch(white, black *Player, isBye bool) *Match {
	return &Match{
		White: white,
		Black: black,
		IsBye: isBye,
	}
}

// MatchEval represents a match with evaluation for problems
type MatchEval struct {
	*Match
	Problem bool
}

// NewMatchEval creates a new match evaluation
func NewMatchEval(white, black *Player, problem bool) *MatchEval {
	return &MatchEval{
		Match:   NewMatch(white, black, false),
		Problem: problem,
	}
}

// Tournament represents a Swiss system tournament
type Tournament struct {
	Players      []*Player
	PlayerCount  int
	TotalRounds  int
	PairingError bool
	CurrentRound int
	ByeQueue     []int // Queue of player IDs for bye assignments
}

// NewTournament creates a new tournament
func NewTournament(totalRounds int) *Tournament {
	return &Tournament{
		Players:      make([]*Player, 0),
		TotalRounds:  totalRounds,
		ByeQueue:     make([]int, 0),
	}
}

// AddPlayer adds a player to the tournament
func (t *Tournament) AddPlayer(player *Player) {
	t.Players = append(t.Players, player)
	t.PlayerCount++
}

// GeneratePairings generates pairings for the given round
func (t *Tournament) GeneratePairings(round int) ([]*Match, error) {
	t.CurrentRound = round
	t.PairingError = false
	
	if round == 1 {
		return t.makeRound1(), nil
	}
	
	return t.makeSubsequent(round), nil
}

// makeRound1 creates pairings for the first round using top-half vs bottom-half
func (t *Tournament) makeRound1() []*Match {
	// Sort players by rating (highest first)
	sortedPlayers := make([]*Player, len(t.Players))
	copy(sortedPlayers, t.Players)
	sort.Slice(sortedPlayers, func(i, j int) bool {
		return sortedPlayers[i].Rating > sortedPlayers[j].Rating
	})

	matches := make([]*Match, 0)
	middle := len(sortedPlayers) / 2
	opp := len(sortedPlayers) / 2
	currentColor := White

	// Pair top half vs bottom half with alternating colors
	for i := 0; i < middle; i++ {
		var white, black *Player
		
		if currentColor == White {
			white = sortedPlayers[i]
			black = sortedPlayers[opp]
			currentColor = Black
		} else {
			white = sortedPlayers[opp]
			black = sortedPlayers[i]
			currentColor = White
		}
		
		// Record the pairing
		white.AddOpp(black.ID)
		black.AddOpp(white.ID)
		white.AddOppRating(black.Rating)
		black.AddOppRating(white.Rating)

		// Add colors
		white.AddColor(White)
		black.AddColor(Black)

		matches = append(matches, NewMatch(white, black, false))
		opp++
	}

	// Handle odd number of players (bye for last player)
	if len(sortedPlayers)%2 == 1 {
		byePlayer := sortedPlayers[len(sortedPlayers)-1]
		// Don't add points here - that happens when results are recorded
		matches = append(matches, NewMatch(byePlayer, nil, true))
	}

	return matches
}

// makeSubsequent creates pairings for subsequent rounds
func (t *Tournament) makeSubsequent(round int) []*Match {
	// Group players by points
	groups := t.makeGroups()
	matches := make([]*Match, 0)
	
	for _, group := range groups {
		groupMatches := t.makePairingForGroup(group, round)
		matches = append(matches, groupMatches...)
	}
	
	return matches
}

// makeGroups creates point groups for pairing
func (t *Tournament) makeGroups() [][]*Player {
	// Sort players by points (highest first), then by ARO, then by rating
	sortedPlayers := make([]*Player, len(t.Players))
	copy(sortedPlayers, t.Players)
	
	sort.Slice(sortedPlayers, func(i, j int) bool {
		if sortedPlayers[i].Points != sortedPlayers[j].Points {
			return sortedPlayers[i].Points > sortedPlayers[j].Points
		}
		if sortedPlayers[i].GetARO() != sortedPlayers[j].GetARO() {
			return sortedPlayers[i].GetARO() > sortedPlayers[j].GetARO()
		}
		return sortedPlayers[i].Rating > sortedPlayers[j].Rating
	})
	
	// Group by points
	groups := make([][]*Player, 0)
	currentGroup := make([]*Player, 0)
	currentPoints := -1.0
	
	for _, player := range sortedPlayers {
		if player.Points != currentPoints {
			if len(currentGroup) > 0 {
				groups = append(groups, currentGroup)
			}
			currentGroup = make([]*Player, 0)
			currentPoints = player.Points
		}
		currentGroup = append(currentGroup, player)
	}
	
	if len(currentGroup) > 0 {
		groups = append(groups, currentGroup)
	}
	
	return groups
}

// makePairingForGroup creates pairings for a specific point group
func (t *Tournament) makePairingForGroup(group []*Player, round int) []*Match {
	if len(group) == 0 {
		return []*Match{}
	}
	
	// Handle single player (gets bye or floats)
	if len(group) == 1 {
		player := group[0]
		// Don't add points here - that happens when results are recorded
		return []*Match{NewMatch(player, nil, true)}
	}
	
	// For groups with even number of players, try optimal pairing
	if len(group)%2 == 0 {
		return t.pairEvenGroup(group)
	}
	
	// For groups with odd number of players, give bye to lowest and pair rest
	// Sort by rating to find lowest
	sort.Slice(group, func(i, j int) bool {
		return group[i].Rating > group[j].Rating
	})
	
	byePlayer := group[len(group)-1]
	// Don't add points here - that happens when results are recorded
	
	remainingGroup := group[:len(group)-1]
	matches := t.pairEvenGroup(remainingGroup)
	matches = append(matches, NewMatch(byePlayer, nil, true))
	
	return matches
}

// pairEvenGroup pairs an even number of players optimally
func (t *Tournament) pairEvenGroup(group []*Player) []*Match {
	if len(group) == 0 {
		return []*Match{}
	}
	
	if len(group) == 2 {
		return t.pairTwoPlayers(group[0], group[1])
	}
	
	// For larger groups, use a simplified pairing algorithm
	// Split into two halves and pair across halves
	mid := len(group) / 2
	topHalf := group[:mid]
	bottomHalf := group[mid:]
	
	matches := make([]*Match, 0)
	
	for i := 0; i < mid; i++ {
		white := topHalf[i]
		black := bottomHalf[i]
		
		// Check if they can play
		if !white.CanPlayOpp(black) {
			// Try to find alternative pairing
			found := false
			for j := i + 1; j < len(bottomHalf); j++ {
				alt := bottomHalf[j]
				if white.CanPlayOpp(alt) {
					// Swap
					bottomHalf[i], bottomHalf[j] = bottomHalf[j], bottomHalf[i]
					black = alt
					found = true
					break
				}
			}
			if !found {
				t.PairingError = true
			}
		}
		
		pairMatches := t.pairTwoPlayers(white, black)
		matches = append(matches, pairMatches...)
	}
	
	return matches
}

// pairTwoPlayers creates a match between two players with optimal color assignment
func (t *Tournament) pairTwoPlayers(p1, p2 *Player) []*Match {
	if !p1.CanPlayOpp(p2) {
		t.PairingError = true
		// Still create the match but mark the error
		return []*Match{NewMatch(p1, p2, false)}
	}
	
	// Determine colors based on preferences
	var white, black *Player
	
	p1Due := p1.GetDueColor()
	p2Due := p2.GetDueColor()
	
	// If one player is due white and the other black, assign accordingly
	if p1Due == White && p2Due == Black {
		white, black = p1, p2
	} else if p1Due == Black && p2Due == White {
		white, black = p2, p1
	} else if p1Due == White && p2Due != White {
		white, black = p1, p2
	} else if p2Due == White && p1Due != White {
		white, black = p2, p1
	} else if p1Due == Black && p2Due != Black {
		white, black = p2, p1
	} else if p2Due == Black && p1Due != Black {
		white, black = p1, p2
	} else {
		// Both have same preference or no preference, use other criteria
		if p1.ShouldAlternate(p2) {
			white, black = p1, p2
		} else {
			white, black = p2, p1
		}
	}
	
	// Record the pairing
	white.AddOpp(black.ID)
	black.AddOpp(white.ID)
	white.AddOppRating(black.Rating)
	black.AddOppRating(white.Rating)
	
	// Add colors
	white.AddColor(White)
	black.AddColor(Black)
	
	return []*Match{NewMatch(white, black, false)}
}

// Utility functions for sorting

// sortPlayersByRating sorts players by rating (highest first)
func sortPlayersByRating(players []*Player) {
	sort.Slice(players, func(i, j int) bool {
		return players[i].Rating > players[j].Rating
	})
}

// sortPlayersByPoints sorts players by points, then ARO, then rating
func sortPlayersByPoints(players []*Player) {
	sort.Slice(players, func(i, j int) bool {
		if players[i].Points != players[j].Points {
			return players[i].Points > players[j].Points
		}
		if players[i].GetARO() != players[j].GetARO() {
			return players[i].GetARO() > players[j].GetARO()
		}
		return players[i].Rating > players[j].Rating
	})
}

// GetPlayers returns all players in the tournament
func (t *Tournament) GetPlayers() []*Player {
	return t.Players
}

// PairingErrorOccurred returns whether a pairing error occurred
func (t *Tournament) PairingErrorOccurred() bool {
	return t.PairingError
}

// GetPlayerCount returns the number of players in the tournament
func (t *Tournament) GetPlayerCount() int {
	return t.PlayerCount
}