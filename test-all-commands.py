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

print("=== Testing GD-MCP Commands ===\n")

# Test 1: Create objects with groups
print("1. Creating objects with groups...")
send_command("LOAD_LEVEL:1,100,100,1:2:3,0;2,200,100,1,0;7,300,100,2,0")

input("\nPress Enter to continue to next test...")

# Test 2: Create Move Trigger
print("\n2. Creating Move Trigger...")
send_command("MOVE_TRIGGER:50,150,1,100,50,1.0,0")

input("\nPress Enter to continue to next test...")

# Test 3: Create Alpha Trigger
print("\n3. Creating Alpha Trigger...")
send_command("ALPHA_TRIGGER:50,200,1,0.5,1.0,0")

input("\nPress Enter to continue to next test...")

# Test 4: Create Rotate Trigger
print("\n4. Creating Rotate Trigger...")
send_command("ROTATE_TRIGGER:50,250,1,90,1.0,0,0,0")

input("\nPress Enter to continue to next test...")

# Test 5: Create Toggle Trigger
print("\n5. Creating Toggle Trigger...")
send_command("TOGGLE_TRIGGER:50,300,1,1")

input("\nPress Enter to continue to next test...")

# Test 6: Move objects in group
print("\n6. Moving objects in group 1...")
send_command("MOVE_OBJECTS:1,400,100")

input("\nPress Enter to continue to next test...")

# Test 7: Delete objects in group
print("\n7. Deleting objects in group 2...")
send_command("DELETE_OBJECTS:2")

print("\n=== All tests completed! ===")
print("\nCheck the MCP panel in GD editor for logs.")
