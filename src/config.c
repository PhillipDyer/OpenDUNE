/* $Id$ */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "libemu.h"
#include "config.h"
#include "global.h"
#include "file.h"

/**
 * Reads and decode the config.
 *
 * @param filename The name of file containing config.
 * @param config The address where the config will be stored.
 * @return True if loading and decoding is successful.
 */
bool Config_Read(char *filename, DuneCfg *config)
{
	char name[18]; /* "data/8.3" */
	FILE *f;
	size_t read;
	uint8 sum;
	uint8 *c;
	int8 i;

	assert(strlen(filename) < 13);

	strcpy(name, DATA_DIR);
	strcat(name, filename);

	f = fopen(name, "rb");
	if (f == NULL) return false;

	read = fread(config, 1, sizeof(DuneCfg), f);
	fclose(f);

	if (read != sizeof(DuneCfg)) return false;

	sum = 0;

	for (c = (uint8 *)config, i = 7; i >= 0; c++, i--) {
		*c ^= 0xA5;
		*c -= i;
		sum += *c;
	}

	sum ^= 0xA5;

	/* Force audio drivers. */
	if (config->soundDrv != 0) config->soundDrv = 7;
	if (config->musicDrv != 0) config->musicDrv = 7;
	if (config->voiceDrv != 0) config->voiceDrv = 1;

	return (sum == config->checksum);
}

/**
 * Loads the game options.
 *
 * @return True if loading is successful.
 */
bool GameOptions_Load()
{
	if (!File_Exists(g_global->string_2AB7)) return false;

	File_ReadBlockFile(g_global->string_2AB7, &g_global->gameConfig, sizeof(g_global->gameConfig));

	Drivers_EnableMusic(g_global->gameConfig.music);

	Drivers_EnableSounds(g_global->gameConfig.sounds);

	g_global->gameSpeed = g_global->gameConfig.gameSpeed;

	return true;
}

/**
 * Saves the game options.
 */
void GameOptions_Save()
{
	uint8 index;

	index = File_Open(g_global->string_2AB7, 2);

	File_Write(index, &g_global->gameConfig, sizeof(g_global->gameConfig));

	File_Close(index);

	if (g_global->gameConfig.music == 0) Music_Play(0);
}
