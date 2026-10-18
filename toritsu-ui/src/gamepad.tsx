import { useEffect } from 'react';
import { DPad } from '@rbuljan/gamepad';
import { Button } from './button';
import { useWebSocket } from './websocket';

export function Controller() {
    const ws = useWebSocket();

    useEffect(() => {
        const dpad = new DPad({
            id: 'dpad',
            parent: '#controls',
            position: {
                left: '10%',
                top: '80%',
            },
            radius: 60,
            onInput() {
                console.log(this.value ? this.direction : 'idle');
            },
        });

        dpad.init();

        return () => {
            dpad.destroy();
        };
    }, []);

    return (
        <>
            <div id='controls'>
                <Button label='reset' onPress={() => {
                    console.log('reset pressed');
                    ws.send({
                        opcode: 0,
                        operand: 1,
                    })
                }}/>
                <Button label='stop' onPress={() => {
                    console.log('stop pressed');
                    ws.send({
                        opcode: 0,
                        operand: 1,
                    })
                 }}/>
            </div>
        </>
    );
}
