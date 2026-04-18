"""
GigaChat Vision MCP Server
Provides image analysis capabilities through GigaChat API
"""

import asyncio
import json
import os
import uuid
import requests
from typing import Any
from mcp.server.models import InitializationOptions
import mcp.types as types
from mcp.server import NotificationOptions, Server
import mcp.server.stdio


# Disable SSL warnings
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


class GigaChatVisionMCP:
    def __init__(self, api_key: str):
        self.api_key = api_key
        self.base_url = "https://gigachat.devices.sberbank.ru/api/v1"
        self.access_token = None
        
    def get_access_token(self):
        """Get access token from GigaChat"""
        url = "https://ngw.devices.sberbank.ru:9443/api/v2/oauth"
        
        headers = {
            "Authorization": f"Basic {self.api_key}",
            "RqUID": str(uuid.uuid4()),
            "Content-Type": "application/x-www-form-urlencoded"
        }
        
        data = {"scope": "GIGACHAT_API_PERS"}
        
        response = requests.post(url, headers=headers, data=data, verify=False)
        response.raise_for_status()
        self.access_token = response.json()["access_token"]
        return self.access_token
    
    def upload_file(self, file_path: str) -> str:
        """Upload file to GigaChat storage"""
        if not self.access_token:
            self.get_access_token()
        
        url = f"{self.base_url}/files"
        
        headers = {
            "Authorization": f"Bearer {self.access_token}"
        }
        
        # Determine MIME type
        ext = os.path.splitext(file_path)[1].lower()
        mime_types = {
            '.jpg': 'image/jpeg',
            '.jpeg': 'image/jpeg',
            '.png': 'image/png',
            '.gif': 'image/gif',
            '.webp': 'image/webp'
        }
        mime_type = mime_types.get(ext, 'image/jpeg')
        
        with open(file_path, 'rb') as f:
            files = {
                'file': (os.path.basename(file_path), f, mime_type)
            }
            
            data = {
                'purpose': 'general'
            }
            
            response = requests.post(url, headers=headers, files=files, data=data, verify=False)
            response.raise_for_status()
            
            return response.json()["id"]
    
    def analyze_image(self, image_path: str, prompt: str = "Podrobno opishi chto izobrazheno na etoy kartinke") -> str:
        """Analyze image using GigaChat-Max"""
        if not self.access_token:
            self.get_access_token()
        
        # Upload file
        file_id = self.upload_file(image_path)
        
        # Send request
        url = f"{self.base_url}/chat/completions"
        
        headers = {
            "Authorization": f"Bearer {self.access_token}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "model": "GigaChat-Max",
            "messages": [
                {
                    "role": "user",
                    "content": prompt,
                    "attachments": [file_id]
                }
            ],
            "temperature": 0.7
        }
        
        try:
            response = requests.post(url, headers=headers, json=payload, verify=False)
            response.raise_for_status()
            
            result = response.json()
            return result["choices"][0]["message"]["content"]
            
        except requests.exceptions.HTTPError as e:
            if e.response.status_code == 401:
                # Token expired, retry
                self.get_access_token()
                return self.analyze_image(image_path, prompt)
            else:
                raise


# Initialize server
server = Server("gigachat-vision")

# Get API key from environment
GIGACHAT_API_KEY = os.getenv("GIGACHAT_API_KEY", "MDE5ZGExNjctYTZiOC03MjZmLThmN2EtMWUwZDQ5MzRlODE1OjBlZDA2MzcwLTQyYTItNDkyYy04NzZkLWZhZDcxOTllMTE4NQ==")
gigachat_client = GigaChatVisionMCP(GIGACHAT_API_KEY)


@server.list_tools()
async def handle_list_tools() -> list[types.Tool]:
    """List available tools"""
    return [
        types.Tool(
            name="analyze_image",
            description="Analyze an image using GigaChat-Max vision model. Supports JPG, PNG, GIF, WEBP formats.",
            inputSchema={
                "type": "object",
                "properties": {
                    "image_path": {
                        "type": "string",
                        "description": "Path to the image file to analyze"
                    },
                    "prompt": {
                        "type": "string",
                        "description": "Question or instruction about the image (optional, default: describe the image)",
                        "default": "Podrobno opishi chto izobrazheno na etoy kartinke"
                    }
                },
                "required": ["image_path"]
            }
        ),
        types.Tool(
            name="check_balance",
            description="Check GigaChat API token balance",
            inputSchema={
                "type": "object",
                "properties": {}
            }
        )
    ]


@server.call_tool()
async def handle_call_tool(
    name: str, arguments: dict | None
) -> list[types.TextContent | types.ImageContent | types.EmbeddedResource]:
    """Handle tool execution"""
    
    if name == "analyze_image":
        if not arguments or "image_path" not in arguments:
            raise ValueError("image_path is required")
        
        image_path = arguments["image_path"]
        prompt = arguments.get("prompt", "Podrobno opishi chto izobrazheno na etoy kartinke")
        
        if not os.path.exists(image_path):
            raise FileNotFoundError(f"Image file not found: {image_path}")
        
        try:
            result = gigachat_client.analyze_image(image_path, prompt)
            return [types.TextContent(type="text", text=result)]
        except Exception as e:
            return [types.TextContent(type="text", text=f"Error analyzing image: {str(e)}")]
    
    elif name == "check_balance":
        try:
            if not gigachat_client.access_token:
                gigachat_client.get_access_token()
            
            url = f"{gigachat_client.base_url}/balance"
            headers = {
                "Authorization": f"Bearer {gigachat_client.access_token}"
            }
            
            response = requests.get(url, headers=headers, verify=False)
            response.raise_for_status()
            
            balance = response.json()
            balance_text = json.dumps(balance, indent=2, ensure_ascii=False)
            
            return [types.TextContent(type="text", text=f"GigaChat Balance:\n{balance_text}")]
        except Exception as e:
            return [types.TextContent(type="text", text=f"Error checking balance: {str(e)}")]
    
    else:
        raise ValueError(f"Unknown tool: {name}")


async def main():
    """Run the server"""
    async with mcp.server.stdio.stdio_server() as (read_stream, write_stream):
        await server.run(
            read_stream,
            write_stream,
            InitializationOptions(
                server_name="gigachat-vision",
                server_version="1.0.0",
                capabilities=server.get_capabilities(
                    notification_options=NotificationOptions(),
                    experimental_capabilities={},
                ),
            ),
        )


if __name__ == "__main__":
    asyncio.run(main())
