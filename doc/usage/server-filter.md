# Example: nginx https<->tcp reverse proxy with denylist

stream section of /etc/nginx/nginx.conf:
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

denylist.txt contents:
```
greet
```

## Command to start:

```sh
badum@tss:~/ossltrace# build/ossltrace --denylist build/deny.txt sudo nginx -g 'daemon off;'
```

## Regular request
```sh
badum@tss:~/ossltrace# curl -k -L --http1.1 https://localhost:443/
```

Client output:
```
{"msg":"this is root page"}
```

Server output:
```
*** SSL_read intercepted
intermediate buffer size is 73 (num was 16384) contents: 

GET / HTTP/1.1
Host: localhost
User-Agent: curl/7.81.0
Accept: */*



***

*** SSL_read intercepted
retval is -1, no buffer, sorry
***

*** SSL_write intercepted
intermediate buffer size is 125, contents: 

HTTP/1.1 200 OK
date: Wed, 06 Dec 2023 14:30:21 GMT
server: uvicorn
content-length: 27
content-type: application/json


return value is 125
***


*** SSL_write intercepted
intermediate buffer size is 27, contents: 

{"msg":"this is root page"}
return value is 27
***


*** SSL_read intercepted
intermediate buffer size is 0 (num was 16384) contents: 



***
```

## Forbidden request
TODO

