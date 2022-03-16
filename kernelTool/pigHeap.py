# -*- coding:UTF-8 -*-

import gdb, json, datetime
from string import punctuation



def swab64(number):
	return int.from_bytes(number.to_bytes(int(len(hex(number)[2:])/2),byteorder='big'),byteorder='little')

def readMemory(addr, length=0x10):
	"""Return a `length` long byte array with the copy of the process memory at `addr`."""
	return gdb.selected_inferior().read_memory(addr, length).tobytes()


def u64(x: bytes, s: bool = False) -> int:
	"""Unpack one qword respecting the current architecture endianness."""
	return struct.unpack("{}Q".format(endian_str()), x)[0] if not s else struct.unpack("{}q".format(endian_str()), x)[0]


def p64(x: int, s: bool = False) -> bytes:
    """Pack one qword respecting the current architecture endianness."""
    return struct.pack("{}Q".format(endian_str()), x) if not s else struct.pack("{}q".format(endian_str()), x)




class pigSlub(gdb.Command):
	def __init__ (self):
		super(pigSlub, self).__init__ ("pigSlub", gdb.COMMAND_USER)
		self._demation = 0
		self._cpu_amount = 0
		self.__per_cpu_offset = None
		self._kmalloc_caches = None
		self._kmalloc_xxx_dict = {"kmalloc-96":1,"kmalloc-192":2,"kmalloc-8":3,"kmalloc-16":4,
						"kmalloc-32":5,"kmalloc-64":6,"kmalloc-128":7,"kmalloc-256":8,
						"kmalloc-512":9,"kmalloc-1024":10,"kmalloc-2048":11,"kmalloc-4096":12,
						"kmalloc-8192":13}
		self._red = "\033[31m{0}\033[0m"
		self._green = "\033[32m{0}\033[0m"
		#self._green_hex = "\033[32m{:>#x}\033[0m"
		self._yellow = "\033[33m{0}\033[0m"
		self._blue = "\033[34m{0}\033[0m"
		self._freelist_harden = False
		self._freelist_harden_swab = False
		self._freelist_random = False
		self._freelist_size2 = False

	def initSlub(self,config):
		self.__per_cpu_offset = gdb.parse_and_eval('__per_cpu_offset')
		self._kmalloc_caches = gdb.parse_and_eval('kmalloc_caches')
		#a = gdb.selected_frame().read_var('kmalloc_caches')
		while(self.__per_cpu_offset[self._cpu_amount] != self.__per_cpu_offset[self._cpu_amount+1]):
			self._cpu_amount = self._cpu_amount + 1
		if(gdb.parse_and_eval('kmem_cache').type == gdb.parse_and_eval('kmalloc_caches[1]').type):
			self._demation = 1
		else:
			self._demation = 2
		if(self._demation == 2):
			self._kmalloc_caches = gdb.parse_and_eval('kmalloc_caches[0]')
		for i in config:
			print(i)
			if( i == "freelist_harden"):
				self._freelist_harden = True
			if( i == "freelist_harden_swab"):
				self._freelist_harden_swab = True
			if( i == "freelist_random"):
				print("get random")
				self._freelist_random = True
			if( i == "freelist_size2"):
				print("get size2")
				self._freelist_size2 = True


	def printHelp(self):
		print("Help:")
		print("You have to init the config!Just work on DEBUG_MOD!")
		print("First:\tpigSlub init [freelist_harden] [freelist_harden_swab] [freelist_random] [freelist_size2]")
		print("Usage:\n\tpigSlub [cpu0/...]\n\tpigSlub [kmalloc-32/...]\n\tpigSlub all")


	def printCpuFreelist(self,kmalloc_xxx,cpu_num):
		#print("printCpuFreelist------FUNC")
		freelist_addr = int(self._kmalloc_caches[self._kmalloc_xxx_dict[kmalloc_xxx]]["cpu_slab"]) + int(self.__per_cpu_offset[cpu_num])
		head_chunk = u64(readMemory(freelist_addr,0x8))
		times = 3
		random_value = 0
		kmalloc_size = int(kmalloc_xxx[8:],10)
		if(self._freelist_harden):
			random_value = int(self._kmalloc_caches[self._kmalloc_xxx_dict[kmalloc_xxx]]["random"])
		if(self._freelist_random):
			times = 0
		if( random_value != 0 ):
			print(self._yellow.format("  Freelist : {0} | random_value : {1}\n    ".format(hex(freelist_addr),hex(random_value))),end= "")
		else:
			print(self._yellow.format("  Freelist : {0}\n    ".format(hex(freelist_addr))),end= "")
		
		print(self._green.format(hex(head_chunk)),end = "")
		print(self._green.format("-->"),end = "")


		for i in range(times):
			try:
				if(self._freelist_size2):
					if(kmalloc_size == 8):
						fd_addr = head_chunk
					else:
						fd_addr = head_chunk + int(kmalloc_size/2)
					#print("FD_addr:{:#x}".format(fd_addr))
					#print(hex(u64(readMemory(fd_addr,0x8))))
					if(self._freelist_harden):
						if(self._freelist_harden_swab):
							next_chunk = u64(readMemory(fd_addr,0x8)) ^ random_value ^ swab64(fd_addr)
						else:
							next_chunk = u64(readMemory(fd_addr,0x8)) ^ random_value ^ fd_addr
					else:
						next_chunk = u64(readMemory(fd_addr,0x8))
				else:
					fd_addr = head_chunk
					#print("FD_addr:{:#x}".format(fd_addr))
					if(self._freelist_harden):
						if(self._freelist_harden_swab):
							next_chunk = u64(readMemory(fd_addr,0x8)) ^ random_value ^ swab64(fd_addr)
						else:
							next_chunk = u64(readMemory(fd_addr,0x8)) ^ random_value ^ fd_addr
					else:
						next_chunk = u64(readMemory(fd_addr,0x8))
				if(next_chunk):
					print(self._green.format(hex(next_chunk)),end = "")
					print(self._green.format("-->"),end = "")
					head_chunk = next_chunk
			except Exception as e:
				print(e)
				break
		print(self._green.format("......."))
		print(self._blue.format("*"*0x40))



	def printCPU(self,cpu_num):
		print(self._red.format("-"*0x48))
		print(self._red.format("CPU-" + str(cpu_num)))
		for key in self._kmalloc_xxx_dict.keys():
			print(self._yellow.format("  "+key))
			self.printCpuFreelist(key,cpu_num)

	def printKmalloc(self,kmalloc_xxx):
		for i in range(self._cpu_amount):
			print(self._red.format("-"*0x48))
			print(self._red.format("CPU-" + str(i)))
			self.printCpuFreelist(kmalloc_xxx,i)

	
	def printAll(self):
		for i in range(self._cpu_amount):
			self.printCPU(i)


	def invoke(self, arg, from_tty):
		if not arg:
			print('Missing option. Type \"pigSlub help\" for more information.')
		else:
			args = arg.split()
			#print(args[0])
			if args[0] == "help":
				self.printHelp()
			elif args[0] == 'init':
				config = args[1:]
				self.initSlub(config)
			elif args[0] == 'all':
				self.printAll()
			elif args[0].startswith("kmalloc-"):
				print("kmalloc-func")
				self.printKmalloc(args[0])
			elif args[0].startswith("CPU") or args[0].startswith("cpu"):
				self.printCPU(int(args[0][3:]))

