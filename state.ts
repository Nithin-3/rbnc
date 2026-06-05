import WebSocket from 'ws';
import { colorToBuf } from './color';
import { COLORS } from './constants';
import { PlayerInfo } from './types';
import { findSpan } from './tool';

export const colors: number[] = [...COLORS];

export const players = new Map<string, PlayerInfo>();

export const insertPlayer = (ws: WebSocket, name: string) => {
	const colorBuf = colorToBuf(colors.splice(Math.floor(Math.random() * colors.length), 1)[0]);
	const [x, y] = findSpan();
	const xyBuf = Buffer.alloc(8);
	xyBuf.writeFloatLE(x, 0);
	xyBuf.writeFloatLE(y, 4);

	players.set(colorBuf.toString("hex"), {
			ws, x, y, name
		})
	return Buffer.concat([
		Buffer.from([0x03]),
		colorBuf,
		xyBuf,
		Buffer.from(name,'utf-8')
	]);

}
