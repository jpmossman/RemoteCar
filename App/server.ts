import * as fs from 'fs';
import * as path from 'path';
import fastify, { FastifyInstance } from 'fastify';
import cors from '@fastify/cors';
import helmet from '@fastify/helmet';
import { WebApp } from './classes/WebApp';
import { StreamingService } from './classes/StreamingService';
import { IOService } from './classes/IOService';

export class WebServer {
    private app: FastifyInstance;
    private webapp: WebApp;
    private streamingService: StreamingService;
    private ioService: IOService;

    constructor(private port: number, private mqttBrokerIp: string) {
        this.app = fastify({
            ignoreTrailingSlash: true
        });

        this.ioService = new IOService(mqttBrokerIp);
        this.streamingService = new StreamingService(this.app, this.ioService);
        this.webapp = new WebApp(this.app, this.ioService, this.streamingService);

        this.initialize();
    }

    private async initialize() {
        await this.setupMiddleware();
        await this.webapp.init();
        await this.streamingService.init();

        this.app.ready(err => {
            if (err) throw err;

            this.streamingService.setEvents();
        });

        this.app.listen({
            port: this.port,
            host: '0.0.0.0'
        }, err => {
            if (err) {
                throw err;
            }
            console.log('\x1b[1m\x1b[34m-----------------------------------\x1b[0m');
            console.group();
            console.log(`Web server is running on \x1b[1m\x1b[35m*:${this.port}\x1b[0m`);
            console.group();
            console.log(`Node Version:  \x1b[1m\x1b[33m${process.version}\x1b[0m`);
            console.groupEnd();
            console.groupEnd();
            console.log('\x1b[1m\x1b[34m-----------------------------------\x1b[0m');
        });
    }

    private async setupMiddleware(): Promise<void> {
        await this.app.register(helmet, {
            referrerPolicy: {
                policy: 'same-origin'
            },
            contentSecurityPolicy: {
                directives: {
                    scriptSrc: [
                        "'self'",
                        'cdn.socket.io'
                    ]
                }
            }
        });
        await this.app.register(cors, {
            origin: '*'
        });
    }
}

function main() {
    let port = 8000;
    if (process.argv.includes('-p')) {
        let tempPort: number = Number(process.argv[process.argv.indexOf('-p') + 1]);
        if (!isNaN(tempPort)) {
            port = tempPort;
        }
    }

    let brokerIp = '10.107.209.214';
    if (process.argv.includes('-b')) {
        let tempIp = process.argv[process.argv.indexOf('-b') + 1];
        if (tempIp) {
            brokerIp = tempIp;
        }
    }
    console.log(brokerIp);

    let server = new WebServer(port, brokerIp);
}

main();