#include "modding.h"
#include "global.h"
#include "z64actor.h"
#include "recomputils.h"
#include "globalobjects_api.h"

#define TORCH_PRINT_INTERVAL 60
bool SwapDayNightTorches = true; // Bool toggle for night torch activation for other mods if needed (currently changed by rando)

typedef struct {
    s16 sceneId;
    Vec3f pos;
    s16 rotX;
    s16 rotY;
} TorchSpawn;

int RandoLoaded;              

RECOMP_HOOK("Play_Init") // Rando Optional Dependency (eventually use torch sanity stuff) (I'll probably forget)
void randocheck() {

    RandoLoaded = recomp_is_dependency_met("mm_recomp_rando") == DEPENDENCY_STATUS_FOUND;

    if (RandoLoaded) {

        SwapDayNightTorches = false;

    }

}


static s16 LitTorchExceptionScenes[] = {

    SCENE_KAKUSIANA, // Grottos

}; // This is for indoor areas or temples.

#define LIT_EXCEPTION_COUNT (sizeof(LitTorchExceptionScenes) / sizeof(s16))

static bool IsLitTorchException(s16 sceneId) {
    for (size_t i = 0; i < LIT_EXCEPTION_COUNT; i++) {
        if (LitTorchExceptionScenes[i] == sceneId) {
            return true;
        }
    }
    return false;
}

