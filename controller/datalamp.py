#!/usr/bin/env python
# encoding: utf-8

import argparse
import sys

from datalamp import config
from datalamp.emojitracker import sum
from datalamp.chaser import chaser
from datalamp import runner

PROGRAMS = { "chaser": chaser, "emoji-sum": sum }

def parse_args(argv):
  argument_parser = argparse.ArgumentParser()
  
  subparsers = argument_parser.add_subparsers()
  runner.add_parsers(subparsers, argv)

  argument_parser.add_argument("-p", "--program", metavar="PROGRAM", default="emoji-sum", help="The visualisation program to run")

  argument_parser.add_argument("-f", "--feed-url", metavar="INPUT", default="http://stream.emojitracker.com/subscribe/raw", help="The url or file path to use as the emojitracker feed")
  argument_parser.add_argument("-k", "--rankings-url", metavar="INPUT", default="http://www.emojitracker.com/api/rankings", help="The url or file path to use for the initial emoji rankings")
  argument_parser.add_argument("-s", "--start-rank", default=0, metavar="RANK", help="Highest ranked emoji to track")
  argument_parser.add_argument("-m", "--max-emojis", default=64, metavar="MAX", help="Maximum number of emojis to track")
  argument_parser.add_argument("-t", "--throttle", default=0, metavar="HERTZ", help="Throttle input to a maximum number of events per second")

  args = argument_parser.parse_args(argv)

  if (not args.func or args.program not in PROGRAMS):
    argument_parser.print_help()
    return 1
  
  return args

def start(args):
  config.config = args
  runner = args.func()
  runner.start(PROGRAMS[args.program])


if __name__ == "__main__":
  args = parse_args(sys.argv[1:])
  sys.exit(start(args))
