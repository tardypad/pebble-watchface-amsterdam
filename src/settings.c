/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#include <pebble.h>

#include "config.h"
#include "settings.h"

typedef enum {
  AppKeyDisplayDate = 0,
} AppKey;

static settingsReloadHandler s_settings_reload_handler;
static bool s_display_date = true;

bool settings_display_date() {
  return s_display_date;
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  LOG_DEBUG("Settings received");

  Tuple *display_date_t = dict_find(iter, AppKeyDisplayDate);
  if (display_date_t) {
    s_display_date = display_date_t->value->int32 == 1;
    persist_write_bool(AppKeyDisplayDate, s_display_date);
    LOG_DEBUG("Settings save display date: %s", s_display_date ? "true" : "false");
  }

  s_settings_reload_handler();
}

static void load_settings() {
  if(persist_exists(AppKeyDisplayDate)) {
    s_display_date = persist_read_bool(AppKeyDisplayDate);
    LOG_DEBUG("Settings load display date: %s", s_display_date ? "true" : "false");
  }

  s_settings_reload_handler();
}

void settings_init(settingsReloadHandler settings_reload_handler) {
  s_settings_reload_handler = settings_reload_handler;
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(20, 0);

  load_settings();
}

void settings_deinit() {
  app_message_deregister_callbacks();
}