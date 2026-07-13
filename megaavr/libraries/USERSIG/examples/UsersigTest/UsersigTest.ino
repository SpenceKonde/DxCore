/***********************************************************************
 * UsersigTest - verify the USERSIG library against the real USERROW.
 *
 * The USERROW ("user signature space") is a small non-volatile area that
 * is NOT touched by a chip erase and survives ordinary sketch uploads.
 * It is meant for the things that belong to the *board* rather than to
 * the sketch: serial numbers, calibration constants, board revisions.
 *
 * On the AVR DU-series it is 512 bytes at 0x1200 - 16x larger than the
 * 32 bytes of the DA/DB/DD parts, which is why anything in the library
 * that assumed an 8-bit index has to be checked.
 *
 * The USERROW is flash-like: a write can only clear bits (1 -> 0). To set
 * a bit back to 1 the whole row must be erased, so the library keeps a RAM
 * copy for those writes:
 *
 *   USERSIG.write(idx, val) returns 1  -> written straight to the USERROW
 *                           returns 0  -> buffered; call USERSIG.flush()
 *   USERSIG.flush()                    -> erase + rewrite, returns the
 *                                         number of bytes that changed
 *   USERSIG.erase()                    -> erase the whole USERROW
 *
 * WARNING: this sketch erases and rewrites the USERROW (2 erase/write
 * cycles per run). The USERROW has a limited endurance - run it to test,
 * not in a loop.
 ***********************************************************************/
#include <USERSIG.h>

/* Where the test data goes. Kept apart so the checks can't mask each other. */
#define MARKER_INDEX  500          /* persistence marker (6 bytes)        */
#define STRUCT_INDEX  480          /* put()/get() test object (12 bytes)  */
#define MARKER_MAGIC  0x47495355UL /* "USIG" */

struct Marker {
  uint32_t magic;
  uint16_t runs;
};

struct Cal {                       /* a plausible real payload            */
  uint32_t serial;
  int16_t  offset;
  char     tag[6];
};

uint8_t tests = 0, failures = 0;

/* Read the USERROW directly, bypassing the library, so that a test cannot
 * be fooled by a library that truncates the index (e.g. index 300 -> 44). */
static inline uint8_t raw(uint16_t idx) {
  return *((volatile uint8_t *)(USER_SIGNATURES_START + idx));
}

void check(const __FlashStringHelper *what, bool ok) {
  tests++;
  if (!ok) {
    failures++;
  }
  Serial.print(ok ? F("  PASS  ") : F("  FAIL  "));
  Serial.println(what);
}

