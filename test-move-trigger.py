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

# Step 1: Create a block with group 1 at position (200, 200)
print("1. Creating block with group 1 at (200, 200)...")
send_command("LOAD_LEVEL:1,200,200,1,0")
time.sleep(0.5)

# Step 2: Create Move Trigger at position (100, 200)
# Format: MOVE_TRIGGER:x,y,targetGroup,moveX,moveY,duration,easing
# This will move group 1 by 100 units right and 50 units up over 1 second
print("2. Creating Move Trigger at (100, 200)...")
print("   - Target Group: 1")
print("   - Move: 100 units right, 50 units up")
print("   - Duration: 1 second")
send_command("MOVE_TRIGGER:100,200,1,100,50,1.0,0")
time.sleep(0.5)

# Step 3: Edit the trigger to make sure targetGroup is set
print("\n3. Confirming trigger settings...")
send_command("EDIT_TRIGGER:100,200,targetGroup,1")
time.sleep(0.5)

print("\n=== Setup Complete! ===")
print("\nIn GD Editor:")
print("- You should see a block at (200, 200) with group 1")
print("- You should see a Move Trigger at (100, 200)")
print("- Test the level to see the block move!")
