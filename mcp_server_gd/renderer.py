"""
Geometry Dash Level Renderer
Renders level objects to PNG image with grid
"""
import json
from PIL import Image, ImageDraw
import os

# Object ID to color mapping (basic colors for different object types)
OBJECT_COLORS = {
    1: (100, 100, 100),      # Block - gray
    2: (255, 50, 50),        # Spike - red
    7: (255, 50, 50),        # Spike - red
    8: (255, 50, 50),        # Spike - red
    39: (255, 50, 50),       # Spike - red
    40: (150, 150, 150),     # Platform - light gray
    12: (100, 200, 255),     # Cube portal - blue
    13: (255, 150, 100),     # Ship portal - orange
    47: (255, 100, 200),     # Ball portal - pink
    111: (150, 255, 100),    # UFO portal - green
    660: (200, 100, 255),    # Wave portal - purple
    745: (255, 200, 50),     # Robot portal - yellow
    1331: (150, 50, 200),    # Spider portal - dark purple
    36: (255, 255, 100),     # Yellow orb
    84: (255, 150, 200),     # Pink orb
    141: (255, 100, 100),    # Red orb
    1022: (100, 150, 255),   # Blue orb
    1333: (100, 255, 100),   # Green orb
    1704: (50, 50, 50),      # Black orb
}

DEFAULT_COLOR = (200, 200, 200)  # Default color for unknown objects

def render_level(json_data: str, output_path: str = None, grid_size: int = 30, scale: float = 0.5) -> str:
    """
    Render level from JSON data to PNG image
    
    Args:
        json_data: JSON string with objects data
        output_path: Output PNG file path (default: temp file)
        grid_size: Grid cell size in pixels
        scale: Scale factor for object positions (GD units to pixels)
    
    Returns:
        Path to rendered PNG file
    """
    try:
        data = json.loads(json_data)
        objects = data.get("objects", [])
        
        if not objects:
            raise ValueError("No objects in level")
        
        # Find level bounds
        min_x = min(obj["x"] for obj in objects)
        max_x = max(obj["x"] for obj in objects)
        min_y = min(obj["y"] for obj in objects)
        max_y = max(obj["y"] for obj in objects)
        
        # Add padding
        padding = 100
        min_x -= padding
        max_x += padding
        min_y -= padding
        max_y += padding
        
        # Calculate image size
        width = int((max_x - min_x) * scale)
        height = int((max_y - min_y) * scale)
        
        # Limit image size
        max_size = 4000
        if width > max_size or height > max_size:
            scale_factor = min(max_size / width, max_size / height)
            width = int(width * scale_factor)
            height = int(height * scale_factor)
            scale *= scale_factor
        
        # Create image
        img = Image.new('RGB', (width, height), color=(30, 30, 40))
        draw = ImageDraw.Draw(img)
        
        # Draw grid
        grid_color = (50, 50, 60)
        for x in range(0, width, grid_size):
            draw.line([(x, 0), (x, height)], fill=grid_color, width=1)
        for y in range(0, height, grid_size):
            draw.line([(0, y), (width, y)], fill=grid_color, width=1)
        
        # Draw ground line (Y=105 in GD coordinates)
        ground_y = height - int((105 - min_y) * scale)
        if 0 <= ground_y <= height:
            draw.line([(0, ground_y), (width, ground_y)], fill=(100, 255, 100), width=2)
        
        # Draw objects
        for obj in objects:
            obj_id = obj["id"]
            x = int((obj["x"] - min_x) * scale)
            y = height - int((obj["y"] - min_y) * scale)  # Flip Y axis
            
            # Get object color
            color = OBJECT_COLORS.get(obj_id, DEFAULT_COLOR)
            
            # Draw object as rectangle (size based on object type)
            size = 15  # Default size
            if obj_id in [1, 40]:  # Blocks
                size = 15
            elif obj_id in [2, 7, 8, 39]:  # Spikes
                size = 12
            elif obj_id in [12, 13, 47, 111, 660, 745, 1331]:  # Portals
                size = 20
            elif obj_id in [36, 84, 141, 1022, 1333, 1704]:  # Orbs
                size = 10
            
            # Draw object
            draw.rectangle(
                [x - size//2, y - size//2, x + size//2, y + size//2],
                fill=color,
                outline=(255, 255, 255),
                width=1
            )
        
        # Save image
        if output_path is None:
            output_path = os.path.join(os.path.expanduser("~"), "Desktop", "gd_level_render.png")
        
        img.save(output_path)
        return output_path
        
    except Exception as e:
        raise Exception(f"Failed to render level: {str(e)}")
