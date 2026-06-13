# HTTP Server in C

A lightweight HTTP server built from scratch in C using raw POSIX sockets. No frameworks, no dependencies — just C, the standard library, and pthreads.

Built as a learning project to understand how HTTP and TCP work at a low level.

## Features

- TCP socket server on port 8080
- Path-based request routing
- Serves static HTML files from a `public/` directory
- 404 handling for unknown routes
- Multi-threaded — each client connection gets its own thread

## Requirements

- Linux (tested on Ubuntu 24.04)
- GCC
- pthreads (included with `build-essential`)

```bash
sudo apt install build-essential
```

## Getting Started

**Clone the repo**

```bash
git clone https://github.com/yourusername/http-server-c.git
cd http-server-c
```

**Create your public directory and HTML files**

```bash
mkdir public
echo "<h1>Home</h1>" > public/index.html
echo "<h1>About</h1>" > public/about.html
```

**Compile**

```bash
gcc -o server server.c -lpthread
```

**Run**

```bash
./server
```

The server listens on `http://localhost:8080` by default.

## Usage

```bash
curl http://localhost:8080/         # serves public/index.html
curl http://localhost:8080/about    # serves public/about.html
curl http://localhost:8080/missing  # returns 404
```

Or open any of those URLs directly in a browser.

## Adding Routes

Routes are defined in `handle_client()` in `server.c`. Add a new `else if` block for each path:

```c
} else if (strcmp(path, "/contact") == 0) {
    serve_file(client_fd, "public/contact.html");
}
```

## Project Structure

```
http-server-c/
├── server.c          # main server source
├── public/           # static files served by the server
│   ├── index.html
│   └── about.html
├── .gitignore
├── LICENSE
└── README.md
```

## Roadmap

- [ ] Request logging to `access.log`
- [ ] POST request handling
- [ ] MIME type detection for CSS, JS, and images
- [ ] Config file for port and public directory path

## License

MIT — see [LICENSE](LICENSE) for details.
