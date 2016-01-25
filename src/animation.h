/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#pragma once

void animation_load(Layer *layer);

void animation_unload(Layer *layer);

void animation_start_sequence(struct tm* tick_time);

char* animation_time_text();

char* animation_date_text();
