const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 3000 });

wss.on('connection', (ws) => {
  ws.on('message', (msg) => {
    console.log("received:", msg.toString());
  });

  setInterval(() => {
    ws.send(JSON.stringify({ type: "player", x: 10, y: 20 }));
  }, 1000);
});
