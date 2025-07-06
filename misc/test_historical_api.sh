#!/bin/bash

# Test script for Bluecherry Historical Data API
# This script tests the new historical data endpoints on port 7005

API_HOST="localhost"
API_PORT="7005"
BASE_URL="http://${API_HOST}:${API_PORT}/api"

# Authentication credentials (replace with actual Bluecherry credentials)
# The API uses HTTP Basic Authentication with Bluecherry user credentials
USERNAME="admin"  # Replace with actual username
PASSWORD="password"  # Replace with actual password

echo "Testing Bluecherry Historical Data API..."
echo "=========================================="
echo "Note: This API requires HTTP Basic Authentication"
echo "Using credentials: $USERNAME (update script with actual credentials)"
echo ""

# Function to make authenticated requests
make_request() {
    local endpoint="$1"
    local description="$2"
    
    echo "$description..."
    response=$(curl -s -u "$USERNAME:$PASSWORD" "${BASE_URL}${endpoint}")
    
    if [ $? -eq 0 ]; then
        if echo "$response" | grep -q "401 Unauthorized"; then
            echo "  ❌ Authentication failed - check credentials"
        elif echo "$response" | grep -q "error\|Error"; then
            echo "  ❌ API error: $response"
        else
            echo "  ✅ Success"
            if command -v jq >/dev/null 2>&1; then
                echo "$response" | jq '.' 2>/dev/null | head -20
            fi
        fi
    else
        echo "  ❌ Request failed"
    fi
    echo ""
}

# Test current stats endpoints (should still work)
echo "1. Testing current CPU stats..."
make_request "/stats/cpu" "Current CPU statistics"

echo "2. Testing current memory stats..."
make_request "/stats/memory" "Current memory statistics"

echo "3. Testing current network stats..."
make_request "/stats/network" "Current network statistics"

echo "4. Testing current storage stats..."
make_request "/stats/storage" "Current storage statistics"

echo "5. Testing current overall stats..."
make_request "/stats/overall" "Current overall system statistics"

# Test historical data endpoints
echo "=========================================="
echo "Testing Historical Data API Endpoints..."
echo "=========================================="

echo "6. Testing CPU history (default 60 entries)..."
make_request "/stats/cpu/history" "CPU historical data (60 entries)"

echo "7. Testing CPU history (10 entries)..."
make_request "/stats/cpu/history?count=10" "CPU historical data (10 entries)"

echo "8. Testing memory history (default 60 entries)..."
make_request "/stats/memory/history" "Memory historical data (60 entries)"

echo "9. Testing network history (default 60 entries)..."
make_request "/stats/network/history" "Network historical data (60 entries)"

echo "10. Testing storage history (default 60 entries)..."
make_request "/stats/storage/history" "Storage historical data (60 entries)"

echo "11. Testing overall history (default 60 entries)..."
make_request "/stats/overall/history" "Overall system historical data (60 entries)"

echo "12. Testing overall history (100 entries)..."
make_request "/stats/overall/history?count=100" "Overall system historical data (100 entries)"

echo "=========================================="
echo "Historical Data API Test Complete!"
echo "=========================================="

# Show sample data structure
echo "Sample CPU history data structure:"
sample_response=$(curl -s -u "$USERNAME:$PASSWORD" "${BASE_URL}/stats/cpu/history?count=1")
if [ $? -eq 0 ] && ! echo "$sample_response" | grep -q "401 Unauthorized"; then
    if command -v jq >/dev/null 2>&1; then
        echo "$sample_response" | jq '.history[0]' 2>/dev/null
    else
        echo "$sample_response" | head -10
    fi
else
    echo "Authentication required to view sample data"
fi

echo ""
echo "Sample memory history data structure:"
sample_response=$(curl -s -u "$USERNAME:$PASSWORD" "${BASE_URL}/stats/memory/history?count=1")
if [ $? -eq 0 ] && ! echo "$sample_response" | grep -q "401 Unauthorized"; then
    if command -v jq >/dev/null 2>&1; then
        echo "$sample_response" | jq '.history[0]' 2>/dev/null
    else
        echo "$sample_response" | head -10
    fi
else
    echo "Authentication required to view sample data"
fi

echo ""
echo "=========================================="
echo "Authentication Information:"
echo "=========================================="
echo "The API uses HTTP Basic Authentication with Bluecherry user credentials."
echo "To use this API:"
echo "1. Update USERNAME and PASSWORD variables in this script"
echo "2. Use HTTP Basic Auth in your requests:"
echo "   curl -u username:password http://localhost:7005/api/stats/cpu/history"
echo "3. For web applications, include Authorization header:"
echo "   Authorization: Basic <base64-encoded-credentials>"
echo ""
echo "Note: The same authentication is used for both current and historical data endpoints." 