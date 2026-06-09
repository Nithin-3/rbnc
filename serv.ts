import WebSocket, { WebSocketServer } from 'ws';
import { insertPlayer, players, resetGame } from './state';
import { broadcast } from './broadcast';
import { PLAYER_MAX } from './constants';
import { findNextPosition } from './tool';

const wss = new WebSocketServer({ port: 3000 });

wss.on('connection', (ws: WebSocket & { color: string }) => {
	if (players.size >= PLAYER_MAX) {
		ws.close(4001, "server full");
		return;
	}
	ws.on('message', (msg: Buffer) => {
		if (msg.length === 0) return;
		const type = msg.readUInt8(0);

		switch (type) {
			case 0x00:
			case 0x01: {
				const [x, y] = findNextPosition(msg.readUInt8(1), msg.readUInt32BE(2).toString(16));
				const xyBuf = Buffer.alloc(8);
				xyBuf.writeFloatLE(x, 0);
				xyBuf.writeFloatLE(y, 4);

				const player = players.get(msg.subarray(2, 6).toString('hex'));
				const frame = msg.readInt32LE(6);

				if (!player) {
					break;
				}

				if (player.currentFrame > frame) {
					break; // old packet
				}
				player.currentFrame = frame;

				broadcast(wss, Buffer.concat([
					Buffer.from([type & 0xff]), // type
					msg.subarray(2, 6), // color
					xyBuf, // float x , float y
					msg.subarray(6), // remaining payload (sequence)
				]));
				break;
			}
			case 0x02: {
				// TODO:
				// send game time
				ws.send(msg);
				break;
			}
			case 0x03: {
				broadcast(wss, insertPlayer(ws, msg.subarray(1).toString('utf-8')));
				break;
			}
			default:
				console.log("unknown packet");
		}
	});

	ws.on('close', () => {
		players.delete(ws.color)
		if (players.size < 1) {
			resetGame()
		}
	});
});
