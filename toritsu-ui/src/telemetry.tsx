import { useRef, useEffect } from "react";
import { ConnectionBar, MultiAxisPlot, EventLog, Attitude } from "@altara/core";
import { HorizontalSituationIndicator } from "@altara/aerospace";
import { useWebSocket } from "./websocket";

import '@altara/core/styles.css'

export function Telemetry() {
    const isMobile = navigator.userAgent.match(/iPhone|Android.+Mobile/) ? true : false;
    const intervalRef = useRef<number | null>(null);

    const ws = useWebSocket();

    useEffect(() => {
        if(intervalRef.current !== null) return;

        intervalRef.current = setInterval(() => {
            ws.send({
                opcode: 0,
                operand: 2,
            })
        }, 750);

        return () => {
            if(intervalRef.current !== null) {
                clearInterval(intervalRef.current);
                intervalRef.current = null;
            }
        }
    }, []);

    return (
        <>
            <div id="telemetry">
                <ConnectionBar latencyMs={0} messagesPerSecond={0} status={ws.connected ? "connected" : "disconnected"} url="ws://192.168.4.1/ws" />
                <div id="tel-common">
                    <MultiAxisPlot height={isMobile ? 180 : 500} mockMode channels={[
                        {
                            axis: 'left',
                            key: 'battery',
                            label: 'Battery',
                            unit: '%'
                        },
                        {
                            axis: 'right',
                            key: 'current',
                            label: 'Current',
                            unit: 'A'
                        }
                    ]}/>
                    <EventLog maxEntries={isMobile ? 3 : 10} entries={[
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                        {
                            message: 'EventLog is disabled.',
                            severity: 'info',
                            timestamp: 1790673590832,
                        },
                    ]}/>
                </div>
                <div id={isMobile ? "tel-mobile" : "tel-pc"}>
                    {isMobile ? <></> :
                        <div id="tel-pc-child">
                        </div>
                    }
                    <Attitude mockMode size={isMobile ? 120 : 300}/>
                    <HorizontalSituationIndicator mockMode size={isMobile ? 120 : 300}/>
                </div>

            </div>
        </>
    );
}
