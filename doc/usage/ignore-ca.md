# Example: ignore-ca usage

Notes:
- TLS certificate is actually self-signed
```
sudo openssl req -new -newkey rsa:4096 -x509 -sha256 -days 365 -nodes -out server.crt -keyout server.key
sudo cp server.* /etc/nginx/certs/
```
- the host name in a certificate still has to match actual host name

## enabled (-i/--ignore-ca)

```
badum@tss:~/ossltrace$ build/ossltrace --ignore-ca curl -L --http1.1 -o/dev/null https://localhost:443/
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
*** SSL_CTX_set_verify intercepted
setting SSL_VERIFY_NONE
***

*** SSL_get_verify_result intercepted
retval is 18, though we'll return 0
***

*** SSL_write intercepted
intermediate buffer size is 73, contents: 

GET / HTTP/1.1
Host: localhost
User-Agent: curl/7.81.0
Accept: */*


return value is 73
***


*** SSL_read intercepted
intermediate buffer size is 125 (num was 102400) contents: 

HTTP/1.1 200 OK
date: Wed, 06 Dec 2023 14:11:42 GMT
server: uvicorn
content-length: 27
content-type: application/json



***

*** SSL_read intercepted
intermediate buffer size is 27 (num was 27) contents: 

{"msg":"this is root page"}

***
100    27  100    27    0     0    385      0 --:--:-- --:--:-- --:--:--   391

*** SSL_read intercepted
retval is -1, no buffer, sorry
***
```

## disabled

```
badum@tss:~/ossltrace$ build/ossltrace curl -L --http1.1 -o/dev/null https://localhost:443/
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
*** SSL_CTX_set_verify intercepted
calling with no changes
***

*** SSL_get_verify_result intercepted
retval is 18, thus returning 18
***
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0

*** SSL_read intercepted
retval is -1, no buffer, sorry
***
curl: (60) SSL certificate verify result: self-signed certificate (18)
More details here: https://curl.se/docs/sslcerts.html

curl failed to verify the legitimacy of the server and therefore could not
establish a secure connection to it. To learn more about this situation and
how to fix it, please visit the web page mentioned above.
```
