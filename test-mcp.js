const { spawn } = require('child_process');
const net = require('net');

// Test MCP server
const mcpServer = spawn('node', ['C:\\Users\\danys\\Desktop\\geometry-dash-mcp\\build\\index.js']);

let buffer = '';

mcpServer.stdout.on('data', (data) => {
    buffer += data.toString();
    console.log('MCP Server:', data.toString());
});

mcpServer.stderr.on('data', (data) => {
    console.log('MCP Server (stderr):', data.toString());
});

// Wait for server to start
setTimeout(() => {
    console.log('\n=== Testing MCP Commands ===\n');
    
    // Send create_level command
    const createLevelRequest = {
        jsonrpc: '2.0',
        id: 1,
        method: 'tools/call',
        params: {
            name: 'create_level',
            arguments: {
                name: 'Test Level',
                difficulty: 'normal'
            }
        }
    };
    
    mcpServer.stdin.write(JSON.stringify(createLevelRequest) + '\n');
    
    setTimeout(() => {
        // Send add_object command
        const addObjectRequest = {
            jsonrpc: '2.0',
            id: 2,
            method: 'tools/call',
            params: {
                name: 'add_object',
                arguments: {
                    levelName: 'Test Level',
                    objectType: 'block',
                    x: 0,
                    y: 0
                }
            }
        };
        
        mcpServer.stdin.write(JSON.stringify(addObjectRequest) + '\n');
        
        setTimeout(() => {
            // Send hot_reload command
            const hotReloadRequest = {
                jsonrpc: '2.0',
                id: 3,
                method: 'tools/call',
                params: {
                    name: 'hot_reload',
                    arguments: {
                        levelName: 'Test Level'
                    }
                }
            };
            
            mcpServer.stdin.write(JSON.stringify(hotReloadRequest) + '\n');
            
            setTimeout(() => {
                console.log('\n=== Test Complete ===\n');
                mcpServer.kill();
                process.exit(0);
            }, 2000);
        }, 1000);
    }, 1000);
}, 1000);
