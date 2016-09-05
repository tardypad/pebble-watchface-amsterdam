/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#pragma once

typedef void (*settingsReloadHandler)(void);

void settings_init(settingsReloadHandler settings_reload_handler);

void settings_deinit();

bool settings_display_date();