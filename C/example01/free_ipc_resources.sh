#!/bin/bash

# Memory
ipcrm -M 0x0badcafe
ipcrm -M 0xdeadbeef
ipcrm -M 0x0d15ea5e

# Message queues
ipcrm -Q 0x8badf00d
ipcrm -Q 0x0badcafe

