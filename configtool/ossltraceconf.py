#!/usr/bin/env python3

import argparse
import socket
import struct
# import time

SOCKET_PATH = "/tmp/parasite.sock" # todo

class EpollClient:
    def __init__(self, socket_path):
        self.socket_path = socket_path
        self.client_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.client_socket.connect(self.socket_path)
        # self.client_socket.settimeout(200)

    def send_command(self, command, data=""):
        command = struct.pack('!H', command)  # Unsigned short in network byte order
        message = command + data.encode()
        self.client_socket.sendall(message)

    def add_string(self, string):
        self.send_command(0, string)
        # time.sleep(0.3)

    def remove_string(self, string):
        self.send_command(1, string)
        # time.sleep(0.3)

    def get_strings(self):
        self.send_command(2)
        # time.sleep(0.3)
        response = self.client_socket.recv(4096).decode()
        return response

    def close(self):
        self.client_socket.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='EpollClient Command Line Interface')
    parser.add_argument('socket_path', type=str, help='The path to the socket')
    subparsers = parser.add_subparsers(dest='command', help='Commands')

    add_parser = subparsers.add_parser('add_string', help='Add a string to the list')
    add_parser.add_argument('string', type=str, help='The string to add')

    get_parser = subparsers.add_parser('get_strings', help='Get the list of strings')

    remove_parser = subparsers.add_parser('remove_string', help='Remove a string from the list')
    remove_parser.add_argument('string', type=str, help='The string to remove')

    args = parser.parse_args()

    client = EpollClient(args.socket_path)
    # client = EpollClient(SOCKET_PATH)

    if args.command == 'add_string':
        client.add_string(args.string)
        print(f'Added string: {args.string}')

    elif args.command == 'get_strings':
        strings = client.get_strings()
        print('Current list of strings:')
        print(strings)

    elif args.command == 'remove_string':
        client.remove_string(args.string)
        print(f'Removed string: {args.string}')

    client.close()

