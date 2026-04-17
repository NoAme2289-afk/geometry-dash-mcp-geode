import net from "net";

export class IPCClient {
  private pipeName = "\\\\.\\pipe\\gd_mcp_pipe";
  private connected = false;

  async sendCommand(command: string): Promise<string> {
    return new Promise((resolve, reject) => {
      const client = net.connect(this.pipeName, () => {
        this.connected = true;
        client.write(command);
      });

      client.on("data", (data) => {
        resolve(data.toString());
        client.end();
      });

      client.on("error", (err) => {
        this.connected = false;
        reject(new Error(`IPC Error: ${err.message}. Make sure GD injector is loaded.`));
      });

      client.on("end", () => {
        this.connected = false;
      });

      // Timeout after 5 seconds
      setTimeout(() => {
        if (!this.connected) {
          client.destroy();
          reject(new Error("IPC timeout. Is Geometry Dash running with injector?"));
        }
      }, 5000);
    });
  }

  async loadLevel(levelData: string): Promise<boolean> {
    try {
      const response = await this.sendCommand(`LOAD_LEVEL:${levelData}`);
      return response === "OK";
    } catch (error) {
      throw error;
    }
  }

  isConnected(): boolean {
    return this.connected;
  }
}
