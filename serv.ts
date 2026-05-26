import WebSocket, { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 3000 });

wss.on('connection', (ws: WebSocket) => {
  ws.on('message', (msg: Buffer) => {
    console.log("received:", msg.toString());
  });

  setInterval(() => {
    ws.send(JSON.stringify({ type: "player", x: 10, y: 20 }));
  }, 1000);
});
