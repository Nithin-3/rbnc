export function colorToBuf(color: number): Buffer {
	return Buffer.from([
		(color >> 24) & 0xff,
		(color >> 16) & 0xff,
		(color >> 8) & 0xff,
		color & 0xff
	]);
}
