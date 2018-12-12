#!/usr/bin/python
# Copyright (c) 2018 Dell Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
# LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
# FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
#
# See the Apache Version 2.0 License for specific language governing
# permissions and limitations under the License.

"""Script to read/write the SmartFusion IO based registers"""

import os
import struct
import sys
import argparse

class SmartFusion(object):
    """SmartFusion IO device

    The SMF microcontroller is a device that hangs off the LPC
    bus, and controls platform functions such as fan speed
    control, thermal management, LED states, etc.
    """

    #pylint: disable-msg=too-many-arguments
    def __init__(self, ram_addr_hi=0x210, ram_addr_lo=0x211,
                 read_addr=0x212, write_addr=0x213,
                 resource='/dev/port'):
        """Initialize the SmartFusion chip, this requires 5 parameters:

            - ram_addr_hi - Address to write the high 8-bits of the mailbox
                            address
            - ram_addr_lo - Address to write the low 8-bits of the mailbox
                            address
            - read_addr   - Address to read the mailbox contents
            - write_addr  - Address to write to update the mailbox
            - resource    - Optional device to handle the port access
                            Defaults to /dev/port
        """
        self.ram_addr_hi = ram_addr_hi
        self.ram_addr_lo = ram_addr_lo
        self.read_addr = read_addr
        self.write_addr = write_addr
        self.resource = resource
    #pylint: enable-msg=too-many-arguments


    def _set_mailbox_address(self, file_desc, mailbox_addr):
        """Set the mailbox address for reading/writing"""
        addr_lo = struct.pack('B', (mailbox_addr >> 0) & 0xFF)
        addr_hi = struct.pack('B', (mailbox_addr >> 8) & 0xFF)

        if os.lseek(file_desc, self.ram_addr_hi,
                    os.SEEK_SET) != self.ram_addr_hi:
            err = 'Seek failed to RAM Address High on %s' % self.resource
            raise IOError(err)

        ret = os.write(file_desc, addr_hi)
        if ret != 1:
            err = 'Write failed to RAM Address High %d' % ret
            raise IOError(err)

        if os.lseek(file_desc, self.ram_addr_lo,
                    os.SEEK_SET) != self.ram_addr_lo:
            err = 'Seek failed to RAM Address Low on %s' % self.resource
            raise IOError(err)

        ret = os.write(file_desc, addr_lo)
        if ret != 1:
            err = 'Write failed to RAM Address Low %d' % ret
            raise IOError(err)


    def read_byte(self, mailbox_addr):
        """Read one byte from :mailbox_addr"""
        file_desc = os.open(self.resource, os.O_RDWR)
        if file_desc < 0:
            raise IOError('File open failed %s' % self.resource)

        try:
            self._set_mailbox_address(file_desc, mailbox_addr)

            # Read the contents of the read_addr
            if os.lseek(file_desc, self.read_addr,
                        os.SEEK_SET) != self.read_addr:
                err = 'Seek failed to Read address on %s' % self.resource
                raise IOError(err)

            buf = os.read(file_desc, 1)
            return ord(buf)
        finally:
            os.close(file_desc)


    def write_byte(self, mailbox_addr, value):
        """Read one byte from :mailbox_addr"""
        file_desc = os.open(self.resource, os.O_RDWR)
        if file_desc < 0:
            raise IOError('File open failed %s' % self.resource)

        try:
            self._set_mailbox_address(file_desc, mailbox_addr)

            # Read the contents of the read_addr
            if os.lseek(file_desc, self.write_addr,
                        os.SEEK_SET) != self.write_addr:
                err = 'Seek failed to Write address on %s' % self.resource
                raise IOError(err)

            byte_val = struct.pack('B', value)
            ret = os.write(file_desc, byte_val)
            if ret != 1:
                err = 'Write failed to Write address %d' % ret
                raise IOError(err)
        finally:
            os.close(file_desc)


    def read(self, mailbox_addr, length):
        """Read :length bytes from :mailbox_addr"""
        return [self.read_byte(addr) for addr in
                    range(mailbox_addr, mailbox_addr + length)]


    def write(self, mailbox_addr, buf):
        """Write :buf to sequential locations starting with :mailbox_addr"""
        for byte_val, addr in zip(buf,
                    range(mailbox_addr, mailbox_addr + len(buf))):

            self.write_byte(addr, byte_val)


def auto_int(val):
    """Helper routine for argparse to parse all types of integers"""
    return int(val, 0)


def hexdump(buf):
    """Dump a list of bytes in hexdump -C format"""
    addr = 0
    while len(buf) > 0:
        print_buf = '%08x: ' % addr
        print_buf += ''.join(['%02x ' % byte for byte in buf[0:16]])

        # Pad print_buf until we reach the necessary number of characters
        print_buf += '   ' * 16
        # Strip out everything but the address field and first 16 characters
        # 10 bytes for address + colon + trailing space
        # 48 bytes for data (3 bytes for each byte)
        print_buf = print_buf[:58]

        print_buf += ''.join([chr(byte) if (byte >= 0x20 and byte < 0x7F)
                                else '.' for byte in buf[0:16]])

        print print_buf
        buf = buf[16:]
        addr += 16

def main():
    """Main routine"""
    parser = argparse.ArgumentParser(
                description='Communicate with SmartFusion chip')
    parser.add_argument('--ram_addr_hi', type=auto_int, default=0x210,
                        help='Address for the RAM_ADDR_HI register')
    parser.add_argument('--ram_addr_lo', type=auto_int, default=0x211,
                        help='Address for the RAM_ADDR_LO register')
    parser.add_argument('--read_reg', type=auto_int, default=0x212,
                        help='Address for the READ_DATA register')
    parser.add_argument('--write_reg', type=auto_int, default=0x213,
                        help='Address for the WRITE_DATA register')

    parser.add_argument('-r', '--read', nargs=1, type=auto_int,
                        help='Address to read data from')
    parser.add_argument('-w', '--write', nargs=1, type=auto_int,
                        help='Address to write data to')
    parser.add_argument('-c', '--count', nargs=1, type=auto_int, default=[1],
                        help='Number of bytes to read')
    parser.add_argument('-d', '--data', nargs='+', type=auto_int,
                        help='Data bytes to write')

    args = parser.parse_args()

    if args.read is None and args.write is None:
        errmsg = 'Must specify exactly one of --read or --write\n'
        sys.stderr.write(errmsg)
        sys.exit(1)

    if args.read is not None and args.write is not None:
        errmsg = 'Must specify either of --read or --write\n'
        sys.stderr.write(errmsg)
        sys.exit(1)

    if args.write is not None and args.data is None:
        errmsg = 'Must specify --data with --write\n'
        sys.stderr.write(errmsg)
        sys.exit(1)

    if args.data is not None and any([data > 0xFF for data in args.data]):
        errmsg = 'Must specify --data as individual bytes (<= 0xFF)\n'
        sys.stderr.write(errmsg)
        sys.exit(1)

    smf_dev = SmartFusion(ram_addr_hi=args.ram_addr_hi,
                          ram_addr_lo=args.ram_addr_lo,
                          read_addr=args.read_reg,
                          write_addr=args.write_reg)

    if args.read:
        read_buf = smf_dev.read(args.read[0], args.count[0])
        hexdump(read_buf)

    if args.write:
        smf_dev.write(args.write[0], args.data)


if __name__ == '__main__':
    main()