static TorchSpawn sTorchSpawns[] = {

    // South Clock Town (7 Torches)

    { SCENE_CLOCKTOWER, { -612.0f, 0.0f, 569.0f }, 0, 0 },              // Bottom Left Near Laundry Pool
    { SCENE_CLOCKTOWER, { -419.0f, 0.0f, -766.0f }, 0, 0 },             // Next to Clock Tower Entrance
    { SCENE_CLOCKTOWER, { 173.0f, 200.0f, -1376.0f }, 0, 0 },           // Top Right of Map Above Mail Box
    { SCENE_CLOCKTOWER, { -773.0f, 100.0f, -1369.0f }, 0, 0 },          // Corner Next to Owl Statue
    { SCENE_CLOCKTOWER, { 175.0f, 0.0f, 7.0f }, 0, 0 },                 // Near Soldier Sign
    { SCENE_CLOCKTOWER, { 175.0f, 0.0f, 575.0f }, 0, 0 },               // Near 20 Rupee Chest
    { SCENE_CLOCKTOWER, { 170.0f, 100.0f, -631.0f }, 0, 0 },            // Right Corner Next To Clock Tower Near Mailbox

    // North Clock Town (6 Torches)

    { SCENE_BACKTOWN, { -758.0f, 200.0f, -2375.0f }, 0, 0 },            // Next to Deku Flower
    { SCENE_BACKTOWN, { -830.0f, 292.0f, -1634.0f }, 0, 0 },            // Next to Grotto
    { SCENE_BACKTOWN, { -352.0f, 204.0f, -2385.0f }, 0, 0 },            // Right Side Near Guard
    { SCENE_BACKTOWN, { -106.0f, 240.0f, -2576.0f }, 0, 0 },            // Playground Top Left
    { SCENE_BACKTOWN, { 484.0f, 240.0f, -2577.0f }, 0, 0 },             // Playground Top Right
    { SCENE_BACKTOWN, { 494.0f, 200.0f, -1886.0f }, 0, 0 },             // Playground Near East Clock Town

    // East Clock Town (8 Torches)

    { SCENE_TOWN, { 606.0f, 100.0f, -826.0f }, 0, 0 },                  // In Front of Stock Pot In
    { SCENE_TOWN, { 1251.0f, 100.0f, -886.0f }, 0, 0 },                 // Next to Termina Field And Milk Bar Left
    { SCENE_TOWN, { 1367.0f, 100.0f, -474.0f }, 0, 0 },                 // Next to Honey and Darling And Termina Field in Corner
    { SCENE_TOWN, { 1179.0f, 100.0f, -15.0f }, 0, 0 },                  // Next to Mail Box
    { SCENE_TOWN, { 231.0f, 120.0f, -183.0f }, 0, 0 },                  // Next to Chest Mini Game & Deku Flower
    { SCENE_TOWN, { 780.0f, 200.0f, -1376.0f }, 0, 0 },                 // End of Stairs in Left Corner
    { SCENE_TOWN, { 1375.0f, 200.0f, -1973.0f }, 0, 0 },                // Near Mayor Office Top Right Corner
    { SCENE_TOWN, { 1079.0f, 372.0f, -949.0f }, 0, 0 },                 // Above Milk bar

    // West Clock Town (3 Torches)

    { SCENE_ICHIBA, { -1025.0f, 0.0f, 8.0f }, 0, 0 },                   // Corner Left When Coming From South Clock Town
    { SCENE_ICHIBA, { -2015.0f, 200.0f, -1373.0f }, 0, 0 },             // Corner Near Swordman School
    { SCENE_ICHIBA, { -1397.0f, 15.0f, -12.0f }, 0, 29125 },            // Curiosity Shop

    // Laundry Pool (2 Torches)

    { SCENE_ALLEY, { -1679.0f, -120.0f, 382.0f }, 0, 0 },               // Near Exit
    { SCENE_ALLEY, { -1948.0f, -120.0f, 359.0f }, 0, 0 },               // Near River

    // Termina Field (13 Torches)

    { SCENE_00KEIKOKU, { -527.0f, -77.0f, 1665.0f }, 0, 0 },            // SCT Left
    { SCENE_00KEIKOKU, { -271.0f, -77.0f, 1665.0f }, 0, 0 },            // SCT Right
    { SCENE_00KEIKOKU, { 1859.0f, 68.0f, -266.0f }, 0, 0 },             // ECT Left
    { SCENE_00KEIKOKU, { 1859.0f, 68.0f, -539.0f }, 0, 0 },             // ECT Right
    { SCENE_00KEIKOKU, { -178.0f, 48.0f, -2418.0f }, 0, 0 },            // NCT Left
    { SCENE_00KEIKOKU, { -622.0f, 48.0f, -2418.0f }, 0, 0 },            // NCT Right
    { SCENE_00KEIKOKU, { -2417.0f, 48.0f, -626.0f }, 0, 0 },            // WCT Left
    { SCENE_00KEIKOKU, { -2417.0f, 48.0f, -187.0f }, 0, 0 },            // WCT Right
    { SCENE_00KEIKOKU, { 4505.0f, 254.0f, 1095.0f }, 0, 0 },            // Observatory
    { SCENE_00KEIKOKU, { -3328.0f, -222.0f, 4188.0f }, 0, -13466 },     // Milk Road Right
    { SCENE_00KEIKOKU, { -3236.0f, -222.0f, 4565.0f }, 0, -12129 },     // Milk Road Left
    { SCENE_00KEIKOKU, { -3644.0f, 48.0f, -147.0f }, 0, 0 },            // Fountain Left
    { SCENE_00KEIKOKU, { -3644.0f, 48.0f, -663.0f }, 0, 0 },            // Fountain Right

    // Milk Road (2 Torches)

    { SCENE_ROMANYMAE, { -3486.0f, 0.0f, 1055.0f }, 0, 0 },             // Close to Termina Field
    { SCENE_ROMANYMAE, { -5387.0f, 68.0f, 1623.0f }, 0, 0 },            // Close to Romani Ranch

    // Gorman Track (4 Torches)

    { SCENE_KOEPONARACE, { -2111.0f, -106.0f, -596.0f }, 0, 0 },        // Close to Exit
    { SCENE_KOEPONARACE, { -1685.0f, -106.0f, -424.0f }, 0, 0 },        // Next to Fence
    { SCENE_KOEPONARACE, { -1731.0f, -116.0f, 3912.0f }, 0, 0 },        // Other Exit Left  (facing towards track)
    { SCENE_KOEPONARACE, { -1390.0f, -116.0f, 4550.0f }, 0, 0 },        // Other Exit Right (facing towards track)

    // Romani Ranch (doing this while "they" are attacking) (8 Torches)

    { SCENE_F01, { 2463.0f, 12.0f, 384.0f }, 0, -21295 },               // Entrance Sign Left
    { SCENE_F01, { 2364.0f, 9.0f, 672.0f }, 0, -19573 },                // Entrance Sign Right
    { SCENE_F01, { -638.0f, 264.0f, -1657.0f }, 0, -3481 },             // Next To Barn
    { SCENE_F01, { -1309.0f, 255.0f, -1182.0f }, 0, -13640 },           // Next To Epona
    { SCENE_F01, { -3412.0f, 172.0f, 15.0f }, 0, 13352 },               // Shack Left
    { SCENE_F01, { -3339.0f, 172.0f, -219.0f }, 0, 13352 },             // Shack Right
    { SCENE_F01, { -3249.0f, 167.0f, 731.0f }, 0, 22980 },              // DRT Right
    { SCENE_F01, { -3179.0f, 167.0f, 819.0f }, 0, 22980 },              // DRT Left

    // Doggy Race Track (4 Torches)

    { SCENE_F01_B, { -3554.0f, 167.0f, 714.0f }, 0, -9022 },            // Entrance Left  (facing door)
    { SCENE_F01_B, { -3260.0f, 167.0f, 1133.0f }, 0, -10068 },          // Entrance Right (facing door)
    { SCENE_F01_B, { -4619.0f, 145.0f, 1869.0f }, 0, -10184 },          // Dog Roof Pole Right
    { SCENE_F01_B, { -4445.0f, 147.0f, 2114.0f }, 0, -9446 },           // Dog Roof Pole Left

    // Cucco Shack (4 Torches)

    { SCENE_F01C, { -3656.0f, 132.0f, -1243.0f }, 0, 1559 },            // Near Gossip Stone
    { SCENE_F01C, { -4095.0f, 132.0f, -113.0f }, 0, -20736 },           // Bottom Left Corner Near Horrible Seam
    { SCENE_F01C, { -5462.0f, 132.0f, -954.0f }, 0, 28377 },            // Top Left  
    { SCENE_F01C, { 4814.0f, 132.0f, -1504.0f }, 0, 12843 },            // Right Side

    // Bean Seller / Grottos (5 Torches)

    { SCENE_KAKUSIANA, { 4372.0f, 0.0f, 1131.0f }, 0, -17255 },         // Pond Right
    { SCENE_KAKUSIANA, { 4126.0f, 0.0f, 877.0f }, 0, 2844 },            // Pond Left
    { SCENE_KAKUSIANA, { 4071.0f, 0.0f, 1372.0f }, 0, 18216 },          // Left Side Of Area To Pond
    { SCENE_KAKUSIANA, { 4247.0f, 0.0f, 1800.0f }, 0, -30015 },         // Behind Light
    { SCENE_KAKUSIANA, { 4356.0f, 0.0f, 1477.0f }, 0, -12210 },         // Right Side Of Area To Pond

    // Road to Southern Swamp (8 Torches)

    { SCENE_24KEMONOMITI, { 427.0f, -182.0f, 939.0f }, 0, -11879 },     // Close to Termina Field
    { SCENE_24KEMONOMITI, { -213.0f, -182.0f, 1972.0f }, 0, 16139 },    // Right Side
    { SCENE_24KEMONOMITI, { 1581.0f, -182.0f, 2117.0f }, 0, -5699 },    // Path to Shooting Gallery Left
    { SCENE_24KEMONOMITI, { 2447.0f, -182.0f, 2758.0f }, 0, -6421 },    // Path To Shooting Gallery Right
    { SCENE_24KEMONOMITI, { 3101.0f, -182.0f, 3032.0f }, 0, -25083 },   // Shooting Gallery Left
    { SCENE_24KEMONOMITI, { 2598.0f, -182.0f, 3347.0f }, 0, -32183 },   // Shooting Gallery Right
    { SCENE_24KEMONOMITI, { 724.0f, -237.0f, 2968.0f }, 0, -22269 },    // Tingle
    { SCENE_24KEMONOMITI, { 400.0f, -237.0f, 3696.0f }, 0, 13362 },     // Next to Southern Swamp

    // Southern Swamp Poisoned (12 Torches)

    { SCENE_20SICHITAI, { -832.0f, 36.0f, -874.0f }, 0, 8476 },         // Entrance Left
    { SCENE_20SICHITAI, { 521.0f, 58.4f, -939.0f }, 0, -10440 },        // Entrance Right
    { SCENE_20SICHITAI, { 41.0f, 20.0f, -117.0f }, 0, -32340 },         // Dock Left
    { SCENE_20SICHITAI, { -122.0f, 20.0f, -111.0f }, 0, -32340 },       // Dock Right
    { SCENE_20SICHITAI, { -311.0f, 170.0f, 33.0f }, 0, 16387 },         // Tourist Center Across Scrub
    { SCENE_20SICHITAI, { 2089.0f, 22.0f, 244.0f }, 0, -32768 },        // Before Path to Potion Shop Right
    { SCENE_20SICHITAI, { 3467.0f, 15.0f, -813.0f }, 0, 25225 },        // Left Side Ladder
    { SCENE_20SICHITAI, { 3807.0f, 15.0f, -746.0f }, 0, 3236 },         // Right Side Ladder
    { SCENE_20SICHITAI, { 3982.0f, 23.0f, -1722.0f }, 0, -5835 },       // Next to Woods of Mystery
    { SCENE_20SICHITAI, { -767.0f, 36.0f, 4143.0f }, 0, 24983 },        // Deku Palace Entrance Left
    { SCENE_20SICHITAI, { -930.0f, 35.8f, 4001.0f }, 0, 24983 },        // Deku Palace Entrance Right
    { SCENE_20SICHITAI, { 311.0f, 393.0f, 3241.0f }, 0, -6788 },        // Entrance to Woodfall

    // Southern Swamp Clear (12 Torches)

    { SCENE_20SICHITAI2, { -832.0f, 36.0f, -874.0f }, 0, 8476 },         // Entrance Left
    { SCENE_20SICHITAI2, { 521.0f, 58.4f, -939.0f }, 0, -10440 },        // Entrance Right
    { SCENE_20SICHITAI2, { 41.0f, 20.0f, -117.0f }, 0, -32340 },         // Dock Left
    { SCENE_20SICHITAI2, { -122.0f, 20.0f, -111.0f }, 0, -32340 },       // Dock Right
    { SCENE_20SICHITAI2, { -311.0f, 170.0f, 33.0f }, 0, 16387 },         // Tourist Center Across Scrub
    { SCENE_20SICHITAI2, { 2089.0f, 22.0f, 244.0f }, 0, -32768 },        // Before Path to Potion Shop Right
    { SCENE_20SICHITAI2, { 3467.0f, 15.0f, -813.0f }, 0, 25225 },        // Left Side Ladder
    { SCENE_20SICHITAI2, { 3807.0f, 15.0f, -746.0f }, 0, 3236 },         // Right Side Ladder
    { SCENE_20SICHITAI2, { 3982.0f, 23.0f, -1722.0f }, 0, -5835 },       // Next to Woods of Mystery
    { SCENE_20SICHITAI2, { -767.0f, 36.0f, 4143.0f }, 0, 24983 },        // Deku Palace Entrance Left
    { SCENE_20SICHITAI2, { -930.0f, 35.8f, 4001.0f }, 0, 24983 },        // Deku Palace Entrance Right
    { SCENE_20SICHITAI2, { 311.0f, 393.0f, 3241.0f }, 0, -6788 },        // Entrance to Woodfall

    // Woods of Mystery (9 Torches)

    { SCENE_26SARUNOMORI, { -79.0f, 11.0f, 146.0f }, 0, 0 },            // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { 112.0f, 0.0f, -1021.0f }, 0, -6744 },       // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -1044.0f, 0.0f, -715.0f }, 0, 25119 },      // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -1603.0f, 0.0f, -722.0f }, 0, -25510 },     // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -1917.0f, 0.0f, -156.0f }, 0, 8230 },       // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -807.0f, 11.0f, -171.0f }, 0, 0 },          // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -718.0f, 0.0f, 1038.0f }, 0, -24912 },      // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -79.0f, 11.0f, 1025.0f }, 0, 0 },           // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order
    { SCENE_26SARUNOMORI, { -1685.0f, 11.0f, 709.0f }, 0, 0 },          // I took Day 2 path then Day 1 path then Day 3 path so they should be in that order

    // Deku Palace (8 Torches)

    { SCENE_22DEKUCITY, { 105.0f, 0.0f, 3214.0f }, 0, 0 },              // Entrance Left  
    { SCENE_22DEKUCITY, { -105.0f, 0.0f, 3214.0f }, 0, 0 },             // Entrance Right
    { SCENE_22DEKUCITY, { -92.0f, 0.0f, 2028.0f }, 0, 0 },              // Path Right (facing chambers)
    { SCENE_22DEKUCITY, { 92.0f, 0.0f, 2028.0f }, 0, 0 },               // Path Left (facing chambers)
    { SCENE_22DEKUCITY, { -4.0f, 100.0f, 1395.0f }, 0, 0 },             // Above Chambers Door
    { SCENE_22DEKUCITY, { 1620.0f, 0.0f, 666.0f }, 0, 0 },              // Near Bean Hole Left
    { SCENE_22DEKUCITY, { 1943.0f, 0.0f, 1247.0f }, 0, 0 },             // Near Bean Hole Near Water
    { SCENE_22DEKUCITY, { 599.0f, 25.0f, 2855.0f }, 0, -27774 },        // Near Deku Flower Path to Swamp Flowers

    // Woodfall (3 Torches)

    { SCENE_21MITURINMAE, { 31.0f, 12.0f, -1405.0f }, 0, 0 },           // Next to Entrance
    { SCENE_21MITURINMAE, { 792.0f, 15.0f, 1602.0f }, 0, -26300 },      // Near GF
    { SCENE_21MITURINMAE, { 1298.0f, 120.0f, -1013.0f }, 0, 29493 },    // Near HP Chest

    // Path to Mountain Village (4 Torches)

    { SCENE_14YUKIDAMANOMITI, { 1334.0f, -924.0f, 6969.0f }, 0, 16661 },        // Near Termina Field Left Torch
    { SCENE_14YUKIDAMANOMITI, { 2211.0f, -904.0f, 6766.0f }, 0, -18826 },       // Near Termina Field Right Torch
    { SCENE_14YUKIDAMANOMITI, { 125.0f, -659.0f, 5856.0f }, 0, 13366 },         // Midway Through
    { SCENE_14YUKIDAMANOMITI, { 686.0f, -400.0f, 4877.0f }, 0, -27600 },        // Near Mountain Village Near Sign

    // Great Bay Coast (16 Torches) (Rip Almighty Torch)

    { SCENE_30GYOSON, { 3028.0f, 80.0f, 4070.0f }, 0, 22978 },          // Closest to Termina Field
    { SCENE_30GYOSON, { 2519.0f, 80.0f, 3210.0f }, 0, 26349 },          // First Fence
    { SCENE_30GYOSON, { 2217.0f, 80.0f, 2795.0f }, 0, -2918 },          // Second Fence
    { SCENE_30GYOSON, { 1727.0f, 80.0f, 3002.0f }, 0, 28520 },          // Third Fence Next to Sign
    { SCENE_30GYOSON, { 1539.0f, 80.0f, 2470.0f }, 0, -6514 },          // Fourth Fence 
    { SCENE_30GYOSON, { 896.0f, 80.0f, 3863.0f }, 0, -25940 },          // Fence Left Before Hut & Spider House
    { SCENE_30GYOSON, { 635.0f, 80.0f, 4332.0f }, 0, -19513 },          // Fence Right Before Hut & Spider House
    { SCENE_30GYOSON, { 1443.0f, 92.0f, 4056.0f }, 0, -16093 },         // Spider House Left
    { SCENE_30GYOSON, { 1431.0f, 92.0f, 4277.0f }, 0, -16461 },         // Spider House Right
    { SCENE_30GYOSON, { 1002.0f, 80.0f, 4819.0f }, 0, -16746 },         // Side of Hut
    { SCENE_30GYOSON, { -7.0f, 80.0f, 3804.0f }, 0, 16528 },            // Beach Below Umbrella (very smart placement)
    { SCENE_30GYOSON, { -1154.0f, 15.0f, -2735.0f }, 0, -10122 },       // Near Fortress
    { SCENE_30GYOSON, { 1886.0f, 679.0f, -1683.0f }, 0, -11953 },       // Wall Near Scarecrow
    { SCENE_30GYOSON, { 52.0f, 80.0f, 4956.0f }, 0, 16086 },            // Uncentered Boats (I can never unsee it)
    { SCENE_30GYOSON, { -509.0f, 78.0f, 5956.0f }, 0, 25677 },          // Near Sign to Zora Cape
    { SCENE_30GYOSON, { -2888.0f, 15.0f, 4216.0f }, 0, -16102 },        // Owl Statue

    // Zora Cape (16 Torches)

    { SCENE_31MISAKI, { 466.0f, 12.0f, 410.0f }, 0, 9 },                // To Great Bay Coast Right
    { SCENE_31MISAKI, { -271.0f, 10.0f, 319.0f }, 0, 1110 },            // To Great Bay Coast Left
    { SCENE_31MISAKI, { 1063.0f, 12.0f, 656.0f }, 0, -26164 },          // Pot Mini Game Left
    { SCENE_31MISAKI, { 1399.0f, 12.0f, 1100.0f }, 0, 6769 },           // Pot Mini Game Right (end the nightmare please)
    { SCENE_31MISAKI, { 2655.0f, 12.0f, 1485.0f }, 0, -3862 },          // Path to Beaver Race Lower First
    { SCENE_31MISAKI, { 3684.0f, 11.0f, 1965.0f }, 0, -7325 },          // Path to Beaver Race Lower Second
    { SCENE_31MISAKI, { 4309.0f, 11.0f, 2777.0f }, 0, -25639 },         // Path to Beaver Race Lower Third
    { SCENE_31MISAKI, { 4156.0f, 40.0f, 3815.0f }, 0, -21865 },         // HP Dive Like Like Left
    { SCENE_31MISAKI, { 3667.0f, 40.0f, 4016.0f }, 0, 27689 },          // HP Dive Like Like Right
    { SCENE_31MISAKI, { 4271.0f, 1255.0f, 5014.0f }, 0, 27689 },        // Beaver Race Entrance Left
    { SCENE_31MISAKI, { 4065.0f, 1255.0f, 4921.0f }, 0, 28817 },        // Beaver Race Entrance Right
    { SCENE_31MISAKI, { 4016.0f, 780.0f, 2401.0f }, 0, -10759 },        // Path to Beaver Race Chest
    { SCENE_31MISAKI, { -1500.0f, 200.0f, 4359.0f }, 0, 31625 },        // Great Fairy Left
    { SCENE_31MISAKI, { -1793.0f, 200.0f, 4368.0f }, 0, -32303 },       // Great Fairy Right
    { SCENE_31MISAKI, { -5471.0f, 14.0f, 1149.0f }, 0, 0 },             // Turtle Left
    { SCENE_31MISAKI, { -5301.0f, 14.0f, 2064.0f }, 0, 17922 },         // Turtle Right

    // Pirate's Fortress Exterior (5 Torches)

    { SCENE_TORIDE, { -2140.0f, 135.0f, 1862.0f }, 0, 19466 },         // Entrance Right (facing entrance)
    { SCENE_TORIDE, { -2028.0f, 135.0f, 2257.0f }, 0, 19370 },         // Entrance Left
    { SCENE_TORIDE, { 6.0f, 200.0f, 1572.0f }, 0, -12843 },            // Opposite Side of Barrel
    { SCENE_TORIDE, { 290.0f, 200.0f, -686.0f }, 0, 0 },               // Opposite Side of Door (kicked out)
    { SCENE_TORIDE, { 330.0f, 400.0f, -2285.0f }, 0, -3368 },          // Above Hookshot Target Near Interior


    // Pirate's Fortress

    { SCENE_KAIZOKU, { 677.0f, -140.0f, -758.0f }, 0, 8140 },           // Next to Box Entrance
    { SCENE_KAIZOKU, { 1019.0f, -160.0f, 82.0f }, 0, 0 },               // Ladder Left
    { SCENE_KAIZOKU, { 1019.0f, -160.0f, 281.0f }, 0, 0 },              // Ladder Right
    { SCENE_KAIZOKU, { 2039.0f, 140.0f, 436.0f }, 0, 0 },               // Stairs Middle
    { SCENE_KAIZOKU, { 2356.0f, 280.0f, -189.0f }, 0, 0 },              // Hookshot Door Left
    { SCENE_KAIZOKU, { 2356.0f, 280.0f, -53.0f }, 0, 0 },               // Hookshot Door Right

};

