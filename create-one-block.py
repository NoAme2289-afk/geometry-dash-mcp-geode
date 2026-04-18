import win32pipe
import win32file

def send_command(command):
    try:
        pipe = win32file.CreateFile(
            r'\\.\pipe\gd_mcp_geode_pipe',
            win32file.GENERIC_WRITE,
            0, None,
            win32file.OPEN_EXISTING,
            0, None
        )
        
        win32file.WriteFile(pipe, command.encode())
        win32file.CloseHandle(pipe)
        print(f"[OK] Sent: {command}")
        return True
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

# Create one block at position (200, 200)
send_command("LOAD_LEVEL:1,200,200,0,0")
