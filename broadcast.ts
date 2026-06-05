import WebSocket, { WebSocketServer } from 'ws';

export function broadcast(wss: WebSocketServer, data: Buffer, exclude?: WebSocket) {
	setTimeout(() => wss.clients.forEach(client => {
		if (client.readyState === WebSocket.OPEN && client !== exclude) {
			client.send(data);
		}
	}), Math.floor(Math.random() * 100))
}
