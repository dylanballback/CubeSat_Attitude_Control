from http import client
from websockets import WebSocketServerProtocol
import asyncio
import time
import json

class WebSocketServer:
    clients = set()
    dashboard = None

    async def register(self, ws: WebSocketServerProtocol) -> None:
        self.clients.add(ws)
        print(str(ws.remote_address) + ' connected.')

    async def unregister(self, ws: WebSocketServerProtocol) -> None:
        self.clients.remove(ws)
        print(str(ws.remote_address) + ' disconnected.')

    async def sendMessage(self, message: str) -> None:
        try:
            if self.clients:
                await asyncio.wait([client.send(message) for client in self.clients])
        except:
            print("Unhandled exception when sending message to clients")

    async def sendMessageClient(self, message: str, client) -> None:
        try:
            await asyncio.wait([client.send(message)])
        except:
            print("Unhandled exception when sending message to client")

    async def ws_handler(self, ws: WebSocketServerProtocol, uri: str) -> None:
        await self.register(ws)
        try:
            await self.distribute(ws)
        finally:
            await self.unregister(ws)

    async def distribute(self, ws: WebSocketServerProtocol) -> None:
        async for message in ws:
            print("Received Message: " + message + " from " + str(ws.remote_address))
            await self.sendMessage(message)
