| Functionality            | Enable              | Toggle |
|---------------|-------|---------------------|--------------------|
| Direction, pinMode() | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, HIGH or LOW | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`         | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`          | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup  | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`        | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`       | `PIN_PULLUP_TGL`       |
| Invert HIGH and LOW |`PIN_INVERT_ON`        | `PIN_INVERT_OFF`       | `PIN_INVERT_TGL`       |
| Use TTL levels (DB/DD only) |`PIN_INLVL_TTL`<br/>`PININLVL_ON`        | `PIN_INLVL_SCHMIT`<br/>`PIN_INLVL_OFF`     | `PIN_INLVL_TGL`        |
| Digital input buffer | `PIN_INPUT_ENABLE`     | `PIN_INPUT_DISABLE`    |  |
| Interrupt on change | `PIN_INT_CHANGE`       | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     |  |
| Interrupt on Rise  | `PIN_INT_RISE`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     |  |
| Interrupt on Fall  | `PIN_INT_FALL`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      |  |
| Interrupt on LOW  | `PIN_INT_LEVEL`        | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      |  |
