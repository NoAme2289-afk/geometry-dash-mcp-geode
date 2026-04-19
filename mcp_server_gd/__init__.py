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
    def gd_create_text(x: float, y: float, text: str, scale: float = 1.0, groups: str = "0") -> str:
        """Create a text object in the level
        
        Args:
            x: X position
            y: Y position
            text: Text content
            scale: Text scale (default 1.0)
            groups: Groups separated by colon
        """
        command = f"CREATE_TEXT:{x},{y},{text},{scale},{groups}"
        if send_command(command):
            return f"Created text '{text}' at ({x}, {y})"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_create_item_counter(x: float, y: float, itemId: int, groups: str = "0") -> str:
        """Create an item counter display
        
        Args:
            x: X position
            y: Y position
            itemId: Item ID to count
            groups: Groups separated by colon
        """
        command = f"CREATE_ITEM_COUNTER:{x},{y},{itemId},{groups}"
        if send_command(command):
            return f"Created item counter for item {itemId} at ({x}, {y})"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_create_collision_block(x: float, y: float, width: float, height: float, blockId: int, groups: str = "0") -> str:
        """Create a collision block
        
        Args:
            x: X position
            y: Y position
            width: Block width
            height: Block height
            blockId: Block ID for collision detection
            groups: Groups separated by colon
        """
        command = f"CREATE_COLLISION_BLOCK:{x},{y},{width},{height},{blockId},{groups}"
        if send_command(command):
            return f"Created collision block at ({x}, {y})"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_create_teleport_portal(x: float, y: float, targetX: float, targetY: float, groups: str = "0") -> str:
        """Create a teleport portal
        
        Args:
            x: Portal X position
            y: Portal Y position
            targetX: Target X position
            targetY: Target Y position
            groups: Groups separated by colon
        """
        command = f"CREATE_TELEPORT_PORTAL:{x},{y},{targetX},{targetY},{groups}"
        if send_command(command):
            return f"Created teleport portal at ({x}, {y}) to ({targetX}, {targetY})"
        else:
            return "Failed to send command"

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
    def gd_pulse_trigger(x: float, y: float, targetGroup: int, r: int, g: int, b: int, fadeIn: float, hold: float, fadeOut: float) -> str:
        """Create a Pulse Trigger to pulse color of objects
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            r: Red (0-255)
            g: Green (0-255)
            b: Blue (0-255)
            fadeIn: Fade in time in seconds
            hold: Hold time in seconds
            fadeOut: Fade out time in seconds
        """
        command = f"PULSE_TRIGGER:{x},{y},{targetGroup},{r},{g},{b},{fadeIn},{hold},{fadeOut}"
        if send_command(command):
            return f"Created Pulse Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_color_trigger(x: float, y: float, colorChannel: int, r: int, g: int, b: int, duration: float, blending: int = 0) -> str:
        """Create a Color Trigger to change color channel
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            colorChannel: Color channel ID (1-999)
            r: Red (0-255)
            g: Green (0-255)
            b: Blue (0-255)
            duration: Duration in seconds
            blending: Blending mode (0=normal, 1=additive)
        """
        command = f"COLOR_TRIGGER:{x},{y},{colorChannel},{r},{g},{b},{duration},{blending}"
        if send_command(command):
            return f"Created Color Trigger for channel {colorChannel}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_follow_trigger(x: float, y: float, targetGroup: int, followGroup: int, xMod: float = 1.0, yMod: float = 1.0, duration: float = 0.0) -> str:
        """Create a Follow Trigger to make one group follow another
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID (group that will move)
            followGroup: Follow group ID (group to follow)
            xMod: X modifier (default 1.0)
            yMod: Y modifier (default 1.0)
            duration: Duration in seconds (0 = instant)
        """
        command = f"FOLLOW_TRIGGER:{x},{y},{targetGroup},{followGroup},{xMod},{yMod},{duration}"
        if send_command(command):
            return f"Created Follow Trigger: group {targetGroup} follows group {followGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_spawn_trigger(x: float, y: float, targetGroup: int, delay: float = 0.0) -> str:
        """Create a Spawn Trigger to spawn objects in a group
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID to spawn
            delay: Delay in seconds before spawning
        """
        command = f"SPAWN_TRIGGER:{x},{y},{targetGroup},{delay}"
        if send_command(command):
            return f"Created Spawn Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_stop_trigger(x: float, y: float, targetGroup: int) -> str:
        """Create a Stop Trigger to stop all triggers affecting a group
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID to stop
        """
        command = f"STOP_TRIGGER:{x},{y},{targetGroup}"
        if send_command(command):
            return f"Created Stop Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_toggle_trigger(x: float, y: float, targetGroup: int, activate: int = 1) -> str:
        """Create a Toggle Trigger to show/hide objects in a group
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            activate: 1 to show, 0 to hide
        """
        command = f"TOGGLE_TRIGGER:{x},{y},{targetGroup},{activate}"
        if send_command(command):
            return f"Created Toggle Trigger for group {targetGroup}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_shake_trigger(x: float, y: float, strength: float, interval: float, duration: float) -> str:
        """Create a Shake Trigger to shake the camera
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            strength: Shake strength (0.0-1.0)
            interval: Shake interval in seconds
            duration: Duration in seconds
        """
        command = f"SHAKE_TRIGGER:{x},{y},{strength},{interval},{duration}"
        if send_command(command):
            return f"Created Shake Trigger with strength {strength}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_animate_trigger(x: float, y: float, targetGroup: int, animationId: int) -> str:
        """Create an Animate Trigger to animate objects
        
        Args:
            x: Trigger X position
            y: Trigger Y position
            targetGroup: Target group ID
            animationId: Animation ID (1-10)
        """
        command = f"ANIMATE_TRIGGER:{x},{y},{targetGroup},{animationId}"
        if send_command(command):
            return f"Created Animate Trigger for group {targetGroup}"
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
    def gd_copy_objects(groupId: int) -> str:
        """Copy all objects in a group to clipboard
        
        Args:
            groupId: Group ID to copy
        """
        command = f"COPY_OBJECTS:{groupId}"
        if send_command(command):
            return f"Copied objects in group {groupId}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_paste_objects(x: float, y: float) -> str:
        """Paste copied objects at position
        
        Args:
            x: X position to paste
            y: Y position to paste
        """
        command = f"PASTE_OBJECTS:{x},{y}"
        if send_command(command):
            return f"Pasted objects at ({x}, {y})"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_rotate_objects(groupId: int, degrees: float, centerX: float = 0.0, centerY: float = 0.0) -> str:
        """Rotate all objects in a group around a center point
        
        Args:
            groupId: Group ID to rotate
            degrees: Rotation degrees
            centerX: Center X position (default 0)
            centerY: Center Y position (default 0)
        """
        command = f"ROTATE_OBJECTS:{groupId},{degrees},{centerX},{centerY}"
        if send_command(command):
            return f"Rotated objects in group {groupId} by {degrees} degrees"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_scale_objects(groupId: int, scaleX: float, scaleY: float, centerX: float = 0.0, centerY: float = 0.0) -> str:
        """Scale all objects in a group from a center point
        
        Args:
            groupId: Group ID to scale
            scaleX: Scale X factor
            scaleY: Scale Y factor
            centerX: Center X position (default 0)
            centerY: Center Y position (default 0)
        """
        command = f"SCALE_OBJECTS:{groupId},{scaleX},{scaleY},{centerX},{centerY}"
        if send_command(command):
            return f"Scaled objects in group {groupId}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_flip_objects(groupId: int, flipX: int = 1, flipY: int = 0) -> str:
        """Flip all objects in a group horizontally and/or vertically
        
        Args:
            groupId: Group ID to flip
            flipX: Flip horizontally (1=yes, 0=no)
            flipY: Flip vertically (1=yes, 0=no)
        """
        command = f"FLIP_OBJECTS:{groupId},{flipX},{flipY}"
        if send_command(command):
            return f"Flipped objects in group {groupId}"
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
    def gd_get_objects_by_group(groupId: int) -> str:
        """Get all objects in a specific group
        
        Args:
            groupId: Group ID to query
        """
        command = f"GET_OBJECTS_BY_GROUP:{groupId}"
        if send_command(command):
            return f"Requested objects in group {groupId}. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_get_triggers_by_type(triggerType: str) -> str:
        """Get all triggers of a specific type
        
        Args:
            triggerType: Trigger type (move, rotate, scale, alpha, pulse, color, etc.)
        """
        command = f"GET_TRIGGERS_BY_TYPE:{triggerType}"
        if send_command(command):
            return f"Requested {triggerType} triggers. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_validate_level() -> str:
        """Validate the level for common errors and issues
        
        Returns validation report with warnings and errors.
        """
        command = "VALIDATE_LEVEL:"
        if send_command(command):
            return "Level validation requested. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_get_level_statistics() -> str:
        """Get detailed statistics about the level
        
        Returns comprehensive statistics including object counts, trigger counts, group usage, etc.
        """
        command = "GET_LEVEL_STATISTICS:"
        if send_command(command):
            return "Level statistics requested. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_batch_create(objectId: int, positions: str, groups: str = "0", colorChannel: int = 0) -> str:
        """Batch create multiple objects at once
        
        Args:
            objectId: Object ID to create
            positions: Positions as "x1,y1;x2,y2;x3,y3"
            groups: Groups separated by colon
            colorChannel: Color channel
        """
        command = f"BATCH_CREATE:{objectId},{positions},{groups},{colorChannel}"
        if send_command(command):
            return f"Batch created objects with ID {objectId}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_pattern_generator(patternType: str, startX: float, startY: float, count: int, spacing: float, objectId: int = 1) -> str:
        """Generate pattern of objects
        
        Args:
            patternType: Pattern type (line, circle, grid, wave)
            startX: Start X position
            startY: Start Y position
            count: Number of objects
            spacing: Spacing between objects
            objectId: Object ID to use (default 1=block)
        """
        command = f"PATTERN_GENERATOR:{patternType},{startX},{startY},{count},{spacing},{objectId}"
        if send_command(command):
            return f"Generated {patternType} pattern with {count} objects"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_import_level(json_data: str) -> str:
        """Import level from JSON data
        
        Args:
            json_data: JSON string containing level data
        """
        command = f"IMPORT_LEVEL:{json_data}"
        if send_command(command):
            return "Level import started. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_optimize_level(remove_duplicates: bool = True, remove_invisible: bool = True, optimize_triggers: bool = True) -> str:
        """Optimize level by removing duplicates, invisible objects, and optimizing triggers
        
        Args:
            remove_duplicates: Remove duplicate objects at same position
            remove_invisible: Remove objects outside level bounds
            optimize_triggers: Optimize and merge similar triggers
        """
        command = f"OPTIMIZE_LEVEL:{int(remove_duplicates)},{int(remove_invisible)},{int(optimize_triggers)}"
        if send_command(command):
            return "Level optimization started. Check MCP panel for results."
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_save_version(comment: str) -> str:
        """Save current level as a version with comment
        
        Args:
            comment: Description of this version
        """
        import os
        import json
        from datetime import datetime
        
        # Export current level
        if send_command("EXPORT_LEVEL"):
            time.sleep(0.5)
            
            try:
                handle = win32file.CreateFile(
                    PIPE_NAME,
                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                    0, None,
                    win32file.OPEN_EXISTING,
                    0, None
                )
                
                result, data = win32file.ReadFile(handle, 64 * 1024)
                win32file.CloseHandle(handle)
                
                json_data = data.decode('utf-8').strip()
                
                # Create versions directory
                versions_dir = os.path.join(os.path.expanduser("~"), "Desktop", "gd_versions")
                os.makedirs(versions_dir, exist_ok=True)
                
                # Save version with timestamp
                timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                version_file = os.path.join(versions_dir, f"version_{timestamp}.json")
                
                version_data = {
                    "timestamp": timestamp,
                    "comment": comment,
                    "level_data": json.loads(json_data)
                }
                
                with open(version_file, 'w') as f:
                    json.dump(version_data, f, indent=2)
                
                return f"Version saved: {version_file}"
                
            except Exception as e:
                return f"Failed to save version: {str(e)}"
        else:
            return "Failed to export level"

    @mcp.tool()
    def gd_list_versions() -> str:
        """List all saved versions of the level"""
        import os
        import json
        
        versions_dir = os.path.join(os.path.expanduser("~"), "Desktop", "gd_versions")
        
        if not os.path.exists(versions_dir):
            return "No versions found. Use gd_save_version to create one."
        
        versions = []
        for filename in sorted(os.listdir(versions_dir), reverse=True):
            if filename.endswith('.json'):
                filepath = os.path.join(versions_dir, filename)
                try:
                    with open(filepath, 'r') as f:
                        data = json.load(f)
                        versions.append(f"{filename}: {data.get('comment', 'No comment')}")
                except:
                    pass
        
        if versions:
            return "Versions:\n" + "\n".join(versions[:10])  # Show last 10
        else:
            return "No versions found"

    @mcp.tool()
    def gd_restore_version(version_filename: str) -> str:
        """Restore level from a saved version
        
        Args:
            version_filename: Filename of version to restore (e.g. version_20260419_141530.json)
        """
        import os
        import json
        
        versions_dir = os.path.join(os.path.expanduser("~"), "Desktop", "gd_versions")
        version_file = os.path.join(versions_dir, version_filename)
        
        if not os.path.exists(version_file):
            return f"Version file not found: {version_filename}"
        
        try:
            with open(version_file, 'r') as f:
                version_data = json.load(f)
            
            level_json = json.dumps(version_data['level_data'])
            command = f"IMPORT_LEVEL:{level_json}"
            
            if send_command(command):
                return f"Restored version: {version_data.get('comment', 'No comment')}"
            else:
                return "Failed to send restore command"
                
        except Exception as e:
            return f"Failed to restore version: {str(e)}"

    @mcp.tool()
    def gd_export_section(startX: float, endX: float, filename: str) -> str:
        """Export a section of the level (X range) to file
        
        Args:
            startX: Start X position
            endX: End X position
            filename: Output filename (will be saved to Desktop)
        """
        command = f"EXPORT_SECTION:{startX},{endX},{filename}"
        if send_command(command):
            return f"Section exported to Desktop/{filename}"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_import_section(filename: str, offsetX: float, offsetY: float = 0.0) -> str:
        """Import a section from file at specified position
        
        Args:
            filename: Input filename (from Desktop)
            offsetX: X offset for imported objects
            offsetY: Y offset for imported objects (default 0)
        """
        command = f"IMPORT_SECTION:{filename},{offsetX},{offsetY}"
        if send_command(command):
            return f"Section imported from {filename} at offset ({offsetX}, {offsetY})"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_merge_levels(level1_file: str, level2_file: str, mode: str = "horizontal") -> str:
        """Merge two level files together
        
        Args:
            level1_file: First level JSON file (from Desktop)
            level2_file: Second level JSON file (from Desktop)
            mode: Merge mode - 'horizontal', 'vertical', or 'overlay'
        """
        command = f"MERGE_LEVELS:{level1_file},{level2_file},{mode}"
        if send_command(command):
            return f"Levels merged in {mode} mode"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_auto_backup_start(interval_minutes: int = 5) -> str:
        """Start automatic backup system
        
        Args:
            interval_minutes: Backup interval in minutes (default 5)
        """
        command = f"AUTO_BACKUP_START:{interval_minutes}"
        if send_command(command):
            return f"Auto-backup started (every {interval_minutes} minutes)"
        else:
            return "Failed to send command"

    @mcp.tool()
    def gd_auto_backup_stop() -> str:
        """Stop automatic backup system"""
        command = "AUTO_BACKUP_STOP:"
        if send_command(command):
            return "Auto-backup stopped"
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
