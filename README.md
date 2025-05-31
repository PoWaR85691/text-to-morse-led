# Text2MorseLed

- *TODO: text2morseled utility*

## Build and run

Build the project and install necessary files.
```bash
mkdir build && cd build
cmake .. && make && sudo make install
```

Run Led-Daemon with a name of led (see /sys/class/led).
```bash
sudo led-daemon/led-daemon input3::capslock
```

Now you can translate text to morse.
```bash
text2morseled/text2morseled "Hello, world!"
```