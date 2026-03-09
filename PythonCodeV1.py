import socket
import json
import time

HOST = "0.0.0.0"
PORT = 5000

LDR_BRIGHT = 2500

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(1)

print("Waiting")

conn, addr = server.accept()
print("Connected", addr)

relay_state = "RELAY_OFF"

no_motion_count = 0
dark_timer_start = None

while True:
    try:
        data = conn.recv(1024).decode().strip()

        if not data:
            continue

        sensor = json.loads(data)

        temp = sensor["temp"]
        hum = sensor["hum"]
        motion = sensor["pir"]
        light = sensor["ldr"]
        power = sensor["power"]

        print("\n--- SENSOR DATA ---")
        print("TEMP:", temp)
        print("HUMIDITY:", hum)
        print("MOTION:", motion)
        print("LIGHT:", light)
        print("POWER:", power)

        bright = light < LDR_BRIGHT
        dark = not bright

        # Motion detected
        if motion == 1:
            relay_state = "RELAY_ON"
            no_motion_count = 0
            dark_timer_start = None

        else:
            no_motion_count += 1

        # Start timer if dark and no motion twice
        if no_motion_count >= 2 and dark and dark_timer_start is None:
            dark_timer_start = time.time()

        # Check 60 second timeout
        if dark_timer_start is not None:
            if time.time() - dark_timer_start >= 60:
                relay_state = "RELAY_OFF"
                dark_timer_start = None
                no_motion_count = 0

        conn.send((relay_state + "\n").encode())

    except:
        break
