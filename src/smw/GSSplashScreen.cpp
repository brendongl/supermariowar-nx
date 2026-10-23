#include "GSSplashScreen.h"

#include "FileList.h"
#include "MapList.h"
#include "Game.h"
#include "GameValues.h"
#include "GSMenu.h"
#include "GSGameplay.h"
#include "eyecandy.h"
#include "path.h"
#include "ResourceManager.h"
#include "ui/ModernMenu.h"

#include <SDL3_image/SDL_image.h>

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern CEyecandyContainer eyecandy[3];

extern MapList *maplist;
extern SkinList *skinlist;
extern MusicList *musiclist;
extern GraphicsList *menugraphicspacklist;

extern CGameValues game_values;
extern CResourceManager* rm;

//-----------------------------------------------------------------------------
// THE LOAD UP SEQUENCE + SPLASH SCREEN
//-----------------------------------------------------------------------------
//that's a bunch of ugly code, maybe i'll throw it out again

SplashScreenState::SplashScreenState()
{
    alpha = 255;
    state = 7;
    timer = 120;
    firstFrame = true;
}

SplashScreenState& SplashScreenState::instance() {
    static SplashScreenState ss;
    return ss;
}

void SplashScreenState::onLeaveState()
{
    delete menu_credits;
}

bool SplashScreenState::init()
{
    App::screenWidth = App::maxScreenWidth;
    rm->loadStartGraphics();

//  gfx_loadimagenocolorkey(&rm->menu_dpi_logo, convertPath("gfx/packs/menu/splash_72dpi.png", menugraphicspacklist->current_name()));
//  gfx_loadimagenocolorkey(&rm->menu_contest_winners, convertPath("gfx/packs/menu/splash_contest_winners.png", menugraphicspacklist->current_name()));

    menu_credits = new gfxSprite();
    *menu_credits = ImageLoader(convertPath("gfx/packs/menu/splash_credits.png", menugraphicspacklist->currentPath())).create();

//	const char * contributors[] = {
//	"no_shorty", "redfalcon", "no_human", "dschingis", "funvill",
//	"matsche", "aeroflare", "Tymoe", "David Olofson", "scoti",
//	"affeOHNEwaffe", "mademan", "mario piuk", "yvoo", "DNightmare",
//	"Armen", "vvill", "zio tiok", "Donny Viszneki","alex-rus07", "JacobTheHero",
//	"Stephan Peijnik", "ventuz", "Stefan Schury", "riahc3",
//	"jinkies.uk", "devguy", "Alexis Morrissette", "Bidbood", "Quinn Storm",
//	"Flexserve", "Xijar", "j rok", "nitsuja", "4matsy", "leftyfb",
//	"Brian Porter", "Brandon Stansbury", "Brian Smith", "Caesar Cypher",
//	"Chris Schager", "Alex Arnot", "Jason Lumbert", "Kevin King",
//	"Fernando Marquez", "DrJones", "SleepyP", "UltimateNinja9",
//	"Alex Brown", "GKi", "mpinger", "neonater", "NMcCoy", "Rogultgot", "Arima",
//	"Toadeeboy", "Crapcom", "Tonberry2k", "Jum2004", "Bacon", "Tetra Vega",
//	"Young Link", "Blordow", "Gozinzolo", "Ragey", "SaturnEchidna", "Link901",
//	"Techokami", "dolorous", "SMW Fan", "Timonator", "YoshiMonarch7", "Tiptup300",
//	"Dude", "Justinio", "Matthew Callis", "VenomousNinja",
//	"Discrosh", "Ikill4you", "Helix Snake", "tubesteak", "wasabimario", "Lee",
//	"Pikawil", "Marioman64", "Peardian", "Bob Ippolito", "Viper Snake", "neavea",
//	"Mr.Bob-omb", "milua", "bobmanperson", "DrTek", "somestrangeflea", "nes6502",
//	"XPort", "Naphistim", "Chaos", "NiGHTS", "Kutter", "Maximilian",
//	"Felix the Ghost", "Water Kirby"};
//
//#define NUM_CONTRIBUTORS ((int)(sizeof(contributors)/sizeof(char*)))
//
//	bool contributorUsed[NUM_CONTRIBUTORS];
//	int contributorOrder[NUM_CONTRIBUTORS];
//
//	for (int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		contributorUsed[k] = false;
//	}
//
//	for (int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		int index = RANDOM_INT(NUM_CONTRIBUTORS);
//
//		while (contributorUsed[index])
//		{
//			if (++index >= NUM_CONTRIBUTORS)
//				index = 0;
//		}
//
//		contributorUsed[index] = true;
//		contributorOrder[k] = index;
//	}
//

    return true;
}

