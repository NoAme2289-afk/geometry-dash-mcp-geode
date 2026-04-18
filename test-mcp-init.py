import subprocess
import json
import sys

# Start MCP server
proc = subprocess.Popen(
    ["python", "C:\\Users\\danys\\Desktop\\geometry-dash-mcp\\mcp-server.py"],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

# Send initialize request
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
proc.stdin.write(json.dumps(init_request) + "\n")
proc.stdin.flush()

# Read response
print("Waiting for response...")
try:
    response = proc.stdout.readline()
    print(f"Response: {response}")
    
    # Send tools/list request
    list_request = {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "tools/list",
        "params": {}
    }
    
    print("\nSending tools/list request...")
    proc.stdin.write(json.dumps(list_request) + "\n")
    proc.stdin.flush()
    
    response2 = proc.stdout.readline()
    print(f"Response: {response2}")
    
except Exception as e:
    print(f"Error: {e}")
    stderr = proc.stderr.read()
    print(f"Stderr: {stderr}")

proc.terminate()
