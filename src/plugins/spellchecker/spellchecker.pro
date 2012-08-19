TARGET = spellchecker
include(spellchecker.pri)
include(../plugins.inc)

USE_ENCHANT {
  include(enchantchecker.inc)
} else:USE_ASPELL {
  include(aspellchecker.inc)
} else:USE_MACSPELL {
  include(macspellchecker.inc)
} else:USE_HUNSPELL {
  include(hunspellchecker.inc)
} else:macx {
  include(macspellchecker.inc)
} else {
  include(hunspellchecker.inc)
}