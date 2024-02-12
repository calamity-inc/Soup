# Soup Mesh Network Protocol

## Architecture

The network consists of "nodes", which are subdivded into "peers" and "admins".

Peers are public-facing and expected to listen on TCP/7106.

## Identitity

Every node is expected to have an RSA keypair. No exact bit-length will be prescribed, but the reference implementation goes for around 1536-bit.

The public key must use E = 65537 (0x10001) as only N will be shared.

## Transport

The WebSocket protocol is used to simplify administration, as this allows admins to use a web-based interface.

TLS is not used as the Web PKI would be a burden.

When a connection is established, the establisher encrypts a random 32-byte sequence with PKCS#1 padding against the receiving party's public key and transmits the encrypted blob. The recipient then verifies the PKCS#1 padding is intact and the data is indeed 32 bytes, closing the connection on failure. If all goes well, the 32 bytes will be split into two 16-byte keys for 128-bit AES-ECB with PCKS#7 padding to encrypt further traffic. The first key is used by the establisher to encrypt messages and by the receiving party to decrypt them, and vice-versa for the second key.

## Linking

For the initial peer—admin linking, the peer pre-shares its IP address, public key N, and a randomly-generated 64-bit "passnum". This data should be packed as follows:

```rust
ip_addr: u32,
passnum: u64,
public_n: Bigint,
```

And presented to the user as a base64-encoded string.

Then, the admin side can establish a connection as described in "Transport" and send the following on the encrypted channel:

```rust
msg_type: u8 = 0, // MESH_MSG_LINK
passnum: u64,
public_n: Bigint, // binary data
```

The peer then responds with a single-byte message: 1 (MESH_MSG_AFFIRMATIVE) or 2 (MESH_MSG_NEGATIVE). The peer closes the connection after transmitting the response in both cases.

## Authentication

To begin authentication, the single-byte message 3 (MESH_MSG_AUTH_REQUEST) is sent to the peer. The peer generates 14 random bytes and responds as follows:

```rust
msg_type: u8 = 4, // MESH_MSG_AUTH_CHALLENGE
challenge: u8[14],
```

The challenge is then signed with SHA256, and the node responds:

```rust
msg_type: u8 = 5, // MESH_MSG_AUTH_FINISH
public_n_hash: u32,
signature: Bigint,
```

The public_n_hash is the result of 32-bit FNV1A on the public N. It is present here because admins can't be identified by their IP address. However, for peers, the peer should verify that not only the public key is trusted, but also the IP address is associated with that public key. The peer responds with 1 (MESH_MSG_AFFIRMATIVE) or closes the connection.

## Application Messages

To obtain a list of supported application messages, you can send byte 6 (MESH_MSG_CAPABILITIES). The response starts with byte 6 (MESH_MSG_CAPABILITIES) which is followed by a comma-separated list of application message names.

To send an application message, use the following format:

```rust
msg_type: u8 = 7, // MESH_MSG_APPLICATION
seq_num: u32,
app_msg_name: String,
separator: char = ',',
data: String,
```

When sending, seq_num is set to the current "send sequence number", which starts at 0 and is incremented after each send.

When receiving, seq_num is checked against the current "receive sequence number", which starts at 0 and is incremented after each receive. If it mismatches, the connection must be closed.

### Replies

Replies to an application message use an abridged format as they need not be "routed":

```rust
msg_type: u8,
seq_num: u32,
data: String,
```

The `msg_type` for replies can be 1 (MESH_MSG_AFFIRMATIVE), 2 (MESH_MSG_NEGATIVE), or 7 (MESH_MSG_APPLICATION).
