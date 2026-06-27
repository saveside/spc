# save's pocket chat
A lightweight, concurrent chat server built in C using raw TCP sockets.

## Featues
- **Command handling:** Type `/help` to explore commands
- **Rooms:** create and switch between named chat rooms
- **Room history:** new joined users see recent messages on entry (circular buffer)

## To-Do
- [ ] Implement a custom CLI client
- [ ] Implement user authentication
- [ ] Personalization for Users
- [ ] Add a support for private messages

## Installation
```bash
git clone https://github.com/saveside/spc
cd spc
mkdir build & cd build
cmake ..
cmake --build .
```

## Usage
To use spc, simply run the command below in your terminal:
```bash
./server start
```

To change the default port (8080), use the -p or --port flag:
```bash
./server start --port 9000
```

You can connect to server using standard networking tools like netcat (nc) or telnet.
```bash
nc localhost 8080
```

## License
spc is licensed under the MIT License. See [LICENSE](LICENSE) for more information.
