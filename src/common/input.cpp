#include "input.h"

#include "GameValues.h"
#include "GlobalConstants.h"

extern CGameValues game_values;
extern SDL_Joystick** joysticks;
extern int joystickcount;

int joystickIndexFromID(SDL_JoystickID id)
{
    for (int i = 0; i < joystickcount; i++) {
        if (joysticks[i] && SDL_GetJoystickID(joysticks[i]) == id)
            return i;
    }
    return -2; // never matches DEVICE_KEYBOARD or a joystick index
}

SDL_Joystick* joystickFromIndex(int index)
{
    if (index < 0 || index >= joystickcount)
        return nullptr;
    return joysticks[index];
}

namespace {
#ifdef __SWITCH__
// Extra physical buttons that also trigger a bound button, so the d-pad and the left
// stick both steer, and either face button of a pair works. Indices follow SDL's
// Switch joystick: 0 A, 1 B, 2 X, 3 Y, 6 L, 7 R, 8 ZL/SL, 9 ZR/SR, 12-15 d-pad
// (left, up, right, down; the stick on a single Joy-Con), 16-19 left stick.
int switchAlias(int button, short iGameState)
{
    if (button >= 16 && button <= 19)
        return button - 4;
    if (iGameState == 0 && button >= 7 && button <= 9)
        return 6;
    if (iGameState == 0) {
        if (button == 1) return 0;
        if (button == 2) return 3;
    }
    return button;
}

bool isBoundButtonDown(SDL_Joystick* joystick, int bound, short iGameState)
{
    if (!joystick)
        return false;
    const int count = SDL_GetNumJoystickButtons(joystick);
    for (int b = 0; b < count; b++) {
        if ((b == bound || switchAlias(b, iGameState) == bound) && SDL_GetJoystickButton(joystick, b))
            return true;
    }
    return false;
}
#endif

bool joyButtonMatches(SDL_Keycode binding, int button, short iGameState)
{
    if (binding == SDL_Keycode(button + JOY_BUTTON_START))
        return true;
#ifdef __SWITCH__
    return binding == SDL_Keycode(switchAlias(button, iGameState) + JOY_BUTTON_START);
#else
    return false;
#endif
}
} // namespace

CPlayerInput::CPlayerInput()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

    iPressedKey = 0;
}

//Pass in 0 for game and 1 for menu
//Clear old button pushed states
void CPlayerInput::ClearPressedKeys(short iGameState)
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        COutputControl* outputControl = &outputControls[iPlayer];
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
            outputControl->keys[iKey].fPressed = false;
        }
        outputControl->menu_skin_prev.fPressed = false;
        outputControl->menu_skin_next.fPressed = false;
        outputControl->menu_color_prev.fPressed = false;
        outputControl->menu_color_next.fPressed = false;
    }

    iPressedKey = 0;
}

void CPlayerInput::ClearGameActionKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
		COutputControl * outputControl = &outputControls[iPlayer];

		// 0-3: direction keys, 4: turbo
        for (int iKey = 5; iKey < NUM_KEYS; iKey++) {
			outputControl->keys[iKey].fPressed = false;
			outputControl->keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Clear all button pushed and down states
//Call this when switching from menu to game
void CPlayerInput::ResetKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
        }
		outputControls[iPlayer].menu_skin_prev = {};
		outputControls[iPlayer].menu_skin_next = {};
		outputControls[iPlayer].menu_color_prev = {};
		outputControls[iPlayer].menu_color_next = {};
	}

	iPressedKey = 0;
}