#define TORCH_SPAWN_COUNT (sizeof(sTorchSpawns) / sizeof(TorchSpawn))
static bool TorchesSpawned = false;
static u16 LastTorchType = 0;

static uintptr_t sTorchObject;

// Global Object Stuff

GLOBAL_OBJECTS_CALLBACK_ON_READY void onGlobalObjectsReady() {
    sTorchObject = (uintptr_t)GlobalObjects_getGlobalObject(OBJECT_SYOKUDAI);
}

RECOMP_HOOK_RETURN("Actor_LoadOverlay") void on_return_Actor_LoadOverlay() {
    ActorProfile* profile = recomphook_get_return_ptr();
    if (profile != NULL && profile->id == ACTOR_OBJ_SYOKUDAI) {
        profile->objectId = GAMEPLAY_KEEP;
    }
}

RECOMP_HOOK("ObjSyokudai_Init")
void Torch_Init(Actor* thisx, PlayState* play) {
    gSegments[0x06] = OS_K0_TO_PHYSICAL(sTorchObject);
}

RECOMP_HOOK("ObjSyokudai_Update")
void Torch_Update(Actor* thisx, PlayState* play) {
    gSegments[0x06] = OS_K0_TO_PHYSICAL(sTorchObject);
}

RECOMP_HOOK("ObjSyokudai_Draw")
void Torch_Draw(Actor* thisx, PlayState* play) {
    gSegments[0x06] = OS_K0_TO_PHYSICAL(sTorchObject);

    OPEN_DISPS(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x06, sTorchObject);
    gSPSegment(POLY_XLU_DISP++, 0x06, sTorchObject);
    CLOSE_DISPS(play->state.gfxCtx);
}

