import asyncio
import websockets
from WebSocketServer import WebSocketServer

if __name__ == '__main__':
    wsServer = WebSocketServer()
    start_server = websockets.serve(wsServer.ws_handler, '0.0.0.0', 5678)
    try:
        loop = asyncio.get_event_loop()
        loop.run_until_complete(start_server)
        
        loop.run_forever()
    except KeyboardInterrupt:
        print("Exiting")