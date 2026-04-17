export interface GDObject {
  id: number;        // Object ID
  x: number;         // X position
  y: number;         // Y position
  rotation?: number; // Rotation in degrees
  scale?: number;    // Scale factor
  flipped?: boolean; // Horizontal flip
}

export interface GDLevel {
  name: string;
  description: string;
  difficulty: string;
  objects: GDObject[];
}

export enum ObjectType {
  Block = 1,
  Spike = 8,
  Platform = 40,
  Orb = 36,
  Portal = 12,
  Decoration = 211,
}
