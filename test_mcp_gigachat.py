"""
Test GigaChat Vision MCP Server
"""

import subprocess
import json
import sys


def test_mcp_server():
    """Test MCP server by sending a request"""
    
    # Start MCP server
    process = subprocess.Popen(
        ["python", "gigachat_vision_mcp.py"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        cwd=r"C:\Users\danys\Desktop\geometry-dash-mcp"
    )
    
    # Initialize request
    init_request = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {
                "name": "test-client",
                "version": "1.0.0"
            }
        }
    }
    
    print("Sending initialize request...")
    process.stdin.write(json.dumps(init_request) + "\n")
    process.stdin.flush()
    
    # Read response
    response = process.stdout.readline()
    print(f"Response: {response}")
    
    # List tools request
    list_tools_request = {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "tools/list",
        "params": {}
    }
    
    print("\nSending tools/list request...")
    process.stdin.write(json.dumps(list_tools_request) + "\n")
    process.stdin.flush()
    
    response = process.stdout.readline()
    print(f"Response: {response}")
    
    # Cleanup
    process.terminate()
    process.wait(timeout=5)


if __name__ == "__main__":
    test_mcp_server()
