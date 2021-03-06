
import json
import opc
import pprint
import serial
import sys

from datalamp import config

def add_parsers(subparsers, argv):
	ConsoleRunner.add_parser(subparsers, argv)
	OPCRunner.add_parser(subparsers, argv)
	SerialRunner.add_parser(subparsers, argv)

class ConsoleRunner(object):
	
	@classmethod
	def add_parser(cls, subparsers, argv):
		parser = subparsers.add_parser("console")
		parser.set_defaults(func = ConsoleRunner)

	def start(self, program):
		pp = pprint.PrettyPrinter(indent = 4)
	
		program.init(config.config)
		for event in program.input_chain(config.config):
			program.on_tick([event])
			pp.pprint(event)


class OPCRunner(object):
	
	@classmethod
	def add_parser(cls, subparsers, argv):
		parser = subparsers.add_parser("opc")
		parser.add_argument("-o", "--opc-host", default="localhost", metavar="OPC HOST", help="OPC server hostname, when using OPC output")
		parser.add_argument("-p", "--opc-port", default="7890", metavar="OPC PORT", help="OPC server port, when using OPC output")
		parser.set_defaults(func = OPCRunner)

	def __init__(self):
		self.client = opc.Client(config.config.opc_host + ":" + config.config.opc_port)

	def start(self, program):
		program.init(config.config)
		for event in program.input_chain(config.config):
			program.on_tick([event])
			self.client.put_pixels(program.pixels, channel = 0)
 

class SerialRunner(object):
	
	EVENT_MARKER = 255
	
	@classmethod
	def add_parser(cls, subparsers, argv):
		parser = subparsers.add_parser("serial")
		parser.add_argument("-d", "--serial-device", default="/dev/ttyATH0", metavar="DEVICE", help="Serial device to write events to")
		parser.add_argument("-b", "--serial-baud", default="115200", metavar="BAUD", help="Serial baud rate")
		parser.set_defaults(func = SerialRunner)

	def __init__(self):
		self.serial_port = serial.Serial(config.config.serial_device, config.config.serial_baud)

	def start(self, program):
		program.init(config.config)
		for event in program.input_chain(config.config):
			address_low_7 = event["address"] & 127
			address_high_7 = (event["address"] & (127 << 7)) >> 7
			self.serial_port.write("".join(map(chr, [SerialRunner.EVENT_MARKER, address_low_7, address_high_7])))
			if "avg" in event:
				avg = event["avg"]
				if avg > 254:
					avg = 254
				self.serial_port.write(chr(avg))
			if config.config.debug:
				print ", ".join(map(str, [SerialRunner.EVENT_MARKER, address_low_7, address_high_7]))
	
