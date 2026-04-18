import net from 'net';

const pipeName = '\\\\.\\pipe\\gd_mcp_pipe';

console.log('Testing IPC connection to GD injector...');
console.log('Pipe name:', pipeName);

const client = net.connect(pipeName, () => {
    console.log('✓ Connected to pipe!');
    
    const testData = 'LOAD_LEVEL:test_data_123';
    console.log('Sending:', testData);
    
    client.write(testData);
});

client.on('data', (data) => {
    console.log('✓ Received response:', data.toString());
    client.end();
    process.exit(0);
});

client.on('error', (err) => {
    console.error('✗ Connection error:', err.message);
    process.exit(1);
});

client.on('end', () => {
    console.log('Connection closed');
});

setTimeout(() => {
    console.error('✗ Timeout - no response from DLL');
    client.destroy();
    process.exit(1);
}, 5000);
