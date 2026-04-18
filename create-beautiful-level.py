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
        print(f"[OK] {command[:60]}...")
        return True
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

print("=== Creating Beautiful Level with Triggers ===\n")

# Clear everything first
print("0. Clearing level...")
send_command("DELETE_ALL:")
time.sleep(0.3)

# === SCENE 1: Rotating platform with blocks ===
print("\n=== Scene 1: Rotating Platform (Group 1) ===")

# Create platform blocks in a circle (Group 1)
for i in range(8):
    angle = i * 45  # 8 blocks in circle
    import math
    x = 500 + 100 * math.cos(math.radians(angle))
    y = 300 + 100 * math.sin(math.radians(angle))
    send_command(f"LOAD_LEVEL:1,{x},{y},1,1")
    time.sleep(0.05)

# Rotate trigger - spin the platform
send_command("ROTATE_TRIGGER:100,300,1,360,4.0,0,0,0")
time.sleep(0.2)

# === SCENE 2: Pulsing blocks (Group 2) ===
print("\n=== Scene 2: Pulsing Blocks (Group 2) ===")

# Create a row of blocks (Group 2)
for i in range(5):
    x = 900 + i * 60
    y = 300
    send_command(f"LOAD_LEVEL:1,{x},{y},2,2")
    time.sleep(0.05)

# Pulse trigger - make them glow
send_command("PULSE_TRIGGER:800,300,2,255,100,255,0.5,1.0,0.5")
time.sleep(0.2)

# Scale trigger - make them grow and shrink
send_command("SCALE_TRIGGER:800,350,2,1.5,1.5,2.0,2")
time.sleep(0.2)

# === SCENE 3: Moving wave (Group 3) ===
print("\n=== Scene 3: Moving Wave (Group 3) ===")

# Create wave pattern blocks (Group 3)
for i in range(10):
    x = 1400 + i * 40
    y = 250 + 50 * math.sin(i * 0.5)
    send_command(f"LOAD_LEVEL:1,{x},{y},3,3")
    time.sleep(0.05)

# Move trigger - wave moves up
send_command("MOVE_TRIGGER:1300,300,3,0,150,3.0,2")
time.sleep(0.2)

# === SCENE 4: Fading blocks (Group 4) ===
print("\n=== Scene 4: Fading Blocks (Group 4) ===")

# Create blocks that will fade (Group 4)
for i in range(6):
    x = 2000 + i * 50
    y = 300
    send_command(f"LOAD_LEVEL:1,{x},{y},4,0")
    time.sleep(0.05)

# Alpha trigger - fade out
send_command("ALPHA_TRIGGER:1900,300,4,0.2,2.0,1")
time.sleep(0.2)

# === SCENE 5: Spawn trigger showcase (Group 5) ===
print("\n=== Scene 5: Delayed Spawn (Group 5) ===")

# Create blocks that spawn later (Group 5)
for i in range(4):
    x = 2500 + i * 60
    y = 350
    send_command(f"LOAD_LEVEL:7,{x},{y},5,0")  # Spikes
    time.sleep(0.05)

# Toggle trigger - disable group 5 initially
send_command("TOGGLE_TRIGGER:2400,300,5,0")
time.sleep(0.1)

# Spawn trigger - enable after 2 seconds
send_command("SPAWN_TRIGGER:2450,300,5,2.0")
time.sleep(0.2)

# === SCENE 6: Complex movement (Group 6) ===
print("\n=== Scene 6: Complex Movement (Group 6) ===")

# Create a square of blocks (Group 6)
positions = [
    (3000, 250), (3050, 250), (3100, 250),
    (3000, 300), (3050, 300), (3100, 300),
    (3000, 350), (3050, 350), (3100, 350)
]
for x, y in positions:
    send_command(f"LOAD_LEVEL:1,{x},{y},6,4")
    time.sleep(0.05)

# Move trigger 1 - move right
send_command("MOVE_TRIGGER:2900,300,6,100,0,1.5,0")
time.sleep(0.1)

# Move trigger 2 - move up (delayed)
send_command("MOVE_TRIGGER:3200,300,6,0,100,1.5,0")
time.sleep(0.2)

# === SCENE 7: Color changing blocks (Group 7) ===
print("\n=== Scene 7: Rainbow Blocks (Group 7) ===")

# Create blocks with different colors
colors = [1, 2, 3, 4, 5]
for i, color in enumerate(colors):
    x = 3500 + i * 60
    y = 300
    send_command(f"LOAD_LEVEL:1,{x},{y},7,{color}")
    time.sleep(0.05)

# Rotate all of them
send_command("ROTATE_TRIGGER:3400,300,7,180,2.0,1,0,0")
time.sleep(0.2)

# === SCENE 8: Follow trigger (Group 8 follows Group 9) ===
print("\n=== Scene 8: Follow Effect (Group 8 follows 9) ===")

# Leader block (Group 9)
send_command("LOAD_LEVEL:2,4000,300,9,0")
time.sleep(0.1)

# Follower blocks (Group 8)
for i in range(3):
    x = 4000 - (i+1) * 60
    y = 300
    send_command(f"LOAD_LEVEL:1,{x},{y},8,0")
    time.sleep(0.05)

# Move leader
send_command("MOVE_TRIGGER:3900,300,9,0,150,3.0,2")
time.sleep(0.1)

# Follow trigger - group 8 follows group 9
send_command("FOLLOW_TRIGGER:3900,350,8,9,1.0,1.0,3.0")
time.sleep(0.2)

print("\n=== Level Creation Complete! ===")
print("\nCreated scenes:")
print("1. Rotating platform (360° rotation)")
print("2. Pulsing & scaling blocks")
print("3. Moving wave pattern")
print("4. Fading blocks (alpha)")
print("5. Delayed spawn with toggle")
print("6. Complex multi-step movement")
print("7. Rainbow rotating blocks")
print("8. Follow effect")
print("\nTest the level to see all effects in action!")