class pigSlab(gdb.Command):
	def __init__ (self):
		super(pigSlab, self).__init__ ("pigSlab", gdb.COMMAND_USER)
		self._demation = 0
		self._cpu_amount = 0
		self.__per_cpu_offset = None
		self._kmalloc_caches = None
		self._kmalloc_xxx_dict = {"kmalloc-96":1,"kmalloc-192":2,"kmalloc-32":5,
						"kmalloc-64":6,"kmalloc-128":7,"kmalloc-256":8,
						"kmalloc-512":9,"kmalloc-1K":10,"kmalloc-2K":11,
						"kmalloc-4K":12,"kmalloc-8K":13,"kmalloc-16K":14,
						"kmalloc-32K":15,"kmalloc-64K":16,"kmalloc-128K":17,
						"kmalloc-256K":18,"kmalloc-512K":19,"kmalloc-1M":20,
						"kmalloc-2M":21,"kmalloc-4M":22}
		self._red = "\033[31m{0}\033[0m"
		self._green = "\033[32m{0}\033[0m"
		#self._green_hex = "\033[32m{:>#x}\033[0m"
		self._yellow = "\033[33m{0}\033[0m"
		self._blue = "\033[34m{0}\033[0m"

	def initSlab(self,config):
		self.__per_cpu_offset = gdb.parse_and_eval('__per_cpu_offset')
		self._kmalloc_caches = gdb.parse_and_eval('kmalloc_caches')
		#a = gdb.selected_frame().read_var('kmalloc_caches')
		while(self.__per_cpu_offset[self._cpu_amount] != self.__per_cpu_offset[self._cpu_amount+1]):
			self._cpu_amount = self._cpu_amount + 1
		if(gdb.parse_and_eval('kmem_cache').type == gdb.parse_and_eval('kmalloc_caches[1]').type):
			self._demation = 1
		else:
			self._demation = 2
		if(self._demation == 2):
			self._kmalloc_caches = gdb.parse_and_eval('kmalloc_caches[0]')

	def printHelp(self):
		print("Help:")
		print("You have to init the config!Just work on DEBUG_MOD!")
		print("First:\tpigSlab init")
		print("Usage:\n\tpigSlab [cpu0/...]\n\tpigSlab [kmalloc-32/...]\n\tpigSlab all")


	def printCpuFreelist(self,kmalloc_xxx,cpu_num):
		#print("printCpuFreelist------FUNC")
		freelist_addr = int(self._kmalloc_caches[self._kmalloc_xxx_dict[kmalloc_xxx]]["cpu_cache"]) + int(self.__per_cpu_offset[cpu_num]) + 0x10
		#print(hex(freelist_addr))
		#print("GETS")
		chunk_amout = u32(readMemory(freelist_addr-0x10,0x4))
		head_chunk = u64(readMemory(freelist_addr,0x8))

		print(self._yellow.format("  Freelist : {0}\n    ".format(hex(freelist_addr))),end= "")
		for i in range(chunk_amout,0,-1):
			try:
				next_chunk = u64(readMemory(freelist_addr+0x8*(i-1),0x8))
				if(next_chunk):
					print(self._green.format(hex(next_chunk)),end = "")
					print(self._green.format("-->"),end = "")
					head_chunk = next_chunk
			except Exception as e:
				print(e)
				break
		print(self._green.format("......."))
		print(self._blue.format("*"*0x40))



	def printCPU(self,cpu_num):
		print(self._red.format("-"*0x48))
		print(self._red.format("CPU-" + str(cpu_num)))
		for key in self._kmalloc_xxx_dict.keys():
			print(self._yellow.format("  "+key))
			self.printCpuFreelist(key,cpu_num)

	def printKmalloc(self,kmalloc_xxx):
		for i in range(self._cpu_amount):
			print(self._red.format("-"*0x48))
			print(self._red.format("CPU-" + str(i)))
			self.printCpuFreelist(kmalloc_xxx,i)

	
	def printAll(self):
		for i in range(self._cpu_amount):
			self.printCPU(i)


	def invoke(self, arg, from_tty):
		if not arg:
			print('Missing option. Type \"pigSlab help\" for more information.')
		else:
			args = arg.split()
			#print(args[0])
			if args[0] == "help":
				self.printHelp()
			elif args[0] == 'init':
				config = args[1:]
				self.initSlab(config)
			elif args[0] == 'all':
				self.printAll()
			elif args[0].startswith("kmalloc-"):
				print("kmalloc-func")
				self.printKmalloc(args[0])
			elif args[0].startswith("CPU") or args[0].startswith("cpu"):
				self.printCPU(int(args[0][3:]))
pigSlab()
pigSlub()