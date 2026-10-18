import { Controller } from './gamepad';
import { Telemetry } from './telemetry';
import { WebsocketProvider } from './websocket';

import './App.css'

function App() {
  const isMobile = navigator.userAgent.match(/iPhone|Android.+Mobile/) ? true : false;

  return (
    <>
      <WebsocketProvider>
        <Telemetry/>
        {isMobile ? <Controller/> : <></> }
      </WebsocketProvider>
    </>
  )
}

export default App