void SplashScreenState::update()
{
    game_values.playerInput.ClearPressedKeys(1);

    // TODO: move this out of this method maybe

    while (SDL_PollEvent(&loop_event)) {
        switch (loop_event.type) {

        case SDL_EVENT_QUIT: {
            game_values.appstate = AppState::Quit;
            return;
        }
        break;

        case SDL_EVENT_KEY_DOWN: {
            switch (loop_event.key.key) {
            case SDLK_RETURN:
                if (loop_event.key.mod & (SDL_KMOD_LALT | SDL_KMOD_RALT)) {
                    game_values.fullscreen = !game_values.fullscreen;
                    gfx_changefullscreen(game_values.fullscreen);
                    blitdest = screen;
                }
                break;

            case SDLK_F4:
                if (loop_event.key.mod & (SDL_KMOD_LALT | SDL_KMOD_RALT)) {
                    game_values.appstate = AppState::Quit;
                    return;
                }
                break;

            case SDLK_INSERT:
                gfx_take_screenshot();
                break;

            default:
                break;
            }
            break;
        }

        default:
            break;
        }

        game_values.playerInput.Update(loop_event, 1);
    }

#if defined(SMW_AUTOTEST)
    static const bool autotest = FileExists(GetHomeDirectory() + "autotest");
    static bool welcomeCaptureDelay = false;
    const bool captureWelcome = SDL_getenv("SMW_CAPTURE_DIR") != nullptr;
    if (autotest && (!captureWelcome || welcomeCaptureDelay))
        game_values.playerInput.outputControls[0].menu_select.fPressed = true;
    if (autotest && captureWelcome && state == 8)
        welcomeCaptureDelay = true;
#endif

    // Continuing before loading finished (state 8) would enter the menus without graphics
    for (int iPlayer = 0; iPlayer < 4 && state == 8; iPlayer++) {
        if (game_values.playerInput.outputControls[iPlayer].menu_select.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_cancel.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_random.fPressed) {
            //if (state <= 6)
            //{
            //  state = 6;
            //  alpha = 255;
            //}
            //else
            {
                blitdest = rm->menu_backdrop.getSurface();
                rm->menu_shade.setalpha(App::menuTransparency);
                rm->menu_shade.draw(0, 0);
                blitdest = screen;

                eyecandy[2].clean();

                game_values.playerInput.ResetKeys();
                game_values.appstate = AppState::Menu;

                MenuState::instance().init();
                GameplayState::instance().init();
                GameStateManager::instance().changeStateTo(&MenuState::instance());
                return;
            }
        }
    }

    //if (state == 0 || state == 3)
    //{
    //  alpha += 4;
    //  if (alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}
    //else if (state == 1 || state == 4)
    //{
    //  if (--timer <= 0)
    //  {
    //      timer = 120;
    //      state++;
    //  }
    //}
    //else if (state == 2 || state == 5)
    //{
    //  alpha -= 4;
    //  if (alpha <= 0)
    //  {
    //      alpha = 0;
    //      state++;
    //  }
    //}
    //else if (state == 6)
    //{
    //  alpha += 5;
    //  if (alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}

    ModernMenu::backdrop();

    //if (state == 0 || state == 1 || state == 2)
    //{
    //  menu_dpi_logo.setalpha((Uint8)alpha);
    //  menu_dpi_logo.draw(195, 186);
    //}
    //else if (state == 3 || state == 4 || state == 5)
    //{
    //  menu_contest_winners.setalpha((Uint8)alpha);
    //  menu_contest_winners.draw(0, 0);
    //}
    //else
    if (state == 6 || state == 7 || state == 8) {
        rm->menu_smw.setalpha((Uint8)alpha);
        rm->menu_smw.draw(App::screenWidth/2 - ((short)rm->menu_smw.getWidth() >> 1), 30);  //smw logo

        rm->menu_version.setalpha((Uint8)alpha);
        rm->menu_version.draw(App::screenWidth/2 + 116, 148);

        rm->menu_font_large.setAlpha((Uint8)alpha);
        //rm->menu_font_large.drawRightJustified(App::screenWidth * 0.98f, 45, "WIP");

        if (state == 8) {
            ModernMenu::panel(App::screenWidth / 2 - 165, 262, 330, 44);
            rm->menu_font_large.drawCentered(App::screenWidth / 2, 272, "PRESS ANY BUTTON");
            rm->menu_font_small.drawCentered(App::screenWidth / 2, 316,
                "Up to 8 players - Joy-Con, Pro Controller & keyboard");

            for (short player = 0; player < MAX_PLAYERS; ++player) {
                const int x = App::screenWidth / 2 - 216 + player * 58;
                const SDL_Rect src{0, 0, 32, 32};
                const SDL_Rect dst{x, 402, 48, 48};
                rm->spr_player[player][0].drawStretch(src, blitdest, dst);
            }

            ModernMenu::fill(0, 444, App::screenWidth, 36, 14, 18, 42);
            ModernMenu::fill(0, 444, App::screenWidth, 2, 72, 84, 140);
            rm->menu_font_small.draw(12, 450, "A tribute to Nintendo & Samuele Poletto");
            rm->menu_font_small.draw(12, 465, "Created by Florian Hufsky, Two52, Steven Graff & Jared");
            rm->menu_font_small.drawRightJustified(App::screenWidth - 12, 457, "sipnplay.cafe");
        }
    }

    if (state == 7) {
//            _load_drawmsg("Loading...");
        rm->menu_font_large.drawCentered(App::screenWidth/2, App::screenHeight * 0.875f, "Loading...");
    } else if (state == 8) {
        eyecandy[2].cleanDeadObjects();
        eyecandy[2].update();
        eyecandy[2].draw();

#if defined(SMW_AUTOTEST)
        static bool capturedWelcome = false;
        if (!capturedWelcome) {
            if (const char* dir = SDL_getenv("SMW_CAPTURE_DIR")) {
                const std::string path = std::string(dir) + "/menu-welcome.png";
                capturedWelcome = IMG_SavePNG(screen, path.c_str());
            }
        }
#endif

        /*          static int timer = 60;
                    static int index = 0;
                    if (++timer >= 60)
                    {
                        eyecandy[2].emplace<EC_GravText>(&rm->menu_font_large, App::screenWidth/2, App::screenHeight, contributors[contributorOrder[index]], -8.2f);
                        timer = 0;

                        if (++index >= NUM_CONTRIBUTORS)
                            index = 0;
                    } */
    }

    // Only start loading after we displayed something, in the 2nd frame
    if (firstFrame) {
        firstFrame = false;
        return;
    }

    if (state == 7) {
        // load initial coin sound
        rm->backgroundmusic[2] = sfxMusic(musiclist->music(1));

        rm->loadAllGraphics();
        rm->LoadGameSounds();

        for (short player = 0; player < MAX_PLAYERS; ++player) {
            if (game_values.skinids[player] < 0 || game_values.skinids[player] >= skinlist->count())
                game_values.skinids[player] = player % skinlist->count();
            rm->LoadMenuSkin(player, game_values.skinids[player], player, false);
        }

        if (!game_values.soundcapable) {
            game_values.sound = false;
            game_values.music = false;
            game_values.soundvolume = 0;
            game_values.musicvolume = 0;
        }

        //Read the map filter lists
        maplist->ReadFilters();
        maplist->ApplyFilters(game_values.pfFilters);

        ifSoundOnPlay(rm->sfx_coin);


        state++;
    }
}
