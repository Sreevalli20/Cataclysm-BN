#include "catch/catch.hpp"
#include "item.h"
#include "map_helpers.h"
#include "player_helpers.h"
#include "state_helpers.h"
#include "type_id.h"

#include <string>

TEST_CASE("suppressed_subsonic_noise", "[ranged][noise]") {
    clear_all_state();
    standard_npc shooter;

    SECTION("quiet .22 with suppressor should produce plink") {
        // .22 ratshot: loudness 140, speed 300 (subsonic)
        // Marlin 39A: loudness -4
        // Suppressor: loudness_modifier -45
        // Total: 140 + (-4) + (-45) = 91
        // After -15 penalty: 91 - 15 = 76
        // Since 91 < 95, cap to 45 → "plink!"
        item gun("marlin_9a");
        item ammo("22_ratshot");
        item suppressor("suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume < 50);
        CHECK(sound_data.sound == "plink!");
    }

    SECTION("quiet .22 with crafted suppressor should produce bang") {
        // .22 ratshot: loudness 140, speed 300 (subsonic)
        // Marlin 39A: loudness -4
        // Crafted suppressor: loudness_modifier -30
        // Total: 140 + (-4) + (-30) = 106
        // Since 106 >= 95, no cap, just -15 penalty: 106 - 15 = 91 → "bang!"
        item gun("marlin_9a");
        item ammo("22_ratshot");
        item suppressor("crafted_suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume >= 50);
        CHECK(sound_data.volume < 120);
        CHECK(sound_data.sound == "bang!");
    }

    SECTION("supersonic .22 with suppressor should produce bang") {
        // .22 LR: loudness 142, speed 370 (supersonic)
        // Marlin 39A: loudness -4
        // Suppressor: loudness_modifier -45
        // Speed > 344, so suppressed but supersonic path
        // Total: 142 + (-4) + (-45) = 93
        // Cap to 120 → 93 → "bang!"
        item gun("marlin_9a");
        item ammo("22_lr");
        item suppressor("suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume >= 50);
        CHECK(sound_data.volume < 120);
        CHECK(sound_data.sound == "bang!");
    }

    SECTION("9mm with suppressor should produce bang") {
        // 9mm: loudness 159, speed 350 (subsonic)
        // Glock 17: loudness 0
        // Suppressor: loudness_modifier -45
        // Total: 159 + 0 + (-45) = 114
        // Since 114 >= 95, no cap, just -15 penalty: 114 - 15 = 99 → "bang!"
        item gun("glock_17");
        item ammo("9mm");
        item suppressor("suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume >= 50);
        CHECK(sound_data.volume < 120);
        CHECK(sound_data.sound == "bang!");
    }

    SECTION("bottle suppressor should not trigger suppression logic") {
        // .22 ratshot: loudness 140, speed 300 (subsonic)
        // Marlin 39A: loudness -4
        // Bottle suppressor: loudness_modifier -15 (not < -20, so not suppressed)
        // Total: 140 + (-4) + (-15) = 121 → "bang!"
        item gun("marlin_9a");
        item ammo("22_ratshot");
        item suppressor("bottle_suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume >= 50);
        CHECK(sound_data.sound == "bang!");
    }

    SECTION(".300 BLK subsonic with suppressor should produce bang") {
        // .300 BLK subsonic: loudness 150, speed 310 (subsonic)
        // AR-15: loudness -4
        // Suppressor: loudness_modifier -45
        // Total: 150 + (-4) + (-45) = 101
        // Since 101 >= 95, no cap, just -15 penalty: 101 - 15 = 86 → "bang!"
        item gun("ar_15");
        item ammo("300blk_ss");
        item suppressor("suppressor");
        gun.put_in(ammo, item_pocket::pocket_id::MOD);
        gun.put_in(suppressor, item_pocket::pocket_id::MOD);

        auto sound_data = gun.gun_noise(false);
        CHECK(sound_data.volume >= 50);
        CHECK(sound_data.volume < 120);
        CHECK(sound_data.sound == "bang!");
    }
}
