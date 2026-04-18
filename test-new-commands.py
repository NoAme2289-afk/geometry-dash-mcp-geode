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

print("=== Testing New Commands ===\n")

# Test 1: Create objects with colors
print("1. Creating objects with different colors...")
send_command("LOAD_LEVEL:1,100,100,0,1;1,200,100,0,2;1,300,100,0,3")
time.sleep(1)

# Test 2: Create a trigger
print("\n2. Creating Toggle Trigger at (50,150)...")
send_command("TOGGLE_TRIGGER:50,150,1,0")
time.sleep(1)

# Test 3: Edit trigger property
print("\n3. Editing trigger - set activate to true...")
send_command("EDIT_TRIGGER:50,150,activate,1")
time.sleep(1)

# Test 4: Edit trigger - change target group
print("\n4. Editing trigger - change targetGroup to 5...")
send_command("EDIT_TRIGGER:50,150,targetGroup,5")
time.sleep(1)

# Test 5: Delete all objects
print("\n5. Deleting all objects...")
send_command("DELETE_ALL:")
time.sleep(1)

print("\n=== All tests completed! ===")
print("\nCheck the MCP panel in GD editor for logs.")
