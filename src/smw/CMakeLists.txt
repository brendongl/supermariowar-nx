add_executable(smw WIN32
	main.cpp

	ai.cpp
	ai.h
	FPSLimiter.cpp
	FPSLimiter.h
	GSGameplay.cpp
	GSGameplay.h
	GSMenu.cpp
	GSMenu.h
	GSSplashScreen.cpp
	GSSplashScreen.h
	net.cpp
	net.h
	network/FileCompressor.cpp
	network/FileCompressor.h
	network/NetConfigManager.cpp
	network/NetConfigManager.h
	ObjectContainer.cpp
	ObjectContainer.h
	objectgame.cpp
	objectgame.h
	objecthazard.cpp
	objecthazard.h
	player.cpp
	player.h
	world.cpp
	world.h

	../common/movingplatform.cpp
	../common/movingplatform.h
)

add_subdirectory(player_components)
add_subdirectory(world)
add_subdirectory(menu)
add_subdirectory(gamemodes)
add_subdirectory(objects)
add_subdirectory(ui)

target_compile_features(smw PUBLIC cxx_std_14)

if(NO_NETWORK)
	target_compile_definitions(smw PRIVATE NETWORK_DISABLED)
else()
	target_sources(smw PRIVATE
		platform/network/enet/NetworkLayerENet.cpp
		platform/network/enet/NetworkLayerENet.h
	)
endif()

if(NINTENDO_SWITCH)
	target_sources(smw PRIVATE
		platform/switch/SwitchPlatform.cpp
		platform/switch/SwitchPlatform.h
	)
endif()

if (WIN32)
	# Add EXE icon on Windows
	target_sources(smw PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw.rc")
endif()

#
# Dependencies
#

target_link_libraries(smw CommonFiles)
if(NOT NO_NETWORK)
	target_link_libraries(smw NetplayFiles)
endif()

if(NOT EMSCRIPTEN)
	target_link_libraries(smw smw_zlib)
endif()

target_link_libraries(smw toml11::toml11)

#
# Installation
#

install(TARGETS smw DESTINATION "${SMW_BINDIR}")

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT SMW_INSTALL_PORTABLE)
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar.desktop" DESTINATION "${SMW_DESKTOPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw.xpm" DESTINATION "${SMW_PIXMAPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw.png" DESTINATION "${SMW_ICONDIR}/32x32/apps/")
endif()

#
# Nintendo Switch: NRO with the game data in RomFS
#
if(NINTENDO_SWITCH)
	set(SMW_ROMFS_DIR "${CMAKE_BINARY_DIR}/romfs")
	set(SMW_ROMFS_STAMP "${CMAKE_BINARY_DIR}/smw-romfs.stamp")
	file(MAKE_DIRECTORY "${SMW_ROMFS_DIR}")
	add_custom_target(smw_romfs
		BYPRODUCTS "${SMW_ROMFS_STAMP}"
		COMMAND ${CMAKE_COMMAND} -E rm -rf "${SMW_ROMFS_DIR}/data"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/data" "${SMW_ROMFS_DIR}/data"
		COMMAND ${CMAKE_COMMAND} -E rm -rf "${SMW_ROMFS_DIR}/data/.git" "${SMW_ROMFS_DIR}/data/maps/ZZleveleditor.map"
		# Extra content shipped with this port (e.g. the Sip n Play Dice Guy skin)
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/platform/switch/data" "${SMW_ROMFS_DIR}/data"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/platform/widescreen/data" "${SMW_ROMFS_DIR}/data"
		COMMAND ${CMAKE_COMMAND} -E touch "${SMW_ROMFS_STAMP}"
		COMMENT "Staging game data into RomFS"
		VERBATIM
	)
	# Pass a devkitPro asset target so changing only images/maps repacks the NRO.
	# A raw ROMFS directory gives nx_create_nro no file dependencies.
	set_target_properties(smw_romfs PROPERTIES
		DKP_ASSET_FOLDER "${SMW_ROMFS_DIR}"
		DKP_ASSET_FILES "${SMW_ROMFS_STAMP}"
	)
	nx_generate_nacp(OUTPUT smw.nacp
		NAME "${SMW_SWITCH_TITLE}"
		AUTHOR "${SMW_SWITCH_AUTHOR}"
		VERSION "${SMW_SWITCH_VERSION}"
	)
	nx_create_nro(smw
		NACP smw.nacp
		ICON "${PROJECT_SOURCE_DIR}/platform/switch/icon.jpg"
		ROMFS smw_romfs
	)
	add_dependencies(smw_nro smw_romfs)
endif()
