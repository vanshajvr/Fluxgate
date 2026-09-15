import socket, struct, threading

def client(name, count=3):
    s = socket.create_connection(("127.0.0.1", 9000))
    for i in range(count):
        msg = f"{name}-msg{i}".encode()
        s.sendall(struct.pack("!I", len(msg)) + msg)
        header = s.recv(4)
        length = struct.unpack("!I", header)[0]
        payload = b""
        while len(payload) < length:
            payload += s.recv(length - len(payload))
        assert payload == msg, f"MISMATCH for {name}: sent {msg}, got {payload}"
        print(f"{name} round-trip {i} OK: {payload}")
    s.close()

threads = [threading.Thread(target=client, args=(f"client{i}",)) for i in range(5)]
for t in threads: t.start()
for t in threads: t.join()
print("ALL 5 CONCURRENT CLIENTS OK")