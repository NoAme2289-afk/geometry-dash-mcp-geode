#!/usr/bin/env python3
"""
Geometry Dash MCP Server
Works with Geode mod via named pipe
"""

import win32file
from typing import Sequence
from mcp.server import Server
from mcp.server.stdio import stdio_server
from mcp.types import Tool, TextContent, ImageContent, EmbeddedResource

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
    except Exception as e:
        return False

async def serve() -> None:
    server = Server("geometry-dash-mcp")

    @server.list_tools()
    async def list_tools() -> list[Tool]:
        """List available Geometry Dash tools"""
        return [
            Tool(
                name="gd_create_object",
                description="Create an object in Geometry Dash editor with groups and colors",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "objectId": {
                            "type": "number",
                            "description": "Object ID (1=block, 2=spike, 7=spike, 40=platform, etc.)"
                        },
                        "x": {"type": "number", "description": "X position"},
                        "y": {"type": "number", "description": "Y position"},
                        "groups": {
                            "type": "string",
                            "description": "Groups separated by colon (e.g. '1:2:3'), use '0' for no groups",
                            "default": "0"
                        },
                        "colorChannel": {
                            "type": "number",
                            "description": "Color channel (0-999), use 0 for default",
                            "default": 0
                        }
                    },
                    "required": ["objectId", "x", "y"]
                }
            ),
            Tool(
                name="gd_move_trigger",
                description="Create a Move Trigger to move objects in a group",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "x": {"type": "number", "description": "Trigger X position"},
                        "y": {"type": "number", "description": "Trigger Y position"},
                        "targetGroup": {"type": "number", "description": "Target group ID"},
                        "moveX": {"type": "number", "description": "Move distance X"},
                        "moveY": {"type": "number", "description": "Move distance Y"},
                        "duration": {"type": "number", "description": "Duration in seconds"},
                        "easing": {"type": "number", "description": "Easing type (0-15)", "default": 0}
                    },
                    "required": ["x", "y", "targetGroup", "moveX", "moveY", "duration"]
                }
            ),
            Tool(
                name="gd_alpha_trigger",
                description="Create an Alpha Trigger to fade objects",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "x": {"type": "number"},
                        "y": {"type": "number"},
                        "targetGroup": {"type": "number"},
                        "opacity": {"type": "number", "description": "Opacity 0.0-1.0"},
                        "duration": {"type": "number"},
                        "easing": {"type": "number", "default": 0}
                    },
                    "required": ["x", "y", "targetGroup", "opacity", "duration"]
                }
            ),
            Tool(
                name="gd_rotate_trigger",
                description="Create a Rotate Trigger to rotate objects",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "x": {"type": "number"},
                        "y": {"type": "number"},
                        "targetGroup": {"type": "number"},
                        "degrees": {"type": "number", "description": "Rotation degrees"},
                        "duration": {"type": "number"},
                        "easing": {"type": "number", "default": 0},
                        "times": {"type": "number", "description": "Times 360", "default": 0},
                        "lockRotation": {"type": "number", "description": "Lock object rotation (0 or 1)", "default": 0}
                    },
                    "required": ["x", "y", "targetGroup", "degrees", "duration"]
                }
            ),
            Tool(
                name="gd_scale_trigger",
                description="Create a Scale Trigger to scale objects",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "x": {"type": "number"},
                        "y": {"type": "number"},
                        "targetGroup": {"type": "number"},
                        "scaleX": {"type": "number"},
                        "scaleY": {"type": "number"},
                        "duration": {"type": "number"},
                        "easing": {"type": "number", "default": 0}
                    },
                    "required": ["x", "y", "targetGroup", "scaleX", "scaleY", "duration"]
                }
            ),
            Tool(
                name="gd_delete_all",
                description="Delete all objects from the level",
                inputSchema={"type": "object", "properties": {}}
            ),
            Tool(
                name="gd_delete_objects",
                description="Delete all objects in a specific group",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "groupId": {"type": "number", "description": "Group ID to delete"}
                    },
                    "required": ["groupId"]
                }
            ),
            Tool(
                name="gd_move_objects",
                description="Move all objects in a group by offset",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "groupId": {"type": "number"},
                        "offsetX": {"type": "number"},
                        "offsetY": {"type": "number"}
                    },
                    "required": ["groupId", "offsetX", "offsetY"]
                }
            ),
        ]

    @server.call_tool()
    async def call_tool(
        name: str, arguments: dict
    ) -> Sequence[TextContent | ImageContent | EmbeddedResource]:
        """Handle tool calls"""
        try:
            if name == "gd_create_object":
                obj_id = arguments["objectId"]
                x = arguments["x"]
                y = arguments["y"]
                groups = arguments.get("groups", "0")
                color = arguments.get("colorChannel", 0)
                
                command = f"LOAD_LEVEL:{obj_id},{x},{y},{groups},{color}"
                success = send_command(command)
                
                if success:
                    return [TextContent(
                        type="text",
                        text=f"Created object {obj_id} at ({x}, {y}) with groups={groups}, color={color}"
                    )]
                else:
                    raise Exception("Failed to send command. Make sure Geometry Dash is running and in editor.")
            
            elif name == "gd_move_trigger":
                x = arguments["x"]
                y = arguments["y"]
                target = arguments["targetGroup"]
                moveX = arguments["moveX"]
                moveY = arguments["moveY"]
                duration = arguments["duration"]
                easing = arguments.get("easing", 0)
                
                command = f"MOVE_TRIGGER:{x},{y},{target},{moveX},{moveY},{duration},{easing}"
                success = send_command(command)
                
                if success:
                    return [TextContent(
                        type="text",
                        text=f"Created Move Trigger at ({x}, {y}) for group {target}: move ({moveX}, {moveY}) over {duration}s"
                    )]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_alpha_trigger":
                command = f"ALPHA_TRIGGER:{arguments['x']},{arguments['y']},{arguments['targetGroup']},{arguments['opacity']},{arguments['duration']},{arguments.get('easing', 0)}"
                success = send_command(command)
                
                if success:
                    return [TextContent(type="text", text=f"Created Alpha Trigger for group {arguments['targetGroup']}")]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_rotate_trigger":
                command = f"ROTATE_TRIGGER:{arguments['x']},{arguments['y']},{arguments['targetGroup']},{arguments['degrees']},{arguments['duration']},{arguments.get('easing', 0)},{arguments.get('times', 0)},{arguments.get('lockRotation', 0)}"
                success = send_command(command)
                
                if success:
                    return [TextContent(type="text", text=f"Created Rotate Trigger for group {arguments['targetGroup']}")]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_scale_trigger":
                command = f"SCALE_TRIGGER:{arguments['x']},{arguments['y']},{arguments['targetGroup']},{arguments['scaleX']},{arguments['scaleY']},{arguments['duration']},{arguments.get('easing', 0)}"
                success = send_command(command)
                
                if success:
                    return [TextContent(type="text", text=f"Created Scale Trigger for group {arguments['targetGroup']}")]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_delete_all":
                success = send_command("DELETE_ALL:")
                if success:
                    return [TextContent(type="text", text="Deleted all objects from level")]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_delete_objects":
                command = f"DELETE_OBJECTS:{arguments['groupId']}"
                success = send_command(command)
                if success:
                    return [TextContent(type="text", text=f"Deleted objects in group {arguments['groupId']}")]
                else:
                    raise Exception("Failed to send command")
            
            elif name == "gd_move_objects":
                command = f"MOVE_OBJECTS:{arguments['groupId']},{arguments['offsetX']},{arguments['offsetY']}"
                success = send_command(command)
                if success:
                    return [TextContent(type="text", text=f"Moved objects in group {arguments['groupId']}")]
                else:
                    raise Exception("Failed to send command")
            
            else:
                raise ValueError(f"Unknown tool: {name}")
        
        except Exception as e:
            raise ValueError(f"Error: {str(e)}")

    options = server.create_initialization_options()
    async with stdio_server() as (read_stream, write_stream):
        await server.run(read_stream, write_stream, options)

if __name__ == "__main__":
    import asyncio
    asyncio.run(serve())
