import * as mqtt from "mqtt";
import { EventEmitter } from 'node:events';
import { Buffer } from 'node:buffer';

export class IOService extends EventEmitter {
    private client: mqtt.MqttClient;
    private topics: { [topic: string]: boolean };
    private connected: boolean;

    constructor(brokerIp: string) {
        super();
        this.topics = {};
        this.connected = false;
        this.client = mqtt.connect(`mqtt://${brokerIp}`); //10.107.209.214

        this.client.on('connect', () => {
            // this.client.publish('inputs/light', 'Hello mqtt')

            Object.keys(this.topics).forEach(topic => {
                this.client.subscribe(topic);
            });
            this.connected = true;
        });

        this.client.on('message', (topic, message) => {
            // console.log(topic, message.toString())
            this.emit(topic, message);
            // this.client.end();
        });
    }

    public getTopics(): string[] {
        return Object.keys(this.topics);
    }

    public publish(topic: string, buffer: string | Buffer): void {
        this.client.publish(topic, buffer);
    }

    public subscribe(topic: string): void;
    public subscribe(topics: string[]): void;
    public subscribe(topics: string | string[]): void {
        if (Array.isArray(topics)) {
            topics.forEach(newTopic => {
                if (!this.topics.hasOwnProperty(newTopic)) {
                    this.topics[newTopic] = true;
                    if (this.connected) {
                        this.client.subscribe(newTopic);
                    }
                }
            });
        }
        else if (typeof topics == 'string') {
            if (!this.topics.hasOwnProperty(topics)) {
                this.topics[topics] = true;
                if (this.connected) {
                    this.client.subscribe(topics);
                }
            }
        }
    }

    public unsubscribe(topic: string): void;
    public unsubscribe(topics: string[]): void;
    public unsubscribe(topics: string | string[]): void {
        if (Array.isArray(topics)) {
            topics.forEach(newTopic => {
                if (this.topics.hasOwnProperty(newTopic)) {
                    delete this.topics[newTopic];
                    if (this.connected) {
                        this.client.unsubscribe(newTopic);
                    }
                }
            });
        }
        else if (typeof topics == 'string') {
            if (this.topics.hasOwnProperty(topics)) {
                delete this.topics[topics];
                if (this.connected) {
                    this.client.unsubscribe(topics);
                }
            }
        }
    }
}