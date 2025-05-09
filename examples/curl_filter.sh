#!/bin/bash

../build/ossltrace -d forbid_get.txt -- curl -L --http1.1 -o/dev/null https://example.com
