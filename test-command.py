import win32pipe
import win32file
import pywintypes

try:
    # Подключаемся к Named Pipe
    pipe = win32file.CreateFile(
        r'\\.\pipe\gd_mcp_geode_pipe',
        win32file.GENERIC_WRITE,
        0,
        None,
        win32file.OPEN_EXISTING,
        0,
        None
    )
    
    # Отправляем команду добавить блок
    command = b'ADD_OBJECT|1|300|300'
    win32file.WriteFile(pipe, command)
    
    print("OK: Komanda otpravlena: ADD_OBJECT|1|300|300")
    print("Blok dolzhen poyavitsya v redaktore na pozicii (300, 300)!")
    
    win32file.CloseHandle(pipe)
    
except pywintypes.error as e:
    print(f"ERROR: {e}")
    print("Ubedityes chto:")
    print("1. Geometry Dash zapushen")
    print("2. Mod GD-MCP vklyuchen v Geode")
    print("3. Vy nahodites v redaktore urovnya")
