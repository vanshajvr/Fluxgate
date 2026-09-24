# Fluxgate

A multi-client instrument telemetry server, built to explore systems design,
networking, concurrency, authentication, and memory safety — areas outside
the Python-based tooling in the rest of my portfolio.

## Status: Milestone 2 — async I/O, multiple concurrent clients

- Custom length-prefixed framing protocol over TCP (4-byte big-endian length
  header + payload)
- Async server built on Boost.Asio's event loop (`io_context`): one thread
  handles many simultaneous clients via `async_accept`/`async_read`/`async_write`
  callback chains, instead of one thread per client
- Each connected client is a `Session` object with its own socket and buffers,
  managed by `shared_ptr`/`enable_shared_from_this` so it stays alive for the
  duration of its pending async operations and cleans itself up automatically
  on disconnect
- Verified with concurrent multi-client tests: 5 clients connected
  simultaneously, each sending multiple messages, all round-tripping correctly
  with interleaved delivery (proof of true concurrent handling, not
  sequential/blocking)
- Refactored from a single `main.cpp` into separate modules (protocol,
  session, server) for maintainability as auth and concurrency features grow

## Project structure
```
src/
├── main.cpp — entry point: creates io_context, creates Server, runs()
├── protocol.hpp — shared wire-format constants
├── session.hpp/cpp — Session: owns one client's socket, buffers, and
│ read/write message loop
└── server.hpp/cpp — Server: owns the acceptor, keeps accepting new
clients independent of already-connected ones
```


## Roadmap

- [x] Milestone 1: single-client echo server, framing protocol
- [x] Milestone 2: async I/O, multiple concurrent clients (Boost.Asio event loop)
- [ ] Milestone 3a: token-list authentication handshake
- [ ] Milestone 3b: upgrade to signed JWT verification
- [ ] Milestone 4: simulated instrument data generator thread + thread-safe
      handoff (queue for logged data, latest-value slot for live dashboard)
- [ ] Milestone 5: C# client / dashboard
- [ ] Milestone 6: sanitizers (ASan/UBSan), unit tests, CI/CD (GitHub Actions)

## Building

Requires CMake, a C++17 compiler, and Boost (system component).

```bash
mkdir build && cd build
cmake ..
make
./fluxgate
```

## Protocol

Every message is framed as:

The server currently accepts multiple concurrent clients and echoes any
received message back to the sender that sent it.