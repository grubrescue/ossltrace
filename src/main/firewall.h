#pragma once

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048

void
init_firewall(char *denylist_file_path);

char *
firewall_get_all_strings();

void
firewall_add_str(const char *str);

int
firewall_remove_str(const char *str);
