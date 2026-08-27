import socket, struct

s = socket.create_connection(("127.0.0.1", 9000))
msg = b"HELLO FROM CLIENT"

s.sendall(struct.pack("!I", len(msg)) + msg)

header = s.recv(4)
length = struct.unpack("!I", header)[0]
payload = s.recv(length)

print("Server echoed back:", payload)
assert payload == msg
print("Round-trip OK")
s.close()