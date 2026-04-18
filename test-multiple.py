import win32pipe
import win32file
import pywintypes
import time

commands = [
    b'ADD_OBJECT|1|100|100',  # Blok
    b'ADD_OBJECT|2|200|100',  # Spike
    b'ADD_OBJECT|1|300|100',  # Blok
    b'ADD_OBJECT|7|400|100',  # Platform
]

for cmd in commands:
    try:
        pipe = win32file.CreateFile(
            r'\\.\pipe\gd_mcp_geode_pipe',
            win32file.GENERIC_WRITE,
            0, None,
            win32file.OPEN_EXISTING,
            0, None
        )
        
        win32file.WriteFile(pipe, cmd)
        print(f"Sent: {cmd.decode()}")
        win32file.CloseHandle(pipe)
        time.sleep(0.1)
        
    except pywintypes.error as e:
        print(f"ERROR: {e}")
        break

print("\nVse komandy otpravleny! Proveryte redaktor GD.")