// Spawning Torches

static void SpawnTorchesIfNeeded(PlayState* play) {
    u16 torchType = 8 << 10;

    if (SwapDayNightTorches) {
        if (gSaveContext.save.time >= CLOCK_TIME(18, 0) ||
            gSaveContext.save.time < CLOCK_TIME(6, 0)) {
            torchType = 10 << 10; // Night torch
        }
        else {
            torchType = 8 << 10;  // Day torch
        }
    }

    if (IsLitTorchException(play->sceneId)) {
        if (SwapDayNightTorches) {
            torchType = 10 << 10;
        }
        else {
            torchType = 8 << 10;
        }
    }

    bool foundTorch = false;
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;
    while (actor != NULL) {
        if (actor->id == ACTOR_OBJ_SYOKUDAI) {
            foundTorch = true;
            break;
        }
        actor = actor->next;
    }

    if (foundTorch && LastTorchType == torchType) {
        return;
    }

    if (foundTorch && LastTorchType != 0 && LastTorchType != torchType) {
        actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;
        while (actor != NULL) {
            Actor* next = actor->next;
            if (actor->id == ACTOR_OBJ_SYOKUDAI) {
                Actor_Kill(actor);
            }
            actor = next;
        }
    }

    for (size_t i = 0; i < TORCH_SPAWN_COUNT; i++) {
        if (sTorchSpawns[i].sceneId != play->sceneId)
            continue;

        Actor* spawned = Actor_Spawn(
            &play->actorCtx,
            play,
            ACTOR_OBJ_SYOKUDAI,
            sTorchSpawns[i].pos.x,
            sTorchSpawns[i].pos.y,
            sTorchSpawns[i].pos.z,
            sTorchSpawns[i].rotX,
            sTorchSpawns[i].rotY,
            0,
            torchType | (i & 0x3FF)
        );

        if (spawned != NULL) {
            recomp_printf(
                "[TORCH] Spawned %s torch #%d in scene %d at (%.1f, %.1f, %.1f)\n",
                (torchType == (10 << 10)) ? "NIGHT" : "DAY",
                (int)i,
                play->sceneId,
                sTorchSpawns[i].pos.x,
                sTorchSpawns[i].pos.y,
                sTorchSpawns[i].pos.z
            );
        }
        else {
            recomp_printf(
                "[TORCH] FAILED to spawn torch #%d in scene %d\n",
                (int)i,
                play->sceneId
            );
        }
    }

    LastTorchType = torchType;
}

