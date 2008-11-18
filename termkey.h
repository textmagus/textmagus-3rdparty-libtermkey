#ifndef GUARD_TERMKEY_H_
#define GUARD_TERMKEY_H_

#include <stdint.h>
#include <stdlib.h>

#define TERMKEY_VERSION_MAJOR 0
#define TERMKEY_VERSION_MINOR 1

#define TERMKEY_CHECK_VERSION \
        termkey_check_version(TERMKEY_VERSION_MAJOR, TERMKEY_VERSION_MINOR)

typedef enum {
  TERMKEY_SYM_UNKNOWN = -1,
  TERMKEY_SYM_NONE = 0,

  // Special names in C0
  TERMKEY_SYM_BACKSPACE,
  TERMKEY_SYM_TAB,
  TERMKEY_SYM_ENTER,
  TERMKEY_SYM_ESCAPE,

  // Special names in G0
  TERMKEY_SYM_SPACE,
  TERMKEY_SYM_DEL,

  // Special keys
  TERMKEY_SYM_UP,
  TERMKEY_SYM_DOWN,
  TERMKEY_SYM_LEFT,
  TERMKEY_SYM_RIGHT,
  TERMKEY_SYM_BEGIN,
  TERMKEY_SYM_FIND,
  TERMKEY_SYM_INSERT,
  TERMKEY_SYM_DELETE,
  TERMKEY_SYM_SELECT,
  TERMKEY_SYM_PAGEUP,
  TERMKEY_SYM_PAGEDOWN,
  TERMKEY_SYM_HOME,
  TERMKEY_SYM_END,

  // Special keys from terminfo
  TERMKEY_SYM_CANCEL,
  TERMKEY_SYM_CLEAR,
  TERMKEY_SYM_CLOSE,
  TERMKEY_SYM_COMMAND,
  TERMKEY_SYM_COPY,
  TERMKEY_SYM_EXIT,
  TERMKEY_SYM_HELP,
  TERMKEY_SYM_MARK,
  TERMKEY_SYM_MESSAGE,
  TERMKEY_SYM_MOVE,
  TERMKEY_SYM_OPEN,
  TERMKEY_SYM_OPTIONS,
  TERMKEY_SYM_PRINT,
  TERMKEY_SYM_REDO,
  TERMKEY_SYM_REFERENCE,
  TERMKEY_SYM_REFRESH,
  TERMKEY_SYM_REPLACE,
  TERMKEY_SYM_RESTART,
  TERMKEY_SYM_RESUME,
  TERMKEY_SYM_SAVE,
  TERMKEY_SYM_SUSPEND,
  TERMKEY_SYM_UNDO,

  // Numeric keypad special keys
  TERMKEY_SYM_KP0,
  TERMKEY_SYM_KP1,
  TERMKEY_SYM_KP2,
  TERMKEY_SYM_KP3,
  TERMKEY_SYM_KP4,
  TERMKEY_SYM_KP5,
  TERMKEY_SYM_KP6,
  TERMKEY_SYM_KP7,
  TERMKEY_SYM_KP8,
  TERMKEY_SYM_KP9,
  TERMKEY_SYM_KPENTER,
  TERMKEY_SYM_KPPLUS,
  TERMKEY_SYM_KPMINUS,
  TERMKEY_SYM_KPMULT,
  TERMKEY_SYM_KPDIV,
  TERMKEY_SYM_KPCOMMA,
  TERMKEY_SYM_KPPERIOD,
  TERMKEY_SYM_KPEQUALS,

  // et cetera ad nauseum
} termkey_sym;

typedef enum {
  TERMKEY_TYPE_UNICODE,
  TERMKEY_TYPE_FUNCTION,
  TERMKEY_TYPE_KEYSYM
} termkey_type;

typedef enum {
  TERMKEY_RES_NONE,
  TERMKEY_RES_KEY,
  TERMKEY_RES_EOF,
  TERMKEY_RES_AGAIN,
} termkey_result;

enum {
  TERMKEY_KEYMOD_SHIFT = 1 << 0,
  TERMKEY_KEYMOD_ALT   = 1 << 1,
  TERMKEY_KEYMOD_CTRL  = 1 << 2,
};

typedef int termkey_keysym;

typedef struct {
  termkey_type type;
  union {
    long           codepoint; // TERMKEY_TYPE_UNICODE
    int            number;    // TERMKEY_TYPE_FUNCTION
    termkey_keysym sym;       // TERMKEY_TYPE_KEYSYM
  } code;
  int modifiers;

  /* Any Unicode character can be UTF-8 encoded in no more than 6 bytes, plus
   * terminating NUL */
  char utf8[7];
} termkey_key;

typedef struct termkey termkey_t;

enum {
  TERMKEY_FLAG_NOINTERPRET = 1 << 0, // Do not interpret C0//G1 codes if possible
  TERMKEY_FLAG_CONVERTKP   = 1 << 1, // Convert KP codes to regular keypresses
  TERMKEY_FLAG_RAW         = 1 << 2, // Input is raw bytes, not UTF-8
  TERMKEY_FLAG_UTF8        = 1 << 3, // Input is definitely UTF-8
  TERMKEY_FLAG_NOTERMIOS   = 1 << 4, // Do not make initial termios calls on construction
};

void termkey_check_version(int major, int minor);

termkey_t *termkey_new(int fd, int flags);
void       termkey_free(termkey_t *tk);
void       termkey_destroy(termkey_t *tk);

int        termkey_get_flags(termkey_t *tk);
void       termkey_set_flags(termkey_t *tk, int newflags);

void       termkey_set_waittime(termkey_t *tk, int msec);
int        termkey_get_waittime(termkey_t *tk);

termkey_result termkey_getkey(termkey_t *tk, termkey_key *key);
termkey_result termkey_getkey_force(termkey_t *tk, termkey_key *key);
termkey_result termkey_waitkey(termkey_t *tk, termkey_key *key);

void       termkey_pushinput(termkey_t *tk, unsigned char *input, size_t inputlen);

termkey_result termkey_advisereadable(termkey_t *tk);

termkey_keysym termkey_register_keyname(termkey_t *tk, termkey_keysym sym, const char *name);
const char *termkey_get_keyname(termkey_t *tk, termkey_keysym sym);

typedef enum {
  TERMKEY_FORMAT_LONGMOD     = 1 << 0, // Shift-... instead of S-...
  TERMKEY_FORMAT_CARETCTRL   = 1 << 1, // ^X instead of C-X
  TERMKEY_FORMAT_ALTISMETA   = 1 << 2, // Meta- or M- instead of Alt- or A-
  TERMKEY_FORMAT_WRAPBRACKET = 1 << 3, // Wrap special keys in brackets like <Escape>
} termkey_format;

// Some useful combinations

#define TERMKEY_FORMAT_VIM (TERMKEY_FORMAT_ALTISMETA|TERMKEY_FORMAT_WRAPBRACKET)

size_t termkey_snprint_key(termkey_t *tk, char *buffer, size_t len, termkey_key *key, termkey_format format);

#endif
