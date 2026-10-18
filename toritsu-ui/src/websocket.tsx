import { createContext, useContext, useEffect, useRef, useState } from "react";
import ReconnectingWebScoket from 'reconnecting-websocket';

type Message = {
    opcode: number;
    operand: number;
};

type WebSocketContextType = {
    connected: boolean;
    send: (message: Message) => void;
    lastMessage: Message | null;
};

const WebSocketContext =
    createContext<WebSocketContextType | null>(null);

    export function WebsocketProvider({
        children,
    }: {
        children: React.ReactNode;
    }) {
        // const ws = useRef<WebSocket | null>(null);
        const ws = useRef<ReconnectingWebScoket | null>(null);

        const [connected, setConnected] = useState(false);
        const [lastMessage, setLastMessage] = useState<Message | null>(null);

        useEffect(() => {
            const options = {
                connectionTimeout: 1500,
            };

            // const socket = new WebSocket("ws://192.168.4.1/ws");
            const socket = new ReconnectingWebScoket("ws://192.168.4.1/ws", [], options);

            ws.current = socket;
            
            socket.binaryType = "arraybuffer";

            socket.onopen = () => {
                console.log("ws connected.");
                setConnected(true);
            };

            socket.onclose = () => {
                console.log("ws disconnected.");
                setConnected(false);
            };

            socket.onerror = (error) => {
                console.log("ws error: ", error);
            };

            socket.onmessage = (event: MessageEvent) => {
                if(event.data instanceof ArrayBuffer) {
                    const view = new Uint8Array(event.data);

                    setLastMessage({
                        opcode: view[0],
                        operand: (view[1] << 8 | view[2])
                    })
                }
            };

            return () => {
                socket.close();
            };
        }, []);

        const send = (message: Message) => {
            if(ws.current?.readyState !== WebSocket.OPEN) {
                return;
            }

            const buffer = new Uint8Array([message.opcode, message.operand << 8, message.operand & 0xff])
            ws.current.send(buffer);
        };

        return (
            <WebSocketContext.Provider value={{connected, send, lastMessage}}>
                {children}
            </WebSocketContext.Provider>
        );
    }

    export function useWebSocket() {
        const context = useContext(WebSocketContext);

        if(!context) {
            throw new Error(
                "useWebSocket must be used inside WebSocketProvider."
            );
        }

        return context;
    }
