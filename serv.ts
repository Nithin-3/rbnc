import WebSocket, { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 3000 });
const PLAYER_MAX = 10;

interface PlayerInfo {
	ws: WebSocket;
	color: number;
	x: number;
	y: number;
	name: string;
}

const players: PlayerInfo[] = [];

let spiralIdx = 0;
const SPIRAL_STEP = 110;
function nextSpiralPos(): [number, number] {
	const idx = spiralIdx++;
	if (idx === 0) return [0, 0];
	let x = 0, y = 0;
	let dir = 0;
	let stepCount = 1, stepProgress = 0, dirChanges = 0;
	for (let i = 0; i < idx; i++) {
		if (dir === 0) x += SPIRAL_STEP;
		else if (dir === 1) y -= SPIRAL_STEP;
		else if (dir === 2) x -= SPIRAL_STEP;
		else y += SPIRAL_STEP;
		stepProgress++;
		if (stepProgress >= stepCount) {
			stepProgress = 0;
			dir = (dir + 1) % 4;
			dirChanges++;
			if (dirChanges % 2 === 0) stepCount++;
		}
	}
	return [x, y];
}

function broadcast(data: Buffer, exclude?: WebSocket) {
	wss.clients.forEach(client => {
		if (client.readyState === WebSocket.OPEN && client !== exclude) {
			client.send(data);
		}
	});
}

function colorToBuf(color: number): Buffer {
	return Buffer.from([
		(color >> 24) & 0xff,
		(color >> 16) & 0xff,
		(color >> 8) & 0xff,
		color & 0xff
	]);
}

const COLORS: number[] = [
	0xbb7135A8,
	0x9492beA8,
	0xe439c0A8,
	0xd8db37A8,
	0x81b976A8,
	0xa080d2A8,
	0x61cd3dA8,
	0x5bd58aA8,
	0x7f6fd0A8,
	0x9f6c8aA8,

	0xff6b6bA8,
	0x4ecdc4A8,
	0x45b7d1A8,
	0xf7b731A8,
	0x815acfA8,
	0x10ac84A8,
	0xee5253A8,
	0x2e86deA8,
	0xff9ff3A8,
	0x54a0ffA8,

	0x00d2d3A8,
	0xff9f43A8,
	0xc8d6e5A8,
	0x727578A8,
	0x1dd1a1A8,
	0xf368e0A8,
	0x48dbfbA8,
	0x8395a7A8,
	0x66788dA8,
	0xfeca57A8,

	0xff4757A8,
	0x5d66fcA8,
	0x2ed573A8,
	0xeccc68A8,
	0x70a1ffA8,
	0x6666edA8,
	0xff6348A8,
	0x7bed9fA8,
	0xa29bfeA8,
	0xfd79a8A8
];

let colors = [...COLORS];

wss.on('connection', (ws: WebSocket) => {
	if (players.length >= PLAYER_MAX) {
		ws.close(4001, "server full");
		return;
	}
	ws.on('message', (msg: Buffer) => {
		if (msg.length === 0) return;
		const type = msg.readUInt8(0);

		switch (type) {
			case 0x00:
			case 0x01: {
				broadcast(msg);
				const p = players.find(p => p.ws === ws);
				if (p && msg.length >= 13) {
					p.x = msg.readFloatLE(1);
					p.y = msg.readFloatLE(5);
				}
				break;
			}
			case 0x03: {
				const pick = colors.splice(Math.floor(Math.random() * colors.length), 1)[0];
				const colorBuf = colorToBuf(pick);
				const [x, y] = nextSpiralPos();
				const xyBuf = Buffer.alloc(8);
				xyBuf.writeFloatLE(x, 0);
				xyBuf.writeFloatLE(y, 4);
				const resp = Buffer.concat([
					Buffer.from([0x03]),
					colorBuf,
					xyBuf,
					msg.subarray(1)
				]);
				broadcast(resp);

				const zeroTime = Buffer.alloc(8);
				zeroTime.writeBigUInt64LE(0n);
				for (const existing of players) {
					const eXyBuf = Buffer.alloc(8);
					eXyBuf.writeFloatLE(existing.x, 0);
					eXyBuf.writeFloatLE(existing.y, 4);
					ws.send(Buffer.concat([
						Buffer.from([0x03]),
						colorToBuf(existing.color),
						eXyBuf,
						zeroTime,
						Buffer.from(existing.name)
					]));
				}

				const timeLen = 8;
				const name = msg.subarray(1 + timeLen).toString('utf-8');
				players.push({ ws, color: pick, x, y, name });
				break;
			}
			default:
				console.log("unknown packet");
		}
	});

	ws.on('close', () => {
		const idx = players.findIndex(p => p.ws === ws);
		if (idx !== -1) {
			const leftColor = players[idx].color;
			players.splice(idx, 1);
			if (players.length === 0)
				colors = [...COLORS];
			const leaveMsg = Buffer.alloc(5);
			leaveMsg.writeUInt8(0x04, 0);
			leaveMsg.writeUInt32LE(leftColor, 1);
			broadcast(leaveMsg);
		}
	});
});
