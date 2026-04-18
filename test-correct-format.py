import win32pipe
import win32file
import pywintypes

try:
    pipe = win32file.CreateFile(
        r'\\.\pipe\gd_mcp_geode_pipe',
        win32file.GENERIC_WRITE,
        0, None,
        win32file.OPEN_EXISTING,
        0, None
    )
    
    # Pravil'nyy format: LOAD_LEVEL:id,x,y;id,x,y;id,x,y
    command = b'LOAD_LEVEL:1,100,100;2,200,100;1,300,100;7,400,100'
    win32file.WriteFile(pipe, command)
    
    print(f"Sent: {command.decode()}")
    print("Bloki dolzhny poyavit'sya v redaktore!")
    
    win32file.CloseHandle(pipe)
    
except pywintypes.error as e:
    print(f"ERROR: {e}")
    print("Proveryte chto GD zapushen i mod vklyuchen")