// Debug Player Printing

static void PrintPlayerPosition(PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (!player) return;

    if ((play->gameplayFrames % TORCH_PRINT_INTERVAL) == 0) {
        recomp_printf(
            "[POS] Scene %d | X %.2f Y %.2f Z %.2f | RotY %d\n",
            play->sceneId,
            player->actor.world.pos.x,
            player->actor.world.pos.y,
            player->actor.world.pos.z,
            player->actor.world.rot.y
        );
    }
}

// Calling of the Torches and Prints and Stuff

RECOMP_HOOK("Play_Update")
void TorchSpawner_PlayUpdateHook(PlayState* play) {
    static s16 lastScene = -1;
    static s16 lastRoom = -1;
    static u16 lastDayTime = 0;

    if (lastScene != play->sceneId) {
        TorchesSpawned = false;
        LastTorchType = 0;
        lastScene = play->sceneId;
    }

    if (lastRoom != play->roomCtx.curRoom.num) {
        TorchesSpawned = false;
        LastTorchType = 0;
        lastRoom = play->roomCtx.curRoom.num;
    }

    if (gSaveContext.save.time < lastDayTime) {
        TorchesSpawned = false;
        LastTorchType = 0;
    }
    lastDayTime = gSaveContext.save.time;

    SpawnTorchesIfNeeded(play);
    PrintPlayerPosition(play);
}