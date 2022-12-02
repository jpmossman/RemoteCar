import * as path from 'path';
import { FastifyInstance } from "fastify";
import { IOService } from "./IOService";
import fastify_static from '@fastify/static';
import { StreamingService } from './StreamingService';

export class WebApp {
    private carData: any;

    constructor(private app: FastifyInstance, private ioService: IOService, private streamService: StreamingService) {
        this.carData = {
            uss1: 0,
            bat: 0,
            light: false
        }
    }

    public async init() {
        this.app.register(fastify_static, {
            root: path.resolve(process.cwd(), 'public'),
            prefix: '/',
        });

        this.ioService.subscribe([
            'out/uss1',
            'out/bat',
            'out/light'
        ]);

        this.ioService.on('out/uss1', (msg: Buffer) => {

        });

        this.ioService.on('out/bat', (msg: Buffer) => {
            if (msg.length > 0) {
                this.carData.bat = (msg[0] >= 100) ? 100 : msg[0];
                this.streamService.broadcast('data/bat', this.carData.bat)
            }
        });

        this.ioService.on('out/light', (msg: Buffer) => {
            if (msg.length > 0) {
                this.carData.light = (msg.at(0) == 1) ? true : false;
                this.streamService.broadcast('data/light', this.carData.light)
            }
        });
    }
}