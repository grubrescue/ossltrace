#pragma once

#define MAX_EVENTS 8
#define BUFFER_SIZE 1024

void
run_parasite_server(char *socket_path, char *denylist_file_path);
