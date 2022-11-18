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
        // console.log(this.app.io);
        this.app.io.on('connection', (socket) => {

            console.log('Socket connected!', socket.id)

            // listen for joystick updates here; then dispatch through mqtt
            socket.on('input/joystick', (data) => {
                let buf: Buffer = Buffer.from([data.rad, data.mag]);
                console.dir(buf);
                this.ioService.publish('input/joystick', buf);
            });
        });
    }

    public broadcast(topic: string, data: any): void {
        this.app.io.emit(topic, data);
    }
}