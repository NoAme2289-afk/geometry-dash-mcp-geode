#!/usr/bin/env python3
"""
MCP Server wrapper with logging
"""
import sys
import datetime

# Log to file
log_file = r"C:\Users\danys\Desktop\geometry-dash-mcp\mcp-debug.log"

with open(log_file, "a") as f:
    f.write(f"\n\n=== MCP Server Started at {datetime.datetime.now()} ===\n")
    f.write(f"Python: {sys.executable}\n")
    f.write(f"Args: {sys.argv}\n")
    f.flush()

try:
    # Import and run the actual server
    import asyncio
    import sys
    sys.path.insert(0, r"C:\Users\danys\Desktop\geometry-dash-mcp")
    
    # Import the serve function
    from mcp_server import serve
    
    with open(log_file, "a") as f:
        f.write("Starting asyncio.run(serve())...\n")
        f.flush()
    
    asyncio.run(serve())
    
except Exception as e:
    with open(log_file, "a") as f:
        f.write(f"ERROR: {e}\n")
        import traceback
        f.write(traceback.format_exc())
        f.flush()
    raise