void dump(uint16_t start, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    if ((i & 0x0F) == 0) {
      Serial.print(F("\n  0x"));
      uint16_t a = start + i;
      if (a < 0x100) Serial.print('0');
      if (a < 0x10)  Serial.print('0');
      Serial.print(a, HEX);
      Serial.print(F(": "));
    }
    uint8_t b = raw(start + i);
    if (b < 0x10) Serial.print('0');
    Serial.print(b, HEX);
    Serial.print(' ');
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < 40 && !Serial; i++) {
    delay(50);                     /* give a USB CDC host time to attach */
  }
  delay(200);

  Serial.println(F("\n=== USERSIG (USERROW) test ==="));
  Serial.print(F("USER_SIGNATURES_START : 0x"));
  Serial.println((uint16_t)USER_SIGNATURES_START, HEX);
  Serial.print(F("USER_SIGNATURES_SIZE  : "));
  Serial.println((uint16_t)USER_SIGNATURES_SIZE);
  Serial.print(F("USERSIG.length()      : "));
  Serial.println((uint16_t)USERSIG.length());

  /* Safety gate. If length() disagrees with the device, the library is
   * carrying 8-bit indices/counters somewhere: write() would land on the
   * wrong byte and flush() would spin forever on a 512-byte USERROW. Stop
   * before touching anything. */
  if ((uint16_t)USERSIG.length() != (uint16_t)USER_SIGNATURES_SIZE) {
    Serial.println(F("\n!! The library reports a size that is not the device's USERROW size."));
    Serial.println(F("!! It is not safe for a 512-byte USERROW - a flush() would hang here."));
    Serial.println(F("!! Stopping before any write."));
    return;
  }

  /* Has this board run the test before? Read the marker BEFORE erasing. */
  Marker prev;
  USERSIG.get(MARKER_INDEX, prev);
  Serial.println();
  if (prev.magic == MARKER_MAGIC) {
    Serial.print(F("Marker from a previous run found - test has run "));
    Serial.print(prev.runs);
    Serial.println(F(" time(s) on this board."));
    Serial.println(F("(The USERROW survived the reset/upload.)"));
  } else {
    Serial.println(F("No marker yet: first run on this board, or the USERROW is blank."));
  }

  Serial.println(F("\nUSERROW before the test (first 32 and last 32 bytes):"));
  dump(0, 32);
  dump(USER_SIGNATURES_SIZE - 32, 32);

  /* ---- 1. erase ---------------------------------------------------- */
  Serial.println(F("\n[1] erase()"));
  USERSIG.erase();
  bool blank = true;
  for (uint16_t i = 0; i < USER_SIGNATURES_SIZE; i++) {
    if (raw(i) != 0xFF) {
      blank = false;
      break;
    }
  }
  check(F("all 512 bytes read 0xFF"), blank);

  /* ---- 2. direct writes, including indices above 255 ---------------- */
  Serial.println(F("\n[2] write() on an erased USERROW - goes straight to flash"));
  const uint16_t idx[6] = {0, 1, 255, 256, 300, 511};
  const uint8_t  val[6] = {0xA5, 0x5A, 0x11, 0x22, 0x33, 0x44};
  bool direct = true, stored = true;
  for (uint8_t i = 0; i < 6; i++) {
    if (USERSIG.write(idx[i], val[i]) != 1) {
      direct = false;
    }
  }
  check(F("every write() returned 1 (written immediately)"), direct);

  for (uint8_t i = 0; i < 6; i++) {
    if (raw(idx[i]) != val[i]) {           /* raw: the TRUE address */
      stored = false;
    }
  }
  check(F("bytes landed at the right addresses (0,1,255,256,300,511)"), stored);
  check(F("read() agrees with the USERROW"),
        USERSIG.read(300) == 0x33 && USERSIG.read(511) == 0x44);

  /* ---- 3. a write that needs an erase is buffered ------------------- */
  Serial.println(F("\n[3] write() that needs a bit set back to 1 - buffered"));
  int8_t r = USERSIG.write(0, 0xFF);       /* 0xA5 -> 0xFF needs an erase */
  check(F("write() returned 0 (deferred until flush)"), r == 0);
  check(F("read() already reports the pending value"), USERSIG.read(0) == 0xFF);
  check(F("the USERROW itself is still unchanged"), raw(0) == 0xA5);

  /* ---- 4. flush ----------------------------------------------------- */
  Serial.println(F("\n[4] flush() - erase and rewrite the row from the buffer"));
  int16_t changed = USERSIG.flush();
  Serial.print(F("  flush() reported changed bytes: "));
  Serial.println(changed);
  check(F("exactly one byte was reported as changed"), changed == 1);
  check(F("the new value reached the USERROW"), raw(0) == 0xFF);
  check(F("the other bytes survived the erase/rewrite"),
        raw(1) == 0x5A && raw(255) == 0x11 && raw(256) == 0x22 &&
        raw(300) == 0x33 && raw(511) == 0x44);

  /* ---- 5. put()/get() an object, above index 255 --------------------- */
  Serial.println(F("\n[5] put()/get() a 12-byte object at index 480"));
  Cal out = {0xDEADBEEF, -1234, "DUtst"};
  USERSIG.put(STRUCT_INDEX, out);
  USERSIG.flush();                          /* commit if anything was buffered */
  Cal in;
  USERSIG.get(STRUCT_INDEX, in);
  check(F("the object survived the round trip"),
        in.serial == out.serial && in.offset == out.offset &&
        strcmp(in.tag, out.tag) == 0);
  check(F("it really is stored at index 480"),
        raw(STRUCT_INDEX) == (uint8_t)(out.serial & 0xFF));

  /* ---- 6. persistence marker ---------------------------------------- */
  Serial.println(F("\n[6] persistence marker"));
  Marker next;
  next.magic = MARKER_MAGIC;
  next.runs  = (prev.magic == MARKER_MAGIC) ? (uint16_t)(prev.runs + 1) : 1;
  USERSIG.put(MARKER_INDEX, next);
  USERSIG.flush();
  Marker back;
  USERSIG.get(MARKER_INDEX, back);
  check(F("marker written and read back"),
        back.magic == MARKER_MAGIC && back.runs == next.runs);

  Serial.println(F("\nUSERROW after the test (first 32 and last 32 bytes):"));
  dump(0, 32);
  dump(USER_SIGNATURES_SIZE - 32, 32);

  /* ---- summary ------------------------------------------------------ */
  Serial.print(F("\nResult: "));
  Serial.print(tests - failures);
  Serial.print('/');
  Serial.print(tests);
  Serial.println(failures ? F(" - *** FAILURES ***") : F(" - ALL PASS"));
  Serial.print(F("Run count stored in the USERROW: "));
  Serial.println(next.runs);
  Serial.println(F("Reset the board, or upload another sketch and this one again,"));
  Serial.println(F("then re-run: the run count must keep increasing - that is the"));
  Serial.println(F("USERROW outliving both the reset and the upload."));
}

void loop() {
  /* Nothing - the USERROW has a limited endurance, so the test runs once. */
}
