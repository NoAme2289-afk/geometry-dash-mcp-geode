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
        print(f"[OK] Sent: {command}")
        return True
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

print("=== Creating Block with Move Trigger ===\n")

# Step 1: Create a block with group 1 at position (300, 300)
print("1. Creating block with group 1 at (300, 300)...")
send_command("LOAD_LEVEL:1,300,300,1,0")
time.sleep(0.5)

# Step 2: Create Move Trigger at position (150, 300)
# Move 200 units to the right (forward), 0 units up
# Duration: 2 seconds, Easing: 0 (none)
print("2. Creating Move Trigger at (150, 300)...")
print("   - Target Group: 1")
print("   - Move: 200 units forward (right)")
print("   - Duration: 2 seconds")
print("   - Easing: 0 (none)")
send_command("MOVE_TRIGGER:150,300,1,200,0,2.0,0")
time.sleep(0.5)

print("\n=== Setup Complete! ===")
print("\nIn GD Editor:")
print("- Block at (300, 300) with group 1")
print("- Move Trigger at (150, 300)")
print("- Test the level - block should move 200 units right over 2 seconds!")
