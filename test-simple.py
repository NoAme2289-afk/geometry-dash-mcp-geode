import win32pipe
import win32file
import time

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
        print(f"[OK] {command}")
        return True
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

print("=== Creating Simple Test Level ===\n")

# Create 3 blocks with group 1
print("Creating 3 blocks with group 1...")
send_command("LOAD_LEVEL:1,300,200,1,0")
time.sleep(0.1)
send_command("LOAD_LEVEL:1,400,200,1,0")
time.sleep(0.1)
send_command("LOAD_LEVEL:1,500,200,1,0")
time.sleep(0.2)

# Create Move Trigger to move them
print("\nCreating Move Trigger to move blocks right...")
send_command("MOVE_TRIGGER:200,200,1,200,0,2.0,0")
time.sleep(0.2)

print("\n=== Done! ===")
print("Test the level in GD - blocks should move 200 units right over 2 seconds!")
