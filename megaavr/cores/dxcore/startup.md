
|   with    | no millis | TCD0    | TCA0  |  TCB0 |  RTC   |
|-----------|-----------|---------|-------|-------|--------|
|  nothing  | 136       |  336    |  258  |   252 |   246  |
|delay(200) | 160       |  586    |  538  |   494 |   270  |
|           |           |         |       |       |        |
|           |           |         |       |       |        |
|           |           |         |       |       |        |

What is going on in there? Well...

```
int main(void)
{
  #if (!defined(USING_OPTIBOOT) && defined(SPM_FROM_APP))
    _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
    // iF SPM_FROM_APP is to work, the bootsize must be set to a non-zero value, othwerwise all flash is BOOTCODE which can never be self-programmed.
    // That causes it to default to putting IVs aty begining of app. But we're not doing that, we're just building a normal app.
    // By setting BOOTCODE = 1, the rest of the flash can be made APPCODE

  #endif
  init();

  initVariant();

#if defined(USBCON)
  USBDevice.attach();
#endif

  setup();

  for (;;) {
    loop();
    #if defined (ENABLE_SERIAL_EVENT)
    if (serialEventRun) serialEventRun();
    #endif
  }
  return 0;
}
