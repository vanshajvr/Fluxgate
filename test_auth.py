import socket, struct

def send_framed(s, data: bytes):
    s.sendall(struct.pack("!I", len(data)) + data)

def recv_framed(s):
    header = s.recv(4)
    if len(header) < 4:
        return None
    length = struct.unpack("!I", header)[0]
    payload = b""
    while len(payload) < length:
        chunk = s.recv(length - len(payload))
        if not chunk:
            return None
        payload += chunk
    return payload

# --- Test 1: valid token, then a real message ---
print("=== Test 1: VALID token ===")
s1 = socket.create_connection(("127.0.0.1", 9000))
send_framed(s1, b"vansh-dev-token")
send_framed(s1, b"real measurement data")
reply = recv_framed(s1)
print("Echoed back:", reply)
assert reply == b"real measurement data"
print("PASS: authenticated client got its echo\n")
s1.close()

# --- Test 2: invalid token, connection should be rejected ---
print("=== Test 2: INVALID token ===")
s2 = socket.create_connection(("127.0.0.1", 9000))
send_framed(s2, b"totally-fake-token")
try:
    send_framed(s2, b"this should never be echoed")
    reply = recv_framed(s2)
    got_rejected = (reply is None)
except (ConnectionResetError, BrokenPipeError):
    got_rejected = True
    reply = "<connection closed by server>"
print("Result:", reply)
assert got_rejected, "Server should have rejected/closed the connection!"
print("PASS: server rejected the bad token, connection closed, no data echoed\n")
s2.close()

print("ALL AUTH TESTS PASSED")