import WebSocket, { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 3000 });

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

const colors: number[] = [
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
wss.on('connection', (ws: WebSocket) => {
	ws.on('message', (msg: Buffer) => {
		if (msg.length === 0) return;
		console.log("received:", msg);
		const type = msg.readUInt8(0);

		switch (type) {
			case 0x01:
			case 0x02:
				ws.send(msg);
				break;
			case 0x03:
				const pick = colors[Math.floor(Math.random() * colors.length)];
				const colorBuf = Buffer.from([
					(pick >> 24) & 0xff,
					(pick >> 16) & 0xff,
					(pick >> 8) & 0xff,
					pick & 0xff
				]);
				const [x, y] = nextSpiralPos();
				const xyBuf = Buffer.alloc(8);
				xyBuf.writeFloatLE(x, 0);
				xyBuf.writeFloatLE(y, 4);
				ws.send(Buffer.concat([
					Buffer.from([0x03]),
					colorBuf,
					xyBuf,
					msg.subarray(1)
				]));
			default:
				console.log("unknown packet");
		}
	});
});
