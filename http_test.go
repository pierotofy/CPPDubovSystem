package main

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"net/url"
	"testing"
)

func TestHTTPEndpoints(t *testing.T) {
	// Test root endpoint
	req := httptest.NewRequest("GET", "/", nil)
	w := httptest.NewRecorder()
	handleRoot(w, req)

	if w.Code != http.StatusOK {
		t.Errorf("Expected status 200, got %d", w.Code)
	}

	if w.Body.String() != "Swisser is running" {
		t.Errorf("Expected 'Swisser is running', got %s", w.Body.String())
	}
}

func TestPingEndpoint(t *testing.T) {
	req := httptest.NewRequest("GET", "/ping", nil)
	w := httptest.NewRecorder()
	handlePing(w, req)

	if w.Code != http.StatusOK {
		t.Errorf("Expected status 200, got %d", w.Code)
	}

	var response PingResponse
	err := json.Unmarshal(w.Body.Bytes(), &response)
	if err != nil {
		t.Fatalf("Failed to parse JSON response: %v", err)
	}

	if response.Swisser != "running" {
		t.Errorf("Expected 'running', got %s", response.Swisser)
	}
}

func TestRoundEndpoint(t *testing.T) {
	// Test with example.json data
	data := `{"rounds": 5,"players": [{"name": "player1", "elo": 2000},{"name": "player2",  "elo": 700},{"name": "player3", "elo": 1000}],"games": [[{"white": "player1", "black": "player2", "result": 1.0},{"white": "player3", "bye": true}]]}`

	form := url.Values{}
	form.Add("data", data)

	req := httptest.NewRequest("POST", "/round", bytes.NewBufferString(form.Encode()))
	req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	w := httptest.NewRecorder()
	handleRound(w, req)

	if w.Code != http.StatusOK {
		t.Errorf("Expected status 200, got %d", w.Code)
	}

	var response []PairingResponse
	err := json.Unmarshal(w.Body.Bytes(), &response)
	if err != nil {
		t.Fatalf("Failed to parse JSON response: %v", err)
	}

	// Should have 2 pairings
	if len(response) != 2 {
		t.Errorf("Expected 2 pairings, got %d", len(response))
	}

	// Verify pairing structure
	regularCount := 0
	byeCount := 0
	for _, pairing := range response {
		if pairing.Bye {
			byeCount++
			if pairing.Black != "" {
				t.Error("Bye pairing should not have black player")
			}
		} else {
			regularCount++
			if pairing.Black == "" {
				t.Error("Regular pairing should have black player")
			}
		}
	}

	if regularCount != 1 || byeCount != 1 {
		t.Errorf("Expected 1 regular and 1 bye pairing, got %d regular and %d bye", regularCount, byeCount)
	}
}

func TestRoundEndpointErrors(t *testing.T) {
	// Test with missing data parameter
	req := httptest.NewRequest("POST", "/round", nil)
	w := httptest.NewRecorder()
	handleRound(w, req)

	if w.Code != http.StatusBadRequest {
		t.Errorf("Expected status 400, got %d", w.Code)
	}

	// Test with invalid JSON
	form := url.Values{}
	form.Add("data", "invalid json")

	req = httptest.NewRequest("POST", "/round", bytes.NewBufferString(form.Encode()))
	req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	w = httptest.NewRecorder()
	handleRound(w, req)

	if w.Code != http.StatusBadRequest {
		t.Errorf("Expected status 400, got %d", w.Code)
	}

	var response ErrorResponse
	err := json.Unmarshal(w.Body.Bytes(), &response)
	if err != nil {
		t.Fatalf("Failed to parse error response: %v", err)
	}

	if response.Error == "" {
		t.Error("Expected error message in response")
	}
}

func TestRoundEndpointWrongMethod(t *testing.T) {
	req := httptest.NewRequest("GET", "/round", nil)
	w := httptest.NewRecorder()
	handleRound(w, req)

	if w.Code != http.StatusMethodNotAllowed {
		t.Errorf("Expected status 405, got %d", w.Code)
	}
}