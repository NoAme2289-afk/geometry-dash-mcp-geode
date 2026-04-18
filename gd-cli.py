#!/usr/bin/env python3
"""
Simple CLI wrapper for Geometry Dash commands
Usage: python gd-cli.py <command> <args>
"""

import sys
import win32file

PIPE_NAME = r'\\.\pipe\gd_mcp_geode_pipe'

def send_command(command: str) -> bool:
    try:
        pipe = win32file.CreateFile(
            PIPE_NAME,
            win32file.GENERIC_WRITE,
            0, None,
            win32file.OPEN_EXISTING,
            0, None
        )
        win32file.WriteFile(pipe, command.encode())
        win32file.CloseHandle(pipe)
        return True
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python gd-cli.py <command> <args>")
        print("\nCommands:")
        print("  create_object <id> <x> <y> [groups] [color]")
        print("  move_trigger <x> <y> <group> <moveX> <moveY> <duration> [easing]")
        print("  delete_all")
        print("  delete_objects <group>")
        return
    
    cmd = sys.argv[1]
    
    if cmd == "create_object":
        obj_id = sys.argv[2]
        x = sys.argv[3]
        y = sys.argv[4]
        groups = sys.argv[5] if len(sys.argv) > 5 else "0"
        color = sys.argv[6] if len(sys.argv) > 6 else "0"
        
        command = f"LOAD_LEVEL:{obj_id},{x},{y},{groups},{color}"
        if send_command(command):
            print(f"[OK] Created object {obj_id} at ({x}, {y})")
        else:
            print("[ERROR] Failed")
    
    elif cmd == "move_trigger":
        x = sys.argv[2]
        y = sys.argv[3]
        group = sys.argv[4]
        moveX = sys.argv[5]
        moveY = sys.argv[6]
        duration = sys.argv[7]
        easing = sys.argv[8] if len(sys.argv) > 8 else "0"
        
        command = f"MOVE_TRIGGER:{x},{y},{group},{moveX},{moveY},{duration},{easing}"
        if send_command(command):
            print(f"[OK] Created Move Trigger for group {group}")
        else:
            print("[ERROR] Failed")
    
    elif cmd == "delete_all":
        if send_command("DELETE_ALL:"):
            print("[OK] Deleted all objects")
        else:
            print("[ERROR] Failed")
    
    elif cmd == "delete_objects":
        group = sys.argv[2]
        if send_command(f"DELETE_OBJECTS:{group}"):
            print(f"[OK] Deleted objects in group {group}")
        else:
            print("[ERROR] Failed")
    
    else:
        print(f"Unknown command: {cmd}")

if __name__ == "__main__":
    main()
