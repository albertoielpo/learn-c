# Makefile for building release binaries

CC = gcc
CFLAGS = -static -Wextra -Wall -Wpedantic -O2 -g -std=c99
RELEASE_DIR = release

# Targets
TARGETS = $(RELEASE_DIR)/char2dec \
          $(RELEASE_DIR)/char2hex \
          $(RELEASE_DIR)/decdump \
          $(RELEASE_DIR)/deldup \
          $(RELEASE_DIR)/git-broom \
          $(RELEASE_DIR)/cidr \
          $(RELEASE_DIR)/perf-metrics-mt

.PHONY: all clean

all: $(RELEASE_DIR) $(TARGETS)

$(RELEASE_DIR):
	mkdir -p $(RELEASE_DIR)

# char2dec - character to decimal converter
$(RELEASE_DIR)/char2dec: char2/char2dec.c utils/semver.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ char2/char2dec.c utils/semver.c

# char2hex - character to hexadecimal converter
$(RELEASE_DIR)/char2hex: char2/char2hex.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# decdump - decimal dump (like hexdump but decimal)
$(RELEASE_DIR)/decdump: decdump/decdump.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# deldup - delete duplicate files by SHA1 hash
$(RELEASE_DIR)/deldup: deldup/deldup.c utils/sha1.c utils/hmap.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ deldup/deldup.c utils/sha1.c utils/hmap.c

# git-broom - clean up dev dependencies in git repos
$(RELEASE_DIR)/git-broom: git-broom/git-broom.c utils/alist.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ git-broom/git-broom.c utils/alist.c

# perf-metrics-mt - CPU benchmark tool
$(RELEASE_DIR)/perf-metrics-mt: perf-metrics/perf-metrics-mt.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ $< -lm -lpthread

# cidr calculator
$(RELEASE_DIR)/cidr: cidr/cidr.c utils/semver.c | $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ cidr/cidr.c utils/semver.c

clean:
	rm -rf $(RELEASE_DIR)
