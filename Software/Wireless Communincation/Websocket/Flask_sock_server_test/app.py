from flask import Flask
from flask_sock import Sock

app = Flask(__name__)
sock = Sock(app)


@sock.route('/')
def reverse(ws):
    ws.send("DYLAN")
    while True:
        text = ws.receive()
        ws.send(text[::-1])


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=80)