//Called during game loop to read input events and see if
//configured keys were pressed.  If they were, then turn on
//key flags to be used by game logic
//iGameState == 0 for in game and 1 for menu
void CPlayerInput::Update(SDL_Event event, short iGameState)
{
	bool fFound = false;
    for (short iPlayer = -1; iPlayer < MAX_PLAYERS; iPlayer++) {
		CInputControl * inputControl;
		COutputControl * outputControl;
		short iDeviceID = DEVICE_KEYBOARD;

		//Allow keyboard input from player 1 at all times (even when he is configured to use joystick)
        if (iPlayer == -1) {
            if (iGameState == 1 && inputControls[0]->iDevice != DEVICE_KEYBOARD) {
				inputControl = &game_values.inputConfiguration[0][0].inputGameControls[1];
				outputControl = &outputControls[0];
				iDeviceID = game_values.inputConfiguration[0][0].iDevice;
            } else {
				continue;
			}
        } else {
			if (!inputControls[iPlayer])
				continue;

			inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];
			outputControl = &outputControls[iPlayer];
			iDeviceID = inputControls[iPlayer]->iDevice;
		}

        if (iDeviceID == DEVICE_KEYBOARD) {
            if (SDL_EVENT_KEY_DOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.key) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}

				iPressedKey = event.key.key;
            } else if (SDL_EVENT_KEY_UP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.key) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_EVENT_MOUSE_MOTION == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] >= MOUSE_UP) {
						if ((inputControl->keys[iKey] == MOUSE_UP && event.motion.yrel < -MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_DOWN && event.motion.yrel > MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_LEFT && event.motion.xrel < -MOUSE_X_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_RIGHT && event.motion.xrel > MOUSE_X_DEAD_ZONE) ||
                                (inputControl->keys[iKey] >= MOUSE_BUTTON_START && (event.motion.state & SDL_BUTTON_MASK(inputControl->keys[iKey] - MOUSE_BUTTON_START)))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
							if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_EVENT_MOUSE_BUTTON_DOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_EVENT_MOUSE_BUTTON_UP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
						if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
							continue;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
			}
        } else {
            if (SDL_EVENT_JOYSTICK_HAT_MOTION == event.type) {
				if (iDeviceID != joystickIndexFromID(event.jhat.which))
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
                    if (inputControl->keys[iKey] >= JOY_HAT_UP && inputControl->keys[iKey] <= JOY_HAT_RIGHT) {
						if ((inputControl->keys[iKey] == JOY_HAT_UP && (event.jhat.value & SDL_HAT_UP)) ||
							(inputControl->keys[iKey] == JOY_HAT_DOWN && (event.jhat.value & SDL_HAT_DOWN)) ||
							(inputControl->keys[iKey] == JOY_HAT_LEFT && (event.jhat.value & SDL_HAT_LEFT)) ||
                                (inputControl->keys[iKey] == JOY_HAT_RIGHT && (event.jhat.value & SDL_HAT_RIGHT))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_EVENT_JOYSTICK_BUTTON_DOWN == event.type) {
				if (iDeviceID != joystickIndexFromID(event.jbutton.which))
					continue;

#ifdef __SWITCH__
                if (iGameState == 1 && event.jbutton.button >= 6 && event.jbutton.button <= 9) {
                    CKeyState* shoulder = event.jbutton.button == 6 ? &outputControl->menu_skin_prev
                        : event.jbutton.button == 7 ? &outputControl->menu_skin_next
                        : event.jbutton.button == 8 ? &outputControl->menu_color_prev
                        : &outputControl->menu_color_next;
                    if (!shoulder->fDown) shoulder->fPressed = true;
                    shoulder->fDown = true;
                    continue;
                }
#endif

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (joyButtonMatches(inputControl->keys[iKey], event.jbutton.button, iGameState)) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_EVENT_JOYSTICK_BUTTON_UP == event.type) {
				if (iDeviceID != joystickIndexFromID(event.jbutton.which))
					continue;

#ifdef __SWITCH__
                if (iGameState == 1 && event.jbutton.button >= 6 && event.jbutton.button <= 9) {
                    CKeyState* shoulder = event.jbutton.button == 6 ? &outputControl->menu_skin_prev
                        : event.jbutton.button == 7 ? &outputControl->menu_skin_next
                        : event.jbutton.button == 8 ? &outputControl->menu_color_prev
                        : &outputControl->menu_color_next;
                    shoulder->fDown = false;
                    continue;
                }
#endif

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (joyButtonMatches(inputControl->keys[iKey], event.jbutton.button, iGameState)) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

#ifdef __SWITCH__
						// Stay down while another button bound to the same action is held
						if (isBoundButtonDown(joystickFromIndex(iDeviceID), inputControl->keys[iKey] - JOY_BUTTON_START, iGameState))
							continue;
#endif
						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_EVENT_JOYSTICK_AXIS_MOTION == event.type) {
				if (iDeviceID != joystickIndexFromID(event.jaxis.which))
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
					bool fUseJoystickInput = false;
					bool fJoystickDown = false;

                    if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_LEFT) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_RIGHT) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_UP) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_DOWN) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_LEFT) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_RIGHT) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_UP) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_DOWN) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
					}

                    if (fUseJoystickInput) {
						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

                        if (fJoystickDown) {
							fFound = true;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							outputControl->keys[iKey].fDown = false;
						}
					}
				}
			}
		}

		//This line might be causing input from some players not to be read
		//if (fFound)
			//break;
	}
}
