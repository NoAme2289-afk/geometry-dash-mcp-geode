import { GDLevel, GDObject } from "./types.js";

/**
 * Create a basic level structure
 */
export function createBasicLevel(
  name: string,
  description: string,
  difficulty: string
): GDLevel {
  return {
    name,
    description,
    difficulty,
    objects: [],
  };
}

/**
 * Encode a single object to GD format
 * Format: 1,objectId,2,x,3,y,additional_properties;
 */
function encodeObject(obj: GDObject): string {
  const parts: string[] = [
    `1,${obj.id}`,
    `2,${obj.x}`,
    `3,${obj.y}`,
  ];

  if (obj.rotation !== undefined) {
    parts.push(`6,${obj.rotation}`);
  }

  if (obj.scale !== undefined) {
    parts.push(`32,${obj.scale}`);
  }

  if (obj.flipped) {
    parts.push(`4,1`);
  }

  return parts.join(",");
}

/**
 * Encode level to Geometry Dash level string format
 * This is a simplified version - real GD levels use more complex encoding
 */
export function encodeLevelString(level: GDLevel): string {
  if (level.objects.length === 0) {
    return "H4sIAAAAAAAAA6tWKs7PLShKLS5OTUlVslIqSCxKzMnPS1eyUqrVUUoqyi9ILSrOzM9TslKqBQBxm9IQMAAAAA==";
  }

  // Encode all objects
  const objectStrings = level.objects.map(encodeObject);
  const objectData = objectStrings.join(";");

  // Create level metadata
  const metadata = [
    `kS38,${level.objects.length}`, // Object count
    `k2,${level.name}`,              // Level name
    `k3,${level.description}`,       // Description
    `k4,${objectData}`,              // Object data
  ].join(",");

  // In real GD, this would be gzip compressed and base64 encoded
  // For simplicity, we'll return a readable format
  return Buffer.from(metadata).toString("base64");
}

/**
 * Decode a level string (simplified version)
 */
export function decodeLevelString(levelString: string): Partial<GDLevel> {
  try {
    const decoded = Buffer.from(levelString, "base64").toString("utf-8");
    // Parse the decoded string (simplified)
    return {
      name: "Decoded Level",
      description: "Imported from level string",
      objects: [],
    };
  } catch (error) {
    throw new Error("Invalid level string format");
  }
}
