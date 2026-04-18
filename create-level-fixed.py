import win32pipe
import win32file
import time
import math

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
        print(f"[OK] {command[:70]}...")
        return True
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

print("=== Creating Beautiful Level (Fixed Coordinates) ===\n")

# Clear everything first
print("0. Clearing level...")
send_command("DELETE_ALL:")
time.sleep(0.3)

# Ground level
GROUND = 165

# === SCENE 1: Rotating platform ===
print("\n=== Scene 1: Rotating Platform (x=600) ===")

# Trigger FIRST (before objects)
send_command("ROTATE_TRIGGER:300,200,1,360,4.0,0,0,0")
time.sleep(0.1)

# Create platform blocks in a circle (Group 1)
center_x, center_y = 600, 300
radius = 80
for i in range(8):
    angle = i * 45
    x = center_x + radius * math.cos(math.radians(angle))
    y = center_y + radius * math.sin(math.radians(angle))
    send_command(f"LOAD_LEVEL:1,{x:.0f},{y:.0f},1,1")
    time.sleep(0.05)

# === SCENE 2: Pulsing blocks ===
print("\n=== Scene 2: Pulsing Blocks (x=1000) ===")

# Triggers FIRST
send_command("PULSE_TRIGGER:900,200,2,255,100,255,0.5,1.0,0.5")
time.sleep(0.1)
send_command("SCALE_TRIGGER:900,250,2,1.5,1.5,2.0,2")
time.sleep(0.1)

# Create blocks on ground (Group 2)
for i in range(5):
    x = 1000 + i * 60
    y = GROUND
    send_command(f"LOAD_LEVEL:1,{x},{y},2,2")
    time.sleep(0.05)

# === SCENE 3: Moving wave ===
print("\n=== Scene 3: Moving Wave (x=1500) ===")

# Trigger FIRST
send_command("MOVE_TRIGGER:1400,200,3,0,150,3.0,2")
time.sleep(0.1)

# Create wave pattern (Group 3)
for i in range(8):
    x = 1500 + i * 50
    y = GROUND + 50 * math.sin(i * 0.8)
    send_command(f"LOAD_LEVEL:1,{x:.0f},{y:.0f},3,3")
    time.sleep(0.05)

# === SCENE 4: Fading blocks ===
print("\n=== Scene 4: Fading Blocks (x=2100) ===")

# Trigger FIRST
send_command("ALPHA_TRIGGER:2000,200,4,0.2,2.0,1")
time.sleep(0.1)

# Create blocks (Group 4)
for i in range(6):
    x = 2100 + i * 50
    y = GROUND
    send_command(f"LOAD_LEVEL:1,{x},{y},4,0")
    time.sleep(0.05)

# === SCENE 5: Spawn effect ===
print("\n=== Scene 5: Delayed Spawn (x=2600) ===")

# Triggers FIRST
send_command("TOGGLE_TRIGGER:2500,200,5,0")
time.sleep(0.1)
send_command("SPAWN_TRIGGER:2550,200,5,2.0")
time.sleep(0.1)

# Create spikes (Group 5)
for i in range(4):
    x = 2600 + i * 60
    y = GROUND + 30
    send_command(f"LOAD_LEVEL:7,{x},{y},5,0")
    time.sleep(0.05)

# === SCENE 6: Complex movement ===
print("\n=== Scene 6: Complex Movement (x=3100) ===")

# Triggers FIRST
send_command("MOVE_TRIGGER:3000,200,6,150,0,1.5,0")
time.sleep(0.1)

# Create square (Group 6)
for row in range(3):
    for col in range(3):
        x = 3100 + col * 50
        y = GROUND + row * 50
        send_command(f"LOAD_LEVEL:1,{x},{y},6,4")
        time.sleep(0.05)

# === SCENE 7: Rainbow rotation ===
print("\n=== Scene 7: Rainbow Blocks (x=3700) ===")

# Trigger FIRST
send_command("ROTATE_TRIGGER:3600,200,7,180,2.0,1,0,0")
time.sleep(0.1)

# Create rainbow blocks (Group 7)
colors = [1, 2, 3, 4, 5]
for i, color in enumerate(colors):
    x = 3700 + i * 60
    y = GROUND
    send_command(f"LOAD_LEVEL:1,{x},{y},7,{color}")
    time.sleep(0.05)

# === SCENE 8: Simple move test ===
print("\n=== Scene 8: Simple Move Test (x=4200) ===")

# Trigger FIRST
send_command("MOVE_TRIGGER:4100,200,8,0,200,2.0,2")
time.sleep(0.1)

# Create test blocks (Group 8)
for i in range(3):
    x = 4200 + i * 60
    y = GROUND
    send_command(f"LOAD_LEVEL:2,{x},{y},8,0")
    time.sleep(0.05)

print("\n=== Level Creation Complete! ===")
print(f"\nGround level: Y={GROUND}")
print("\nScenes:")
print("1. x=600  - Rotating platform")
print("2. x=1000 - Pulsing blocks")
print("3. x=1500 - Moving wave")
print("4. x=2100 - Fading blocks")
print("5. x=2600 - Delayed spawn")
print("6. x=3100 - Complex movement")
print("7. x=3700 - Rainbow rotation")
print("8. x=4200 - Simple move test")
print("\nAll triggers are placed BEFORE their target objects!")
print("Test the level to see all effects!")
