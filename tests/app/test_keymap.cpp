#include "test_framework.hpp"

#include "forge/app/keymap.hpp"
#include "forge/app/shortcut.hpp"

FORGE_TEST_CASE("app.shortcut_equality")
{
    const forge::Shortcut a(forge::Key::S, forge::ModifierKey::Ctrl);
    const forge::Shortcut b(forge::Key::S, forge::ModifierKey::Ctrl);
    const forge::Shortcut c(forge::Key::S);
    FORGE_CHECK(a == b);
    FORGE_CHECK(!(a == c));
}

FORGE_TEST_CASE("app.shortcut_to_string")
{
    FORGE_CHECK_EQ(forge::Shortcut(forge::Key::S, forge::ModifierKey::Ctrl).to_string(),
                   std::string("Ctrl+S"));
    FORGE_CHECK_EQ(
        forge::Shortcut(forge::Key::Z, forge::ModifierKey::Ctrl | forge::ModifierKey::Shift)
            .to_string(),
        std::string("Ctrl+Shift+Z"));
    FORGE_CHECK_EQ(forge::Shortcut(forge::Key::F3).to_string(), std::string("F3"));
}

FORGE_TEST_CASE("app.shortcut_parse_round_trip")
{
    const auto parsed = forge::Shortcut::parse("Ctrl+Shift+Z");
    FORGE_REQUIRE(parsed.has_value());
    FORGE_CHECK(parsed->key == forge::Key::Z);
    FORGE_CHECK(forge::has_modifier(parsed->modifiers, forge::ModifierKey::Ctrl));
    FORGE_CHECK(forge::has_modifier(parsed->modifiers, forge::ModifierKey::Shift));

    FORGE_CHECK(!forge::Shortcut::parse("Ctrl+").has_value());     // no key
    FORGE_CHECK(!forge::Shortcut::parse("Bogus+S").has_value());   // unknown token? S valid, Bogus invalid
}

FORGE_TEST_CASE("app.keymap_bind_resolve_unbind")
{
    forge::Keymap keymap;
    const forge::Shortcut save(forge::Key::S, forge::ModifierKey::Ctrl);
    keymap.bind(save, "file.save");

    const auto resolved = keymap.resolve(save);
    FORGE_REQUIRE(resolved.has_value());
    FORGE_CHECK_EQ(*resolved, std::string("file.save"));

    FORGE_CHECK(keymap.unbind(save));
    FORGE_CHECK(!keymap.resolve(save).has_value());
}

FORGE_TEST_CASE("app.keymap_default_bindings")
{
    const forge::Keymap keymap = forge::Keymap::default_keymap();
    FORGE_CHECK_EQ(keymap.size(), static_cast<std::size_t>(12));

    const auto undo =
        keymap.resolve(forge::Shortcut(forge::Key::Z, forge::ModifierKey::Ctrl));
    FORGE_REQUIRE(undo.has_value());
    FORGE_CHECK_EQ(*undo, std::string("edit.undo"));

    const auto move = keymap.resolve(forge::Shortcut(forge::Key::G));
    FORGE_REQUIRE(move.has_value());
    FORGE_CHECK_EQ(*move, std::string("transform.move"));

    const auto shortcut = keymap.shortcut_for("edit.redo");
    FORGE_REQUIRE(shortcut.has_value());
    FORGE_CHECK_EQ(shortcut->to_string(), std::string("Ctrl+Shift+Z"));
}
