#!/usr/bin/env python3

import argparse
import socket
import struct
import os
import re

class OssltraceClient:
    def __init__(self, socket_path):
        self.socket_path = socket_path
        self.client_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.client_socket.connect(self.socket_path)

    def send_command(self, command, data=""):
        command = struct.pack('!H', command)  # Unsigned short in network byte order
        message = command + data.encode()
        self.client_socket.sendall(message)

    def add_string(self, string):
        self.send_command(0, string)

    def remove_string(self, string):
        self.send_command(1, string)

    def get_strings(self):
        self.send_command(2)
        response = self.client_socket.recv(4096).decode()
        return response

    def close(self):
        self.client_socket.close()


def list_pids():
    pids = []
    for filename in os.listdir('/tmp'):
        match = re.match(r'parasite\.sock\.(\d+)', filename)
        if match:
            pids.append(int(match.group(1)))
    return pids


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ossltrace config CLI')
    subparsers = parser.add_subparsers(dest='command', help='Commands')

    # list pids command
    listpids_parser = subparsers.add_parser('listpids', help='List PIDs for socket files')

    # serve pid-dependent args
    client_parser = argparse.ArgumentParser(add_help=False)
    client_parser.add_argument('pid', type=int, help='The PID suffix for the socket')

    add_parser = subparsers.add_parser('addstring', parents=[client_parser], help='Add a string to the list')
    add_parser.add_argument('string', type=str, help='The string to add')

    get_parser = subparsers.add_parser('liststrings', parents=[client_parser], help='Get the list of strings')

    remove_parser = subparsers.add_parser('removestring', parents=[client_parser], help='Remove a string from the list')
    remove_parser.add_argument('string', type=str, help='The string to remove')

    args = parser.parse_args()

    if args.command == 'listpids':
        pids = list_pids()
        print('List of PIDs:')
        for pid in pids:
            print(pid)
    else:
        socket_path = f'/tmp/parasite.sock.{args.pid}'
        client = OssltraceClient(socket_path)

        if args.command == 'addstring':
            client.add_string(args.string)
            print(f'Added string: {args.string}')

        elif args.command == 'liststrings':
            strings = client.get_strings()
            print('Current list of strings:')
            print(strings)

        elif args.command == 'removestring':
            client.remove_string(args.string)
            print(f'Removed string: {args.string}')

        client.close()
