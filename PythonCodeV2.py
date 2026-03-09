import socket
import json
from datetime import datetime

HOST = "0.0.0.0"
PORT = 5000

server = socket.socket()
server.bind((HOST,PORT))
server.listen(1)

print("Waiting ESP32...")

conn,addr = server.accept()
print("Connected:",addr)

buffer=""

while True:
    try:
        data = conn.recv(1024).decode()

        if not data:
            continue

        buffer += data

        while "\n" in buffer:
            line, buffer = buffer.split("\n",1)

            if not line.strip():
                continue

            sensor = json.loads(line)

            temp = sensor["temp"]
            hum = sensor["hum"]
            motion = sensor["pir"]
            light = sensor["ldr"]
            power = sensor["power"]

            # Control logic
            fan_state = "FAN_ON" if temp > 20 else "FAN_OFF"
            led_state = "LED_ON" if motion == 1 else "LED_OFF"

            print(f"[{datetime.now().strftime('%H:%M:%S')}] "
                  f"T:{temp:.1f} H:{hum:.1f} "
                  f"L:{light} M:{motion} P:{power:.2f}")

            print("Fan:",fan_state,"LED:",led_state)

            conn.send((fan_state+"\n").encode())
            conn.send((led_state+"\n").encode())

    except Exception as e:
        print(e)
        break
