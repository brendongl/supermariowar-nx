# Conversation 1562424909341265921 — approved widescreen maps

User approved final 23 stage pictures (24 candidates minus Rat Race) and said “im happy lets go”.
Scope: convert to playable maps; original maps only at <=4 players, approved wide maps only at >4.
Repo: /home/brendongl/projects/supermariowar-nx, branch eight-players; baseline e9206ecb.
Pre-existing untracked servers.toml is untouched. No helpers used. No device touched or public push/release.

## Completed gates

- Preserved approved v2/v3/v4 references, names and revisions in tools/widescreen.
- Authored 23 binary 27x15 maps using engine serializer. 0smw Wide now shares the same overlay: 24 wide maps total.
- Separate live blocks and collision data; original center preserved except requested music/yellow-block edits. Independent verification checks 6,900 center cells, source items/warps/hazards, live side blocks.
- Manual picker/random and Quick Game width filtering implemented and integration-tested at 2,4,5,8,4,8 players (290 normal / 24 wide).
- Eight-slot selector: slots 5–8 CPU-only, random skins, individual teams. Scripted tours/worlds/minigames hidden above four. Controller input still limited to four humans.
- Full 23-map eight-CPU ASan/UBSan sweep: 22 initially passed; The Pit failed overlap detection in Greed. Fixed nonfatal ceiling spike resolution, targeted 90-second retest passed four matches. No sanitizer errors in the full sweep.
- Three original maps passed four-player regressions. Two maps passed five-player boundary tests (three matches each). Winterland final-art retest passed two eight-CPU matches.
- Removed right registration strips, including partially masked Winterland strip. Actual game captures in gameplay-preview.jpg.
- 61 unit tests / 185 assertions pass. Fixed discovered FileIO dangling filename and transactional legacy four-slot TOML parsing.
- Normal PC build and Switch NRO build succeed. Fixed NRO asset dependency so image/map-only changes actually repack RomFS. Verified all 140 shared asset files occur byte-for-byte in the final NRO.

## Changed files

Shared overlay platform/widescreen/data; author tools/references under tools/widescreen;
map authoring/thumbnail/selection, player/team/match menus, collision resolution, FileIO/TOML,
CMake packaging, tests, README, this evidence directory. Upstream data submodule unchanged.

## Missing evidence / blocked gates

- Physical Switch/controller validation remains pending: Fox occupied by another reservation. Own queued job d9790d02-e93b-4b08-bcb5-bb05af05ade3 cancelled cleanly; do not heartbeat or use it.
- Isolated Eden build-wide-eden.UMYqPj aborted before loading the NRO with std::logic_error (basic_string null). This is an unavailable emulator gate, not a failed game test. Private Xvfb stopped. Do not repeatedly probe the same emulator failure.
- No hardware performance claims for this pack. Game Lab todo #4 remains open for validation.

## Evidence and next action

validation.json retains full results including the original failure and subsequent successful retest.
pack-manifest.json identifies the exact embedded assets; provenance.json records source/reference hashes.
Final binary: build-switch/src/smw/smw.nro; delivery ZIP under build-wide-delivery.*.
No active processes/helpers or leases required by this task remain.
Next: obtain a new Fox reservation when available, install/test the final NRO via switchctl, confirm manual player selection, 4->5->8->4 map lists and gameplay, then release lease. Bowser/cafe consoles require separate authorization. Do not push or publicly release until asked.
