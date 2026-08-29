# Fluxgate

A multi-client instrument telemetry server, built to explore systems design,
networking, concurrency, authentication, and memory safety — areas outside
the Python-based tooling in the rest of my portfolio.

## Status: Milestone 1 — single-client synchronous echo server

- Custom length-prefixed framing protocol over TCP (4-byte big-endian length
  header + payload)
- Synchronous accept/read/write using Boost.Asio
- Verified round-trip with a standalone test client

## Roadmap

- [x] Milestone 1: single-client echo server, framing protocol
- [ ] Milestone 2: async I/O, multiple concurrent clients (Boost.Asio event loop)
- [ ] Milestone 3: token-based authentication handshake
- [ ] Milestone 4: simulated instrument data generator thread + thread-safe
      handoff (queue for logged data, latest-value slot for live dashboard)
- [ ] Milestone 5: C# client / dashboard
- [ ] Milestone 6: sanitizers (ASan/UBSan), unit tests, CI/CD (GitHub Actions)

## Building

Requires CMake, a C++17 compiler, and Boost (system component).

​```bash
mkdir build && cd build
cmake ..
make
./fluxgate
​```

## Protocol

Every message is framed as:

​```
[4 bytes: length, big-endian uint32][N bytes: payload]
​```

The server currently echoes any received message back to the sender.