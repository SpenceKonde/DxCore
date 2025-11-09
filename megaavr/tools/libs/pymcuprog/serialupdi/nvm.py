"""
NVM implementations on various UPDI device families
"""
from logging import getLogger
from pymcuprog.pymcuprog_errors import PymcuprogError
from . import constants
from .timeout import Timeout
from time import sleep
import pause_mod

class NvmUpdi(object):
    """
    Base class for NVM
    """

    def __init__(self, readwrite, device):
        self.logger = getLogger(__name__)
        self.readwrite = readwrite
        self.device = device

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller
        """
        raise NotImplementedError("NVM stack not ready")

    def write_flash(self, address, data):
        """
        Writes data to flash
        :param address: address to write to
        :param data: data to write
        """
        raise NotImplementedError("NVM stack not ready")

    def write_eeprom(self, address, data):
        """
        Write data to EEPROM
        :param address: address to write to
        :param data: data to write
        """
        raise NotImplementedError("NVM stack not ready")

    def write_fuse(self, address, data):
        """
        Writes one fuse value
        :param address: address to write to
        :param data: data to write
        """
        raise NotImplementedError("NVM stack not ready")

    def wait_flash_ready(self):
        """
        Waits for the NVM controller to be ready
        """
        timeout = Timeout(10000)  # 10 sec timeout, just to be sure

        self.logger.debug("Wait flash ready")
        while not timeout.expired():
            status = self.readwrite.read_byte(self.device.nvmctrl_address + constants.UPDI_NVMCTRL_STATUS)
            if status & (1 << constants.UPDI_NVM_STATUS_WRITE_ERROR):
                self.logger.error("NVM error")
                return False

            if not status & ((1 << constants.UPDI_NVM_STATUS_EEPROM_BUSY) |
                             (1 << constants.UPDI_NVM_STATUS_FLASH_BUSY)):
                return True

        self.logger.error("Wait flash ready timed out")
        return False

    def execute_nvm_command(self, command):
        """
        Executes an NVM COMMAND on the NVM CTRL
        :param command: command to execute
        """
        self.logger.debug("NVMCMD %d executing", command)
        return self.readwrite.write_byte(self.device.nvmctrl_address + constants.UPDI_NVMCTRL_CTRLA, command)


class NvmUpdiP0(NvmUpdi):
    """
    AKA Version 0 UPDI NVM
    Present on, for example, tiny817 -> mega4809
    paged write, no RWW
    """

    def __init__(self, readwrite, device):
        NvmUpdi.__init__(self, readwrite, device)
        self.logger = getLogger(__name__)

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller

        Note that on locked devices this is not possible
        and the ERASE KEY has to be used instead, see the unlock method
        """
        self.logger.info("Chip erase using NVM CTRL")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_flash_ready():
            raise IOError("Timeout waiting for flash ready before erase ")

        # Erase
        self.execute_nvm_command(constants.UPDI_V0_NVMCTRL_CTRLA_CHIP_ERASE)

        # And wait for it
        if not self.wait_flash_ready():
            raise IOError("Timeout waiting for flash ready after erase")

        return True

    def write_flash(self, address, data, blocksize=2, bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to flash (v0)
        :param address: address to write to
        :param data: data to write
        """
        if len(data) == 1:
            if (address >= 0x1280) and (address <= 0x128A):
                self.logger.info("Write to address 0x%06x (which is either the lockbyte or a fuse)")
                return write_fuse(address, data, write_delay = 2)
        if (len(data) % 2 == 1) and (len(data) <= 256): # The second condition should never be false. No part with NVMv0 has been released or
            # announced with a page size larger than 128b, and the current logic works up to twice that voltage. The situation that triggers this
            # codepath can (will) happen at the very end of an upload if you trick the compiler into generating a binary with an odd length
            # This is quite uncommon, but (because it happens only at the end of a write) the use of a b
            return self.write_nvm(address, data, use_word_access=False, blocksize=blocksize, bulkwrite = 0, pagewrite_delay=pagewrite_delay)
        return self.write_nvm(address, data, use_word_access=True, blocksize=blocksize,  bulkwrite=bulkwrite, pagewrite_delay=pagewrite_delay)

    def write_eeprom(self, address, data):
        """
        Write data to EEPROM (v0)
        :param address: address to write to
        :param data: data to write
        """
        return self.write_nvm(address, data, use_word_access=False, nvmcommand=constants.UPDI_V0_NVMCTRL_CTRLA_ERASE_WRITE_PAGE)

    def write_fuse(self, address, data, write_delay=1):
        """
        Writes one fuse value (v0)
        :param address: address to write to
        :param data: data to write
        :param write_delay: only default (1) is used ever. pause after every write, as fusewrite failures have been encountered without it.
        """

        # Check that NVM controller is ready
        if not self.wait_flash_ready():
            raise PymcuprogError("Timeout waiting for flash ready before fuse write ")

        # Write address to NVMCTRL ADDR
        self.logger.debug("Load NVM address")
        self.readwrite.write_byte(self.device.nvmctrl_address + constants.UPDI_NVMCTRL_ADDRL, address & 0xFF)
        self.readwrite.write_byte(self.device.nvmctrl_address + constants.UPDI_NVMCTRL_ADDRH, (address >> 8) & 0xFF)

        # Write data
        self.logger.debug("Load fuse data")
        self.readwrite.write_byte(self.device.nvmctrl_address + constants.UPDI_NVMCTRL_DATAL, data[0] & 0xFF)

        # Execute
        self.logger.debug("Execute fuse write")
        self.execute_nvm_command(constants.UPDI_V0_NVMCTRL_CTRLA_WRITE_FUSE)

        if write_delay > 0:
            pause_mod.milliseconds(write_delay)
        if not self.wait_flash_ready():
            raise PymcuprogError("Timeout waiting for flash ready after fuse write ")

    def write_nvm(self, address, data, use_word_access, nvmcommand=constants.UPDI_V0_NVMCTRL_CTRLA_WRITE_PAGE, blocksize=2,  bulkwrite=0, pagewrite_delay=0):
        """
        Writes a page of data to NVM (v0)

        By default the PAGE_WRITE command is used, which
        requires that the page is already erased.
        By default word access is used (flash)
        :param address: address to write to
        :param data: data to write
        :param use_word_access: write whole words?
        :param nvmcommand: command to use for commit
        :param bulkwrite: Passed down from nvmserialupdi 0 = normal or single write.
            1 means it's part of writing the whole flash.
            In that case we only st ptr if address = 0.
        :param pagewrite_delay: (ms) delay before pagewrite

        """

        # unless we are in a bulk (whole flash) write, in which case we skip almost everything.
        if (bulkwrite == 0 ) or address == 0x8000 or address == 0x4000 or not use_word_access:
            # Check that NVM controller is ready
            # I will grudgingly check this at the very start. I am extremely skeptical about the usefulness of this test.
            # If it's not ready, they'll get another error will they not? Every command like this costs about a half second
            # on every upload when using serialupdi - at any bsaud rate, assuming 256 pages. It's all USB latency.
            if not self.wait_flash_ready():
                raise PymcuprogError("Timeout waiting for flash ready before page buffer clear ")
                # Clear the page buffer
            self.logger.debug("Clear page buffer")
            self.execute_nvm_command(constants.UPDI_V0_NVMCTRL_CTRLA_PAGE_BUFFER_CLR)

             # Wait for NVM controller to be ready
            if not self.wait_flash_ready():
                raise PymcuprogError("Timeout waiting for flash ready after page buffer clear")

        # Load the page buffer by writing directly to location
        if use_word_access:
            self.readwrite.write_data_words(address, data, blocksize)
        else:
            self.readwrite.write_data(address, data)

        # Write the page to NVM, maybe erase first
        self.logger.debug("Committing data")

        self.execute_nvm_command(nvmcommand)

        if pagewrite_delay > 0:
            pause_mod.milliseconds(pagewrite_delay)
        # SACRIFICES SPEED FOR COMPATIBILITY - above line should execute only when --pagepause command line parameter is 1 or more (default 0), so we can adjust it externally
        if not bulkwrite == 1:
            # do a final NVM status check only if not doing a bulk write, or after the last chunk (when bulkwrite = 2)
            # not doing this every page made uploads about 15% faster
            if not self.wait_flash_ready():
                raise PymcuprogError("Timeout waiting for flash ready after write")


class NvmUpdiP2(NvmUpdi):
    """
    AKA Version 1 UPDI NVM
    Present on, for example, AVR-DA/DB
    word write
    """

    def __init__(self, readwrite, device):
        NvmUpdi.__init__(self, readwrite, device)
        self.logger = getLogger(__name__)

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller
        Note that on locked devices this it not possible
        and the ERASE KEY has to be used instead
        """
        self.logger.info("Chip erase using NVM CTRL")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for flash ready before erase ")

        # Erase
        self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_CHIP_ERASE)

        # And wait for it
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for flash ready after erase")

        return True

    def write_flash(self, address, data, blocksize=2, bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to flash (v1)
        :param address: address to write to
        :param data: data to write
        :return:
        """
        return self.write_nvm(address, data, use_word_access=True, blocksize=blocksize, bulkwrite=bulkwrite, pagewrite_delay=pagewrite_delay)

    def write_eeprom(self, address, data):
        """
        Writes data to NVM (EEPROM)
        :param address: address to write to
        :param data: data to write
        """
        nvm_command = constants.UPDI_V1_NVMCTRL_CTRLA_EEPROM_ERASE_WRITE

        # Check that NVM controller is ready
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for NVM ready before command write")

        # Write the command to the NVM controller
        self.logger.info("NVM EEPROM erase/write command")
        self.execute_nvm_command(nvm_command)

        # Write the data
        self.readwrite.write_data(address, data)

        # Wait for NVM controller to be ready again
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for NVM ready after data write")

        # Remove command from NVM controller
        self.logger.info("Clear NVM command")
        self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_NOCMD)

    def write_fuse(self, address, data):
        """
        Writes one fuse value
        V1 fuses are EEPROM-based
        :param address: address to write to
        :param data: data to write
        """
        return self.write_eeprom(address, data)

    def write_nvm(self, address, data, use_word_access, blocksize=2, bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to NVM (version 1)
        This version of the NVM block has no page buffer, so words are written directly.
        :param address: address to write to
        :param data: data to write
        :param use_word_access: write in whole words, almost always true.
        :param pagewrite_delay: not used on AVR Dx (V1 NVMCTRL) - these do not have page writes as a concept.
        """
        nvm_command = constants.UPDI_V1_NVMCTRL_CTRLA_FLASH_WRITE

        if bulkwrite == 0 or (address & 32767) == 0:
            # Check that NVM controller is ready
            if not self.wait_flash_ready():
                raise Exception("Timeout waiting for flash ready before nvm write ")

            # Write the command to the NVM controller
            self.logger.info("NVM write command")
            self.execute_nvm_command(nvm_command)

        # Write the data
        if use_word_access:
            self.readwrite.write_data_words(address, data, blocksize)
        else:
            self.readwrite.write_data(address, data)

        # Wait for NVM controller to be ready again
        if bulkwrite != 1:
            if not self.wait_flash_ready():
                raise Exception("Timeout waiting for flash ready after data write")

            # Remove command from NVM controller
            self.logger.info("Clear NVM command")
            self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_NOCMD)


