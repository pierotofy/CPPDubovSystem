// Package swisser implements the FIDE Swiss Dubov system for chess tournament pairings.
// 
// Copyright (C) 2025 Piero Toffanin
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

package main

// Color represents the color a player can play in a match
type Color int

const (
	NoColor Color = iota
	White
	Black
)

// ColorPreference represents the strength of a player's color preference
type ColorPreference int

const (
	NoPreference ColorPreference = 1 + iota
	Mild
	Alternation
	Absolute
)

// Player represents a tournament player with their game history and preferences
type Player struct {
	Name                string
	Rating              int
	ID                  int
	Points              float64
	OppPlayed           map[int]bool  // Opponents played against (by ID)
	OppPlayedReturn     []int         // List of opponent IDs for iteration
	PairingRestrictions map[int]bool  // Players this player cannot be paired with
	ColorHist           []Color       // History of colors played
	OppRating           []int         // Ratings of opponents faced
	OppCount            int           // Number of opponents played
	ColorCount          int           // Number of colors played
	NumUpfloated        int           // Number of times upfloated
	ARO                 float64       // Average Rating of Opponents (cached)
	UpfloatedPrev       bool          // If player upfloated in the last round
	ReceivedBye         bool          // If player had a bye/forfeit win
	DueColorSaved       Color         // Saved due color
	Strength            ColorPreference // Saved preference strength
	HasSavedColor       bool          // If color preference was saved
	HasSavedStrength    bool          // If strength has been saved
	TrfPts              float64       // For TRF parsing compatibility
}

// NewPlayer creates a new player with the given parameters
func NewPlayer(name string, rating, id int, points float64) *Player {
	return &Player{
		Name:                name,
		Rating:              rating,
		ID:                  id,
		Points:              points,
		OppPlayed:           make(map[int]bool),
		OppPlayedReturn:     make([]int, 0),
		PairingRestrictions: make(map[int]bool),
		ColorHist:           make([]Color, 0),
		OppRating:           make([]int, 0),
		ARO:                 -1.0,
		DueColorSaved:       NoColor,
		Strength:            NoPreference,
	}
}

// CanPlayOpp determines if this player can play against the given opponent
func (p *Player) CanPlayOpp(opp *Player) bool {
	if p.ID == opp.ID {
		return false
	}
	if p.OppPlayed[opp.ID] {
		return false
	}
	if p.PairingRestrictions[opp.ID] {
		return false
	}
	return true
}

// GetARO calculates and returns the average rating of opponents
func (p *Player) GetARO() float64 {
	if p.ARO >= 0 {
		return p.ARO
	}
	
	if len(p.OppRating) == 0 {
		p.ARO = 0.0
		return p.ARO
	}
	
	sum := 0
	for _, rating := range p.OppRating {
		sum += rating
	}
	p.ARO = float64(sum) / float64(len(p.OppRating))
	return p.ARO
}

// AddOpp adds an opponent to the list of played opponents
func (p *Player) AddOpp(id int) {
	if !p.OppPlayed[id] {
		p.OppPlayed[id] = true
		p.OppPlayedReturn = append(p.OppPlayedReturn, id)
		p.OppCount++
	}
}

// AddColor adds a color to the player's color history
func (p *Player) AddColor(c Color) {
	p.ColorHist = append(p.ColorHist, c)
	p.ColorCount++
}

// AddOppRating adds an opponent's rating to the list
func (p *Player) AddOppRating(rating int) {
	p.OppRating = append(p.OppRating, rating)
	p.ARO = -1.0 // Reset cached ARO
}

// AddPoints adds points to the player's total
func (p *Player) AddPoints(pts float64) {
	p.Points += pts
}

// IncrementUpfloat increments the upfloat count
func (p *Player) IncrementUpfloat() {
	p.NumUpfloated++
}

// CanUpfloat determines if the player can upfloat in the current round
func (p *Player) CanUpfloat(currentRound int) bool {
	// Basic implementation - can be refined based on tournament rules
	return p.NumUpfloated < currentRound/2
}

// SetUpfloatPrevStatus sets whether the player upfloated in the previous round
func (p *Player) SetUpfloatPrevStatus(status bool) {
	p.UpfloatedPrev = status
}

// SetByeStatus sets whether the player received a bye
func (p *Player) SetByeStatus(status bool) {
	p.ReceivedBye = status
}

// AddPairingRestriction adds a pairing restriction against a specific opponent
func (p *Player) AddPairingRestriction(oppID int) {
	p.PairingRestrictions[oppID] = true
}

// GetDueColor determines the color the player is due to play
func (p *Player) GetDueColor() Color {
	if len(p.ColorHist) == 0 {
		return NoColor
	}
	
	whiteCount := 0
	blackCount := 0
	
	for _, color := range p.ColorHist {
		switch color {
		case White:
			whiteCount++
		case Black:
			blackCount++
		}
	}
	
	// If equal or no colors played, no preference
	if whiteCount == blackCount {
		return NoColor
	}
	
	// Due the color played less
	if whiteCount > blackCount {
		return Black
	}
	return White
}

// GetPreferenceStrength determines how strongly the player prefers their due color
func (p *Player) GetPreferenceStrength() ColorPreference {
	if len(p.ColorHist) == 0 {
		return NoPreference
	}
	
	whiteCount := 0
	blackCount := 0
	
	for _, color := range p.ColorHist {
		switch color {
		case White:
			whiteCount++
		case Black:
			blackCount++
		}
	}
	
	diff := whiteCount - blackCount
	if diff < 0 {
		diff = -diff
	}
	
	switch diff {
	case 0:
		return NoPreference
	case 1:
		return Mild
	case 2:
		return Alternation
	default:
		return Absolute
	}
}

// ShouldAlternate determines if the player should receive their due color over the opponent
func (p *Player) ShouldAlternate(opp *Player) bool {
	pPref := p.GetPreferenceStrength()
	oppPref := opp.GetPreferenceStrength()
	
	// Player with stronger preference gets priority
	if pPref > oppPref {
		return true
	}
	if oppPref > pPref {
		return false
	}
	
	// If equal preference, check other factors (e.g., rating, ID)
	if p.Rating != opp.Rating {
		return p.Rating > opp.Rating
	}
	
	return p.ID < opp.ID
}

// IsColorHistEqual checks if two players have equal color histories
func (p *Player) IsColorHistEqual(opp *Player) bool {
	if len(p.ColorHist) != len(opp.ColorHist) {
		return false
	}
	
	pWhite, pBlack := 0, 0
	oppWhite, oppBlack := 0, 0
	
	for _, color := range p.ColorHist {
		if color == White {
			pWhite++
		} else if color == Black {
			pBlack++
		}
	}
	
	for _, color := range opp.ColorHist {
		if color == White {
			oppWhite++
		} else if color == Black {
			oppBlack++
		}
	}
	
	return pWhite == oppWhite && pBlack == oppBlack
}

// GetFirstColorPlayed returns the first color the player played
func (p *Player) GetFirstColorPlayed() Color {
	if len(p.ColorHist) == 0 {
		return NoColor
	}
	return p.ColorHist[0]
}