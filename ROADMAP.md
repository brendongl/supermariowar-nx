# Roadmap

Ideas and plans for Super Mario War NX that are not built yet. Want to help with one? Open an issue to talk it over first.

## Player accounts and stats

Keep a record of kills, wins, games played and tournament results for each player across sessions.

- Before a match, each player is asked if they have an account. Players who do link it by scanning a QR code shown on the console screen with their phone. Everyone else plays as a guest.
- Results upload once a match finishes: kills, deaths, wins and placings by game mode and map.
- Player profile pages on the web, plus leaderboards.
- Tournament and tour brackets with history, so a night of games at the cafe becomes one event you can look back on.
- First home: [Sip n Play](https://sipnplay.cafe) accounts.

Open questions:
- Consoles without internet access: keep results on the SD card and upload them later.
- Privacy: guests by default, and nothing is uploaded unless a player has linked an account.

## Online multiplayer in the browser

A web build (the game already builds with Emscripten) where people play together online, sharing the same accounts and stats. This comes after accounts.

## Widescreen (16:9) stages

Stages are 20x15 tiles (640x480). True 16:9 means 27x15 tiles. Stages wrap from one side to the other, so each one needs 7 new columns designed by hand, and its background redrawn wider. The plan is a separate widescreen stage pack, made with help from the community. Until then the Switch build fills the 16:9 screen with side panels.

## Smaller items

- A standalone NSP that launches on hardware. It currently crashes Atmosphère's `fatal` module, so the forwarder NSP is used instead.
- Test with real controllers: 4 players, docking and undocking, and suspend and resume from HOME.
- Send the general bug fixes back to the [upstream project](https://github.com/mmatyas/supermariowar).