class NvmUpdiP3(NvmUpdi):
    """
    Version P:3 UPDI NVM properties
    """

    # NVM CTRL peripheral definition
    NVMCTRL_CTRLA = 0x00
    NVMCTRL_CTRLB = 0x01
    NVMCTRL_INTCTRL = 0x04
    NVMCTRL_INTFLAGS = 0x05
    NVMCTRL_STATUS = 0x06
    NVMCTRL_DATA = 0x08 # 16-bit
    NVMCTRL_ADDR = 0x0C # 24-bit

    # CTRLA commands
    NVMCMD_NOCMD = 0x00
    NVMCMD_NOOP = 0x01
    NVMCMD_FLASH_PAGE_WRITE = 0x04
    NVMCMD_FLASH_PAGE_ERASE_WRITE = 0x05
    NVMCMD_FLASH_PAGE_ERASE = 0x08
    NVMCMD_FLASH_PAGE_BUFFER_CLEAR = 0x0F
    NVMCMD_EEPROM_PAGE_WRITE = 0x14
    NVMCMD_EEPROM_PAGE_ERASE_WRITE = 0x15
    NVMCMD_EEPROM_PAGE_ERASE = 0x17
    NVMCMD_EEPROM_PAGE_BUFFER_CLEAR = 0x1F
    NVMCMD_CHIP_ERASE = 0x20
    NVMCMD_EEPROM_ERASE = 0x30

    # STATUS
    STATUS_WRITE_ERROR_bm = 0x70
    STATUS_WRITE_ERROR_bp = 4
    STATUS_EEPROM_BUSY_bp = 0
    STATUS_FLASH_BUSY_bp = 1


    def __init__(self, readwrite, device):
        NvmUpdi.__init__(self, readwrite, device)
        self.logger = getLogger(__name__)

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller

        Note that on locked devices this is not possible and the ERASE KEY has to be used instead, see the unlock method
        """
        self.logger.debug("Chip erase using NVM CTRL")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before chip erase")

        # Erase
        self.execute_nvm_command(self.NVMCMD_CHIP_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after chip erase")


    def erase_flash_page(self, address):
        """
        Erasing single flash page using the NVM controller

        :param address: Start address of page to erase
        :type address: int
        """
        self.logger.debug("Erase flash page at address 0x%08X", address)

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before flash page erase")

        # Dummy write
        self.readwrite.write_data(address, [0xFF])

        # Erase
        self.execute_nvm_command(self.NVMCMD_FLASH_PAGE_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after flash page erase")

    def erase_eeprom(self):
        """
        Erase EEPROM memory only
        """
        self.logger.debug("Erase EEPROM")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before EEPROM erase")

        # Erase
        self.execute_nvm_command(self.NVMCMD_EEPROM_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after EEPROM erase")

    def erase_user_row(self, address, size=0):
        """
        Erase User Row memory only

        :param address: Start address of user row
        :type address: int
        :param size: Size of user row
        :type size: int, optional, not used for this variant
        """
        self.logger.debug("Erase user row")

        # size is not used for this NVM version
        _dummy = size
        # On this NVM version user row is implemented as FLASH
        return self.erase_flash_page(address)

    def write_flash(self, address, data, blocksize=2,  bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to flash

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_nvm(address, data, use_word_access=True, blocksize=blocksize,  bulkwrite=bulkwrite, pagewrite_delay=pagewrite_delay)

    def write_user_row(self, address, data):
        """
        Writes data to user row

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        # On this NVM variant user row is implemented as FLASH
        return self.write_nvm(address, data, use_word_access=True)

    def write_eeprom(self, address, data):
        """
        Write data to EEPROM

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_nvm(address, data, use_word_access=False,
                              nvmcommand=self.NVMCMD_EEPROM_PAGE_ERASE_WRITE,
                              erasebuffer_command=self.NVMCMD_EEPROM_PAGE_BUFFER_CLEAR)

    def write_fuse(self, address, data):
        """
        Writes one fuse value

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_eeprom(address, data)

    def write_nvm(self,
                  address,
                  data,
                  use_word_access,
                  nvmcommand=NVMCMD_FLASH_PAGE_WRITE,
                  erasebuffer_command=NVMCMD_FLASH_PAGE_BUFFER_CLEAR, blocksize=2,  bulkwrite=0, pagewrite_delay=0):
        """
        Writes a page of data to NVM

        By default the PAGE_WRITE command is used, which requires that the page is already erased.
        By default word access is used (required for flash)

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        :param use_word_access: True for 16-bit writes (eg: flash)
        :type use_word_access: bool, defaults to True
        :param nvmcommand: command to use for commit
        :type nvmcommand: int, defaults to NVMCMD_PAGE_WRITE
        :param erasebuffer_command: command to use for erasing the page buffer
        :type erasebuffer_command: int, defaults to NVMCMD_FLASH_PAGE_BUFFER_CLEAR
        :raises: PymcuprogSerialUpdiNvmTimeout if a timeout occurred
        :raises: PymcuprogSerialUpdiNvmError if an error condition is encountered
        """

        # Check that NVM controller is ready
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before page buffer clear")

        # Clear the page buffer
        self.logger.debug("Clear page buffer")
        self.execute_nvm_command(erasebuffer_command)

        # Wait for NVM controller to be ready
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after page buffer clear")

        # Load the page buffer by writing directly to location
        if use_word_access:
            self.readwrite.write_data_words(address, data, blocksize)
        else:
            self.readwrite.write_data(address, data)

        # Write the page to NVM, maybe erase first
        self.logger.debug("Committing data")
        self.execute_nvm_command(nvmcommand)

        # Wait for NVM controller to be ready again
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after page write")

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

    def wait_nvm_ready(self, timeout_ms=100):
        """
        Waits for the NVM controller to be ready

        :param timeout_ms: Timeout period in milliseconds
        :type timeout_ms: int, defaults to 100
        :returns: True if 'ready', False if timeout occurred before ready
        :rtype: bool
        :raises: PymcuprogSerialUpdiNvmError if an error condition is encountered
        """
        timeout = Timeout(timeout_ms)

        self.logger.debug("Wait NVM ready")
        while not timeout.expired():
            status = self.readwrite.read_byte(self.device.nvmctrl_address + self.NVMCTRL_STATUS)
            if status & self.STATUS_WRITE_ERROR_bm:
                self.logger.error("NVM error (%d)", status >> self.STATUS_WRITE_ERROR_bp)
                raise PymcuprogSerialUpdiNvmError(msg="NVM error", code=(status >> self.STATUS_WRITE_ERROR_bp))

            if not status & ((1 << self.STATUS_EEPROM_BUSY_bp) | (1 << self.STATUS_FLASH_BUSY_bp)):
                return True

        self.logger.error("Wait NVM ready timed out")
        return False

    def execute_nvm_command(self, command):
        """
        Executes an NVM COMMAND on the NVM CTRL

        :param command: command to execute
        :type param: int
        """
        self.logger.debug("NVMCMD %d executing", command)
        return self.readwrite.write_byte(self.device.nvmctrl_address + self.NVMCTRL_CTRLA, command)


