#!/usr/bin/env python3
import sys
import json

# Minimal MCP server that just responds to initialize
while True:
    try:
        line = sys.stdin.readline()
        if not line:
            break
        
        request = json.loads(line)
        
        if request.get("method") == "initialize":
            response = {
                "jsonrpc": "2.0",
                "id": request.get("id"),
                "result": {
                    "protocolVersion": "2024-11-05",
                    "capabilities": {"tools": {}},
                    "serverInfo": {"name": "test-mcp", "version": "1.0.0"}
                }
            }
            print(json.dumps(response), flush=True)
        elif request.get("method") == "tools/list":
            response = {
                "jsonrpc": "2.0",
                "id": request.get("id"),
                "result": {"tools": []}
            }
            print(json.dumps(response), flush=True)
    except Exception as e:
        sys.stderr.write(f"Error: {e}\n")
        sys.stderr.flush()
