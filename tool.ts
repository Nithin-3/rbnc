import { players } from "./state"

export const findSpan = (): [number, number] => {
	const players_arr = [...players.values()];
	const occupied = (x: number, y: number) =>
		players_arr.some(p => Math.hypot(p.x - x, p.y - y) < 100);

	// nearest = closest to origin
	for (let ring = 0; ring < 10; ring++) {
		const n = ring * 100;
		// Check positions on this ring: origin, +x, -x, +y, -y, and 4 diagonals
		const spots = ring === 0
			? [[0, 0]]
			: [[n, 0], [-n, 0], [0, n], [0, -n],
			[n, n], [-n, n], [n, -n], [-n, -n]];
		for (const [x, y] of spots) {
			if (!occupied(x, y)) return [x, y];
		}
	}
	return [0, 0]; // fallback
}

const DIR_UP = (1 << 0)	    // 0001
const DIR_DOWN = (1 << 1)   // 0010
const DIR_LEFT = (1 << 2)   // 0100
const DIR_RIGHT = (1 << 3)  // 1000
const SPEED = 50
const dt = 1 / 60;
export const findNextPosition = (dir: number, colorHex: string) => {
	let dirX: number = 0, dirY: number = 0;
	if (dir & DIR_UP) dirY -= 1;
	if (dir & DIR_DOWN) dirY += 1;
	if (dir & DIR_LEFT) dirX += 1;
	if (dir & DIR_RIGHT) dirX -= 1;

	const len = Math.sqrt(dirX * dirX + dirY * dirY);
	const player = players.get(colorHex) ?? { x: 0, y: 0 };
	if (len > 0) {
		player.x += dirX / len * SPEED * dt;
		player.y += dirY / len * SPEED * dt;
	}
	return [player.x,player.y];
}
