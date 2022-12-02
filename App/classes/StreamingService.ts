import { FastifyInstance } from "fastify";
import socketioServer from 'fastify-socket.io';
import { IOService } from "./IOService";

export class StreamingService {

    constructor(private app: FastifyInstance, private ioService: IOService) {

    }

    public async init(): Promise<void> {
        await this.app.register(socketioServer);
    }

    public setEvents(): void {
        // handle each new socketio connection
        this.app.io.on('connection', (socket) => {

            console.log('Socket connected!', socket.id)

            // listen for joystick updates here; then dispatch through mqtt
            socket.on('input/joystick', (data) => {
                let buf: Buffer = Buffer.from([data.rad, data.mag]);
                console.dir(buf);
                this.ioService.publish('input/joystick', buf);
            });

            // listen for button updates
            socket.on('input/light', (data) => {
                let buf: Buffer = Buffer.from([data]);
                console.dir(buf);
                this.ioService.publish('input/light', buf);
            });
        });

        // handle updates from mqtt broker
        this.ioService.subscribe(['output/battery', 'output/light', 'output/uss1']);

        this.ioService.on('output/battery', (msg) => {
            this.broadcast('output/battery', (msg[0] << 8) | msg[1]);
        });
        this.ioService.on('output/light', (msg) => {
            this.broadcast('output/light', msg[0]);
        });
        this.ioService.on('output/uss1', (msg) => {
            this.broadcast('output/uss1', msg[0]);
        });
    }

    public broadcast(topic: string, data: any): void {
        this.app.io.emit(topic, data);
    }
}