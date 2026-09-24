# Fluxgate

A multi-client instrument telemetry server, built to explore systems design,
networking, concurrency, authentication, and memory safety — areas outside
the Python-based tooling in the rest of my portfolio.

## Status: Milestone 3a — token-list authentication 

- The handshake design: first message from a new connection is treated as a token, not data then confirmed before the normal read/write loop begins
- The Authenticator interface — an abstract interface (verify(token)), with TokenListAuthenticator as the current implementation, specifically so 3b (JWT) can swap in without touching Session/Server
- Fail-closed behavior: invalid token → connection closed immediately, no data ever echoed.
- Verified: both paths tested (valid token → echo works; invalid token → connection rejected), plus confirmed auth doesn't break the milestone 2 concurrency guarantees (multiple authenticated clients still served concurrently)

## Project structure
```
src/
├── main.cpp — entry point: creates io_context, creates Server, runs()
├── protocol.hpp — shared wire-format constants
├── session.hpp/cpp — Session: owns one client's socket, buffers, and
│ read/write message loop
├── server.hpp/cpp — Server: owns the acceptor, keeps accepting new
│  clients independent of already-connected ones
├── authenticator.hpp: abstract Authenticator interface (verify(token)),
│   so the verification mechanism can be swapped without touching Session/Server
└── token_list_authenticator.hpp : current implementation: checks a token
     against a fixed in-memory set (milestone 3b will add a JWT-based one) 
```


## Roadmap

- [x] Milestone 1: single-client echo server, framing protocol
- [x] Milestone 2: async I/O, multiple concurrent clients (Boost.Asio event loop)
- [x] Milestone 3a: token-list authentication handshake
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

```
[4 bytes: length, big-endian uint32][N bytes: payload]
​```

On a new connection, the first message is reserved for the auth token and
is not echoed. Once verified, all subsequent messages go through the normal
echo loop. An invalid token causes the server to close the connection
immediately, with no data ever echoed back.