class NvmUpdiP4(NvmUpdi):
    """
    AKA Version 1 UPDI NVM
    Present on, for example, AVR-DA and newer
    word write
    """

    def __init__(self, readwrite, device):
        NvmUpdi.__init__(self, readwrite, device)
        self.logger = getLogger(__name__)

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller
        Note that on locked devices this it not possible
        and the ERASE KEY has to be used instead
        """
        self.logger.info("Chip erase using NVM CTRL")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for flash ready before erase ")

        # Erase
        self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_CHIP_ERASE)

        # And wait for it
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for flash ready after erase")

        return True

    def write_flash(self, address, data, blocksize=2, bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to flash (v1)
        :param address: address to write to
        :param data: data to write
        :return:
        """
        return self.write_nvm(address, data, use_word_access=True, blocksize=blocksize, bulkwrite=bulkwrite, pagewrite_delay=pagewrite_delay)

    def write_eeprom(self, address, data):
        """
        Writes data to NVM (EEPROM)
        :param address: address to write to
        :param data: data to write
        """
        nvm_command = constants.UPDI_V1_NVMCTRL_CTRLA_EEPROM_ERASE_WRITE

        # Check that NVM controller is ready
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for NVM ready before command write")

        # Write the command to the NVM controller
        self.logger.info("NVM EEPROM erase/write command")
        self.execute_nvm_command(nvm_command)

        # Write the data
        self.readwrite.write_data(address, data)

        # Wait for NVM controller to be ready again
        if not self.wait_flash_ready():
            raise Exception("Timeout waiting for NVM ready after data write")

        # Remove command from NVM controller
        self.logger.info("Clear NVM command")
        self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_NOCMD)

    def write_fuse(self, address, data):
        """
        Writes one fuse value
        V1 fuses are EEPROM-based
        :param address: address to write to
        :param data: data to write
        """
        return self.write_eeprom(address, data)

    def write_nvm(self, address, data, use_word_access, blocksize=2, bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to NVM (version 1)
        This version of the NVM block has no page buffer, so words are written directly.
        :param address: address to write to
        :param data: data to write
        :param use_word_access: write in whole words, almost always true.
        :param pagewrite_delay: not used on AVR Dx (V1 NVMCTRL) - these do not have page writes as a concept.
        """
        nvm_command = constants.UPDI_V1_NVMCTRL_CTRLA_FLASH_WRITE

        if bulkwrite == 0 or (address & 32767) == 0:
            # Check that NVM controller is ready
            if not self.wait_flash_ready():
                raise Exception("Timeout waiting for flash ready before nvm write ")

            # Write the command to the NVM controller
            self.logger.info("NVM write command")
            self.execute_nvm_command(nvm_command)

        # Write the data
        if use_word_access:
            self.readwrite.write_data_words(address, data, blocksize)
        else:
            self.readwrite.write_data(address, data)

        # Wait for NVM controller to be ready again
        if bulkwrite != 1:
            if not self.wait_flash_ready():
                raise Exception("Timeout waiting for flash ready after data write")

            # Remove command from NVM controller
            self.logger.info("Clear NVM command")
            self.execute_nvm_command(constants.UPDI_V1_NVMCTRL_CTRLA_NOCMD)



class NvmUpdiP5(NvmUpdi):
    """
    Version P:5 UPDI NVM properties
    """

    # NVM CTRL peripheral definition
    NVMCTRL_CTRLA = 0x00
    NVMCTRL_CTRLB = 0x01
    NVMCTRL_CTRLC = 0x02
    NVMCTRL_INTCTRL = 0x04
    NVMCTRL_INTFLAGS = 0x05
    NVMCTRL_STATUS = 0x06
    NVMCTRL_DATA = 0x08 # 16-bit
    NVMCTRL_ADDR = 0x0C # 24-bit

    # CTRLA commands
    NVMCMD_NOCMD = 0x00
    NVMCMD_NOOP = 0x01
    NVMCMD_FLASH_PAGE_WRITE = 0x04
    NVMCMD_FLASH_PAGE_ERASE_WRITE = 0x05
    NVMCMD_FLASH_PAGE_ERASE = 0x08
    NVMCMD_FLASH_PAGE_BUFFER_CLEAR = 0x0F
    NVMCMD_EEPROM_PAGE_WRITE = 0x14
    NVMCMD_EEPROM_PAGE_ERASE_WRITE = 0x15
    NVMCMD_EEPROM_PAGE_ERASE = 0x17
    NVMCMD_EEPROM_PAGE_BUFFER_CLEAR = 0x1F
    NVMCMD_CHIP_ERASE = 0x20
    NVMCMD_EEPROM_ERASE = 0x30

    # STATUS
    STATUS_WRITE_ERROR_bm = 0x70
    STATUS_WRITE_ERROR_bp = 4
    STATUS_EEPROM_BUSY_bp = 0
    STATUS_FLASH_BUSY_bp = 1


    def __init__(self, readwrite, device):
        NvmUpdi.__init__(self, readwrite, device)
        self.logger = getLogger(__name__)

    def chip_erase(self):
        """
        Does a chip erase using the NVM controller

        Note that on locked devices this is not possible and the ERASE KEY has to be used instead, see the unlock method
        """
        self.logger.debug("Chip erase using NVM CTRL")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before chip erase")

        # Erase
        self.execute_nvm_command(self.NVMCMD_CHIP_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after chip erase")


    def erase_flash_page(self, address):
        """
        Erasing single flash page using the NVM controller

        :param address: Start address of page to erase
        :type address: int
        """
        self.logger.debug("Erase flash page at address 0x%08X", address)

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before flash page erase")

        # Dummy write
        self.readwrite.write_data(address, [0xFF])

        # Erase
        self.execute_nvm_command(self.NVMCMD_FLASH_PAGE_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after flash page erase")

    def erase_eeprom(self):
        """
        Erase EEPROM memory only
        """
        self.logger.debug("Erase EEPROM")

        # Wait until NVM CTRL is ready to erase
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before EEPROM erase")

        # Erase
        self.execute_nvm_command(self.NVMCMD_EEPROM_ERASE)

        # And wait for it
        status = self.wait_nvm_ready()

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

        if not status:
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after EEPROM erase")

    def erase_user_row(self, address, size=0):
        """
        Erase User Row memory only

        :param address: Start address of user row
        :type address: int
        :param size: Size of user row
        :type size: int, optional, not used for this variant
        """
        self.logger.debug("Erase user row")

        # size is not used for this NVM version
        _dummy = size
        # On this NVM version user row is implemented as FLASH
        return self.erase_flash_page(address)

    def write_flash(self, address, data, blocksize=2,  bulkwrite=0, pagewrite_delay=0):
        """
        Writes data to flash

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_nvm(address, data, use_word_access=True, blocksize=blocksize,  bulkwrite=bulkwrite, pagewrite_delay=pagewrite_delay)

    def write_user_row(self, address, data):
        """
        Writes data to user row

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        # On this NVM variant user row is implemented as FLASH
        return self.write_nvm(address, data, use_word_access=True)

    def write_eeprom(self, address, data):
        """
        Write data to EEPROM

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_nvm(address, data, use_word_access=False,
                              nvmcommand=self.NVMCMD_EEPROM_PAGE_ERASE_WRITE,
                              erasebuffer_command=self.NVMCMD_EEPROM_PAGE_BUFFER_CLEAR)

    def write_fuse(self, address, data):
        """
        Writes one fuse value

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        """
        return self.write_eeprom(address, data)

    def write_nvm(self,
                  address,
                  data,
                  use_word_access,
                  nvmcommand=NVMCMD_FLASH_PAGE_WRITE,
                  erasebuffer_command=NVMCMD_FLASH_PAGE_BUFFER_CLEAR, blocksize=2,  bulkwrite=0, pagewrite_delay=0):
        """
        Writes a page of data to NVM

        By default the PAGE_WRITE command is used, which requires that the page is already erased.
        By default word access is used (required for flash)

        :param address: address to write to
        :type address: int
        :param data: data to write
        :type data: list of bytes
        :param use_word_access: True for 16-bit writes (eg: flash)
        :type use_word_access: bool, defaults to True
        :param nvmcommand: command to use for commit
        :type nvmcommand: int, defaults to NVMCMD_PAGE_WRITE
        :param erasebuffer_command: command to use for erasing the page buffer
        :type erasebuffer_command: int, defaults to NVMCMD_FLASH_PAGE_BUFFER_CLEAR
        :raises: PymcuprogSerialUpdiNvmTimeout if a timeout occurred
        :raises: PymcuprogSerialUpdiNvmError if an error condition is encountered
        """

        # Check that NVM controller is ready
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready before page buffer clear")

        # Clear the page buffer
        self.logger.debug("Clear page buffer")
        self.execute_nvm_command(erasebuffer_command)

        # Wait for NVM controller to be ready
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after page buffer clear")

        # Load the page buffer by writing directly to location
        if use_word_access:
            self.readwrite.write_data_words(address, data, blocksize)
        else:
            self.readwrite.write_data(address, data)

        # Write the page to NVM, maybe erase first
        self.logger.debug("Committing data")
        self.execute_nvm_command(nvmcommand)

        # Wait for NVM controller to be ready again
        if not self.wait_nvm_ready():
            raise PymcuprogSerialUpdiNvmTimeout("Timeout waiting for NVM controller to be ready after page write")

        # Remove command
        self.execute_nvm_command(self.NVMCMD_NOCMD)

    def wait_nvm_ready(self, timeout_ms=100):
        """
        Waits for the NVM controller to be ready

        :param timeout_ms: Timeout period in milliseconds
        :type timeout_ms: int, defaults to 100
        :returns: True if 'ready', False if timeout occurred before ready
        :rtype: bool
        :raises: PymcuprogSerialUpdiNvmError if an error condition is encountered
        """
        timeout = Timeout(timeout_ms)

        self.logger.debug("Wait NVM ready")
        while not timeout.expired():
            status = self.readwrite.read_byte(self.device.nvmctrl_address + self.NVMCTRL_STATUS)
            if status & self.STATUS_WRITE_ERROR_bm:
                self.logger.error("NVM error (%d)", status >> self.STATUS_WRITE_ERROR_bp)
                raise PymcuprogSerialUpdiNvmError(msg="NVM error", code=(status >> self.STATUS_WRITE_ERROR_bp))

            if not status & ((1 << self.STATUS_EEPROM_BUSY_bp) | (1 << self.STATUS_FLASH_BUSY_bp)):
                return True

        self.logger.error("Wait NVM ready timed out")
        return False

    def execute_nvm_command(self, command):
        """
        Executes an NVM COMMAND on the NVM CTRL

        :param command: command to execute
        :type param: int
        """
        self.logger.debug("NVMCMD %d executing", command)
        return self.readwrite.write_byte(self.device.nvmctrl_address + self.NVMCTRL_CTRLA, command)
