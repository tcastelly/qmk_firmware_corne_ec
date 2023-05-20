SRC += quantizer_mouse.c

include keyboards/sekigon/keyboard_quantizer/mini/keymaps/full/dynamic_config/rules.mk
include keyboards/sekigon/keyboard_quantizer/mini/keymaps/full/cli/rules.mk

VPATH += keyboards/sekigon/keyboard_quantizer/mini/keymaps/full/dynamic_config/
VPATH += keyboards/sekigon/keyboard_quantizer/mini/keymaps/full/cli/

KEY_OVERRIDE_ENABLE = yes
COMBO_ENABLE = yes
TAP_DANCE_ENABLE = yes
LEADER_ENABLE = yes