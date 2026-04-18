#!/usr/bin/env node

import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
  Tool,
} from "@modelcontextprotocol/sdk/types.js";
import { GDLevel, GDObject, ObjectType } from "./types.js";
import { encodeLevelString, createBasicLevel, encodeForInjection } from "./level-encoder.js";
import { IPCClient } from "./ipc-client.js";

const server = new Server(
  {
    name: "geometry-dash-mcp-server",
    version: "0.1.0",
  },
  {
    capabilities: {
      tools: {},
    },
  }
);

// Store created levels in memory
const levels: Map<string, GDLevel> = new Map();

// IPC client for hot-reload
const ipcClient = new IPCClient();

// Define available tools
const tools: Tool[] = [
  {
    name: "create_level",
    description: "Create a new Geometry Dash level with basic properties",
    inputSchema: {
      type: "object",
      properties: {
        name: {
          type: "string",
          description: "Level name",
        },
        description: {
          type: "string",
          description: "Level description",
        },
        difficulty: {
          type: "string",
          enum: ["auto", "easy", "normal", "hard", "harder", "insane", "demon"],
          description: "Level difficulty",
        },
      },
      required: ["name"],
    },
  },
  {
    name: "add_object",
    description: "Add an object to a Geometry Dash level",
    inputSchema: {
      type: "object",
      properties: {
        levelName: {
          type: "string",
          description: "Name of the level to add object to",
        },
        objectType: {
          type: "string",
          enum: ["block", "spike", "platform", "orb", "portal", "decoration"],
          description: "Type of object to add",
        },
        x: {
          type: "number",
          description: "X position (in grid units, 30 pixels per unit)",
        },
        y: {
          type: "number",
          description: "Y position (in grid units, 30 pixels per unit)",
        },
        objectId: {
          type: "number",
          description: "Specific object ID (optional, defaults based on type)",
        },
      },
      required: ["levelName", "objectType", "x", "y"],
    },
  },
  {
    name: "export_level",
    description: "Export a level as Geometry Dash level string format",
    inputSchema: {
      type: "object",
      properties: {
        levelName: {
          type: "string",
          description: "Name of the level to export",
        },
      },
      required: ["levelName"],
    },
  },
  {
    name: "list_levels",
    description: "List all created levels",
    inputSchema: {
      type: "object",
      properties: {},
    },
  },
  {
    name: "hot_reload",
    description: "Hot reload current level into Geometry Dash editor (requires injector DLL loaded)",
    inputSchema: {
      type: "object",
      properties: {
        levelName: {
          type: "string",
          description: "Name of the level to hot reload",
        },
      },
      required: ["levelName"],
    },
  },
];

// Handle tool listing
server.setRequestHandler(ListToolsRequestSchema, async () => ({
  tools,
}));

// Handle tool execution
server.setRequestHandler(CallToolRequestSchema, async (request) => {
  const { name, arguments: args } = request.params;

  try {
    if (name === "create_level") {
      const { name: levelName, description = "", difficulty = "normal" } = args as any;
      
      const level = createBasicLevel(levelName, description, difficulty);
      levels.set(levelName, level);

      return {
        content: [
          {
            type: "text",
            text: `Level "${levelName}" created successfully!\nDifficulty: ${difficulty}\nDescription: ${description || "None"}`,
          },
        ],
      };
    }

    if (name === "add_object") {
      const { levelName, objectType, x, y, objectId } = args as any;
      
      const level = levels.get(levelName);
      if (!level) {
        throw new Error(`Level "${levelName}" not found. Create it first with create_level.`);
      }

      // Map object types to default IDs
      const defaultIds: Record<string, number> = {
        block: 1,
        spike: 8,
        platform: 40,
        orb: 36,
        portal: 12,
        decoration: 211,
      };

      const finalObjectId = objectId || defaultIds[objectType] || 1;
      
      const obj: GDObject = {
        id: finalObjectId,
        x: x * 30, // Convert grid units to pixels
        y: y * 30,
      };

      level.objects.push(obj);

      return {
        content: [
          {
            type: "text",
            text: `Added ${objectType} (ID: ${finalObjectId}) at position (${x}, ${y}) to level "${levelName}".\nTotal objects: ${level.objects.length}`,
          },
        ],
      };
    }

    if (name === "export_level") {
      const { levelName } = args as any;
      
      const level = levels.get(levelName);
      if (!level) {
        throw new Error(`Level "${levelName}" not found.`);
      }

      const levelString = encodeLevelString(level);

      return {
        content: [
          {
            type: "text",
            text: `Level "${levelName}" exported!\n\nObjects: ${level.objects.length}\n\nLevel String (copy this):\n${levelString}\n\nTo use: Copy the level string and import it in Geometry Dash level editor.`,
          },
        ],
      };
    }

    if (name === "list_levels") {
      if (levels.size === 0) {
        return {
          content: [
            {
              type: "text",
              text: "No levels created yet. Use create_level to create one.",
            },
          ],
        };
      }

      const levelList = Array.from(levels.entries())
        .map(([name, level]) => `- ${name}: ${level.objects.length} objects`)
        .join("\n");

      return {
        content: [
          {
            type: "text",
            text: `Created levels (${levels.size}):\n${levelList}`,
          },
        ],
      };
    }

    if (name === "hot_reload") {
      const { levelName } = args as any;
      
      const level = levels.get(levelName);
      if (!level) {
        throw new Error(`Level "${levelName}" not found.`);
      }

      try {
        // Use simple format for DLL injection: id,x,y;id,x,y
        const injectionData = encodeForInjection(level);
        const success = await ipcClient.loadLevel(injectionData);
        
        if (success) {
          return {
            content: [
              {
                type: "text",
                text: `✓ Level "${levelName}" hot reloaded into Geometry Dash!\nObjects: ${level.objects.length}\n\nThe level should now appear in the editor.`,
              },
            ],
          };
        } else {
          throw new Error("Failed to load level into game");
        }
      } catch (error) {
        const errorMsg = error instanceof Error ? error.message : String(error);
        return {
          content: [
            {
              type: "text",
              text: `✗ Hot reload failed: ${errorMsg}\n\nMake sure:\n1. Geometry Dash is running\n2. You're in the level editor\n3. The injector DLL is loaded\n\nFalling back to manual export...`,
            },
          ],
          isError: true,
        };
      }
    }

    throw new Error(`Unknown tool: ${name}`);
  } catch (error) {
    const errorMessage = error instanceof Error ? error.message : String(error);
    return {
      content: [
        {
          type: "text",
          text: `Error: ${errorMessage}`,
        },
      ],
      isError: true,
    };
  }
});

// Start server
async function main() {
  const transport = new StdioServerTransport();
  await server.connect(transport);
  console.error("Geometry Dash MCP Server running on stdio");
}

main().catch((error) => {
  console.error("Fatal error:", error);
  process.exit(1);
});
