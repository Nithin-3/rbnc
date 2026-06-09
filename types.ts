import WebSocket from 'ws';

export type PlayerInfo = {
	ws: WebSocket;
	x: number;
	y: number;
	name: string;
	currentFrame:number;
};
