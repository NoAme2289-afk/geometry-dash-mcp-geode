"""
MCP server for Geometry Dash.
Provides tools for controlling Geometry Dash editor via named pipe.
"""

__all__ = ("main",)

import win32file
from mcp.server.fastmcp import FastMCP

PIPE_NAME = r'\\.\pipe\gd_mcp_geode_pipe'

def send_command(command: str) -> bool:
    """Send command to Geometry Dash via named pipe"""
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
    except Exception:
        return False

def main() -> int:
    """Main entry point for Geometry Dash MCP server"""
    mcp = FastMCP("geometry-dash-mcp")

    @mcp.tool()
    def gd_create_object(objectId: int, x: float, y: float, groups: str = "0", colorChannel: int = 0) -> str:
        """Create an object in Geometry Dash editor with groups and colors
        
        Args:
            objectId: Object ID (1=block, 2=spike, 7=spike, 40=platform, etc.)
            x: X position
            y: Y position
            groups: Groups separated by colon (e.g. '1:2:3'), use '0' for no groups
            colorChannel: Color channel (0-999), use 0 for default
        """
        command = f"LOAD_LEVEL:{objectId},{x},{y},{groups},{colorChannel}"
        if send_command(command):
            return f"Created object {objectId} at ({x}, {y}) with groups={groups}, color={colorChannel}"
        else:
            return "Failed to send command. Make sure Geometry Dash is running and in editor."

    @mcp.tool()
    def gd_move_trigger(x: float, y: float, targetGroup: int, moveX: float, moveY: float, duration: float, easing: int = 0) -> str:
        """Create a Move Trigger to move objects in a group
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            moveX: Move distance X
            moveY: Move distance Y
            duration: Duration in seconds
            easing: Easing type (0-15)
        """
        command = f"MOVE_TRIGGER:{x},{y},{targetGroup},{moveX},{moveY},{duration},{easing}"
        if send_command(command):
            return f"Created Move Trigger at ({x}, {y}) for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_alpha_trigger(x: float, y: float, targetGroup: int, opacity: float, duration: float, easing: int = 0) -> str:
        """Create an Alpha Trigger to fade objects
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            opacity: Opacity 0.0-1.0
            duration: Duration in seconds
            easing: Easing type (0-15)
        """
        command = f"ALPHA_TRIGGER:{x},{y},{targetGroup},{opacity},{duration},{easing}"
        if send_command(command):
            return f"Created Alpha Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_rotate_trigger(x: float, y: float, targetGroup: int, degrees: float, duration: float, easing: int = 0, times: int = 0, lockRotation: int = 0) -> str:
        """Create a Rotate Trigger to rotate objects
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            degrees: Rotation degrees
            duration: Duration in seconds
            easing: Easing type (0-15)
            times: Times 360 (for multiple rotations)
            lockRotation: Lock object rotation (0 or 1)
        """
        command = f"ROTATE_TRIGGER:{x},{y},{targetGroup},{degrees},{duration},{easing},{times},{lockRotation}"
        if send_command(command):
            return f"Created Rotate Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_scale_trigger(x: float, y: float, targetGroup: int, scaleX: float, scaleY: float, duration: float, easing: int = 0) -> str:
        """Create a Scale Trigger to scale objects
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            scaleX: Scale X factor
            scaleY: Scale Y factor
            duration: Duration in seconds
            easing: Easing type (0-15)
        """
        command = f"SCALE_TRIGGER:{x},{y},{targetGroup},{scaleX},{scaleY},{duration},{easing}"
        if send_command(command):
            return f"Created Scale Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_delete_all() -> str:
        """Delete all objects from the level"""
        if send_command("DELETE_ALL:"):
            return "Deleted all objects from level"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_delete_objects(groupId: int) -> str:
        """Delete all objects in a specific group
        
        Args:
            groupId: Group ID to delete
        """
        command = f"DELETE_OBJECTS:{groupId}"
        if send_command(command):
            return f"Deleted objects in group {groupId}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_move_objects(groupId: int, offsetX: float, offsetY: float) -> str:
        """Move all objects in a group by offset
        
        Args:
            groupId: Group ID to move
            offsetX: X offset
            offsetY: Y offset
        """
        command = f"MOVE_OBJECTS:{groupId},{offsetX},{offsetY}"
        if send_command(command):
            return f"Moved objects in group {groupId}"
        else:
            return "Failed to send command"
    
    @mcp.tool()
    def gd_get_level_info() -> str:
        """Get information about the current level (object counts by type)
        
        Returns statistics about the level including total objects, blocks, spikes, portals, orbs, triggers, and decorations.
        """
        command = "GET_LEVEL_INFO:"
        if send_command(command):
            return "Level info requested. Check Geometry Dash MCP panel for results."
        else:
            return "Failed to send command"
    
    @mcp.tool()
    def gd_list_objects() -> str:
        """List all objects in the current level (first 50 objects)
        
        Returns a list of objects with their IDs and positions.
        """
        command = "LIST_OBJECTS:"
        if send_command(command):
            return "Object list requested. Check Geometry Dash MCP panel for results."
        else:
            return "Failed to send command"
    
    @mcp.tool()
    def gd_undo() -> str:
        """Undo the last action in the editor"""
        command = "UNDO:"
        if send_command(command):
            return "Undo action sent"
        else:
            return "Failed to send command"
    
    @mcp.tool()
    def gd_redo() -> str:
        """Redo the last undone action in the editor"""
        command = "REDO:"
        if send_command(command):
            return "Redo action sent"
        else:
            return "Failed to send command"
    
    @mcp.tool()
    def gd_export_level() -> str:
        """Export the current level data to JSON (check MCP panel in GD for output)"""
        command = "EXPORT_LEVEL:"
        if send_command(command):
            return "Export level command sent. Check the MCP panel in Geometry Dash for JSON output."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_render_level(output_path: str = None) -> str:
        """Render the current Geometry Dash level to PNG image with grid
        
        Args:
            output_path: Optional output path for PNG file (default: Desktop/gd_level_render.png)
        
        Returns:
            Path to rendered PNG file
        """
        from .renderer import render_level
        import os
        
        # Send EXPORT_LEVEL command to get JSON data
        if send_command("EXPORT_LEVEL"):
            # Wait for response
            time.sleep(0.5)
            
            # Read response from pipe
            try:
                handle = win32file.CreateFile(
                    PIPE_NAME,
                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                    0, None,
                    win32file.OPEN_EXISTING,
                    0, None
                )
                
                # Read JSON data
                result, data = win32file.ReadFile(handle, 64 * 1024)
                win32file.CloseHandle(handle)
                
                json_data = data.decode('utf-8').strip()
                
                # Render level
                if output_path is None:
                    output_path = os.path.join(os.path.expanduser("~"), "Desktop", "gd_level_render.png")
                
                rendered_path = render_level(json_data, output_path)
                return f"Level rendered successfully: {rendered_path}"
                
            except Exception as e:
                return f"Failed to render level: {str(e)}"
        else:
            return "Failed to send EXPORT_LEVEL command"

    mcp.run(transport="stdio")
    return 0
