"""
Test script to render Geometry Dash level
"""
import win32file
import win32pipe
import time
from mcp_server_gd.renderer import render_level

PIPE_NAME = r"\\.\pipe\gd_mcp_geode_pipe"

def send_command(command):
    """Send command to Geometry Dash"""
    try:
        handle = win32file.CreateFile(
            PIPE_NAME,
            win32file.GENERIC_READ | win32file.GENERIC_WRITE,
            0, None,
            win32file.OPEN_EXISTING,
            0, None
        )
        
        win32file.WriteFile(handle, command.encode('utf-8'))
        win32file.CloseHandle(handle)
        return True
    except Exception as e:
        print(f"Error sending command: {e}")
        return False

def get_level_data():
    """Get level data from Geometry Dash"""
    try:
        # Send EXPORT_LEVEL command
        if not send_command("EXPORT_LEVEL"):
            return None
        
        # Wait a bit for processing
        time.sleep(0.5)
        
        # Read response
        handle = win32file.CreateFile(
            PIPE_NAME,
            win32file.GENERIC_READ | win32file.GENERIC_WRITE,
            0, None,
            win32file.OPEN_EXISTING,
            0, None
        )
        
        result, data = win32file.ReadFile(handle, 64 * 1024)
        win32file.CloseHandle(handle)
        
        return data.decode('utf-8').strip()
        
    except Exception as e:
        print(f"Error getting level data: {e}")
        return None

if __name__ == "__main__":
    print("Getting level data from Geometry Dash...")
    json_data = get_level_data()
    
    if json_data:
        print(f"Received data: {json_data[:200]}...")
        print("\nRendering level...")
        
        output_path = r"C:\Users\danys\Desktop\gd_level_render.png"
        rendered_path = render_level(json_data, output_path)
        
        print(f"✅ Level rendered successfully: {rendered_path}")
    else:
        print("❌ Failed to get level data. Make sure Geometry Dash is running and you're in the editor.")
