# ossltrace - SSL/TLS Traffic Interception Tool

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](LICENSE)

## Overview

ossltrace is a Linux application that intercepts SSL/TLS library calls through runtime instrumentation, enabling traffic analysis and manipulation without proxy configuration or root privileges. The tool supports three distinct interception techniques to handle various application scenarios.

## Key Features

- **Multiple Interception Methods**: LD_PRELOAD, rtld-audit API, and runtime patching
- **Precise Runtime Patching**: Uses Capstone engine for instruction boundary analysis
- **Dynamic Configuration**: Live updates via control interface
- **Traffic Inspection**: Logging and content filtering capabilities
- **Certificate Control**: Optional verification bypass

## Technical Approach

### Interception Techniques

1. **LD_PRELOAD**  
   Basic symbol interception through dynamic linker precedence. Fastest method but cannot intercept dynamically resolved symbols.

2. **rtld-audit API**  
   Linker-provided auditing interface that handles both static and dynamic symbol resolution. Incompatible with applications compiled using `-fno-plt` optimization.

3. **Runtime Patching**  
   - Uses Capstone to disassemble and analyze function prologues
   - Fundamentally also uses LD_PRELOAD
   - Handles all symbol resolution cases including `dlsym()`
   - Works with `-fno-plt` compiled binaries
   - Has some performance overhead on each intercepting function call on unconditional jumps

## Dynamic Configuration

The included `ossltraceconf.py` control script communicates via Unix domain socket to provide real-time filter rule updates. It can be found in `configtool` subdirectory.

## Usage

### Build
```bash
sudo make
```
This command installs ossltrace binary to build/ subdirectory and intercepting libs to /usr/lib.

Dependencies will be checked during build. Requires:
- Capstone development files (unnecessary, if pkgbuild doesn't find one in a system -- then only preload and audit will be available);
- OpenSSL headers;
- Python3 (for control script).

### Basic Command
```bash
make && sudo make install
ossltrace [options] -- <command>
```

### Options
```
  -m, --mode       Interception mode (preload|audit|capstone)
  -o, --output     Log destination (file/stderr/stdout)
  -q, --quiet      Suppress logging
  -d, --denylist   File containing filter patterns
  -i, --ignore-ca  Disable certificate verification
  -h, --help       Display help
```


### Use with nginx
1. Setup nginx as reverse-proxy using SSL.
```
stream {
    upstream stream_backend {
         server localhost:8080;
    }

    server {
        listen                443 ssl;
        proxy_pass            stream_backend;

        ssl_certificate       /etc/nginx/certs/server.crt;
        ssl_certificate_key   /etc/nginx/certs/server.key;
        ssl_protocols         SSLv3 TLSv1 TLSv1.1 TLSv1.2;
        ssl_ciphers           HIGH:!aNULL:!MD5;
        ssl_session_cache     shared:SSL:20m;
        ssl_session_timeout   4h;
        ssl_handshake_timeout 30s;
     }
}
```
2. Start it.
```
build/ossltrace --denylist build/deny.txt sudo nginx -g 'daemon off;'
```

### Use with curl (simplier, maybe for testing purposes)
```
build/ossltrace --ignore-ca curl -L --http1.1 -o/dev/null https://localhost:443/
```

## License

BSD 3-Clause License. See [LICENSE](LICENSE) for complete terms.
