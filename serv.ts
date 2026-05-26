import WebSocket, { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 3000 });

wss.on('connection', (ws: WebSocket) => {
  ws.on('message', (msg: Buffer) => {
    console.log("received:", msg.toString());
    ws.send(msg.toString());
  });
});
