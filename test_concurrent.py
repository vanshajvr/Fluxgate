import socket, struct, threading

def send_framed(s, data: bytes):
    s.sendall(struct.pack("!I", len(data)) + data)

def recv_framed(s):
    header = s.recv(4)
    length = struct.unpack("!I", header)[0]
    payload = b""
    while len(payload) < length:
        payload += s.recv(length - len(payload))
    return payload

def client(name, count=3):
    s = socket.create_connection(("127.0.0.1", 9000))
    send_framed(s, b"vansh-dev-token")  # auth first
    for i in range(count):
        msg = f"{name}-msg{i}".encode()
        send_framed(s, msg)
        reply = recv_framed(s)
        assert reply == msg, f"MISMATCH for {name}: sent {msg}, got {reply}"
    s.close()
    print(f"{name} all messages OK")

threads = [threading.Thread(target=client, args=(f"client{i}",)) for i in range(5)]
for t in threads: t.start()
for t in threads: t.join()
print("ALL 5 AUTHENTICATED CONCURRENT CLIENTS OK")