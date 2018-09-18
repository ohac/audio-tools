/*
  Copyright 2013-2016 David Robillard <d@drobilla.net>
  Copyright 2018 OHASHI Hideya <ohachige@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#define MIDIFALSE_URI "http://lv2.sighash.info/midifalse"
#define MIDIFALSE__onoff MIDIFALSE_URI "#onoff"

#include "script.h"

typedef enum {
  MIDIFALSE_IN = 0,
  MIDIFALSE_OUT = 1
} PortIndex;

typedef struct {
  const LV2_Atom_Sequence* in;
  LV2_Atom_Sequence* out;

  LV2_URID_Map* map;
  LV2_Log_Log* log;
  LV2_Log_Logger logger;
  LV2_Atom_Forge forge;

  LV2_Atom_Forge_Frame notify_frame;

  struct {
    LV2_URID atom_Float;
    LV2_URID atom_String;
    LV2_URID atom_Path;
    LV2_URID atom_Resource;
    LV2_URID atom_Sequence;
    LV2_URID atom_URID;
    LV2_URID atom_eventTransfer;
    LV2_URID midi_Event;
    LV2_URID patch_Set;
    LV2_URID patch_Get;
    LV2_URID midifalse_onoff;
    LV2_URID param_gain;
    LV2_URID patch_property;
    LV2_URID patch_value;
  } uris;

  char* onoff_handler;
} Midifalse;

static LV2_Handle
instantiate(const LV2_Descriptor* descriptor, double rate,
            const char* bundle_path,
            const LV2_Feature* const* features)
{
  Midifalse* self = (Midifalse*)calloc(1, sizeof(Midifalse));
  LV2_URID_Map* map = NULL;
  int i;

  for (i = 0; features[i]; ++i) {
    if (!strcmp (features[i]->URI, LV2_URID__map)) {
      map = (LV2_URID_Map*)features[i]->data;
      self->map = map;
    } else if (!strcmp (features[i]->URI, LV2_LOG__log)) {
      self->log = (LV2_Log_Log*)features[i]->data;
    }
  }
  lv2_atom_forge_init(&self->forge, map);
  lv2_log_logger_init(&self->logger, map, self->log);
  if (!map) {
    lv2_log_error(&self->logger, "Missing feature\n");
    free(self);
    return NULL;
  }
  self->uris.atom_Float = map->map(map->handle, LV2_ATOM__Float);
  self->uris.atom_String = map->map(map->handle, LV2_ATOM__String);
  self->uris.atom_Path = map->map(map->handle, LV2_ATOM__Path);
  self->uris.atom_Resource = map->map(map->handle, LV2_ATOM__Resource);
  self->uris.atom_Sequence = map->map(map->handle, LV2_ATOM__Sequence);
  self->uris.atom_URID = map->map(map->handle, LV2_ATOM__URID);
  self->uris.atom_eventTransfer = map->map(map->handle, LV2_ATOM__eventTransfer);
  self->uris.midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
  self->uris.param_gain = map->map(map->handle, LV2_PARAMETERS__gain);
  self->uris.patch_Get = map->map(map->handle, LV2_PATCH__Get);
  self->uris.patch_Set = map->map(map->handle, LV2_PATCH__Set);
  self->uris.patch_property = map->map(map->handle, LV2_PATCH__property);
  self->uris.patch_value = map->map(map->handle, LV2_PATCH__value);
  self->uris.midifalse_onoff = map->map(map->handle, MIDIFALSE__onoff);

  return (LV2_Handle)self;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Midifalse* self = (Midifalse*)instance;
  switch ((PortIndex)port) {
  case MIDIFALSE_IN:
    self->in = (const LV2_Atom_Sequence*)data;
    break;
  case MIDIFALSE_OUT:
    self->out = (LV2_Atom_Sequence*)data;
    break;
  }
}

static void
activate(LV2_Handle instance)
{
}

static void
run(LV2_Handle instance, uint32_t sample_count)
{
  Midifalse* self = (Midifalse*)instance;
  const uint32_t out_capacity = self->out->atom.size;
  typedef struct {
    LV2_Atom_Event event;
    uint8_t msg[3];
  } MIDINoteEvent;
  MIDINoteEvent evo;
  uint8_t script_stack[256];
  int script_stackp;

  lv2_atom_forge_set_buffer(&self->forge,
                            (uint8_t*)self->out,
                            out_capacity);
  lv2_atom_forge_sequence_head(&self->forge, &self->notify_frame, 0);

  lv2_atom_sequence_clear(self->out);
  self->out->atom.type = self->in->atom.type;
  LV2_ATOM_SEQUENCE_FOREACH(self->in, ev) {
    if (ev->body.type == self->uris.midi_Event) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
      case LV2_MIDI_MSG_NOTE_OFF:
        evo.event.time.frames = ev->time.frames;
        evo.event.body.type = ev->body.type;
        evo.event.body.size = ev->body.size;
        script_stack[0] = msg[0];
        script_stack[1] = msg[1];
        script_stack[2] = msg[2];
        script_stackp = 3;
        if (self->onoff_handler) {
          script_stackp = run_script(self->onoff_handler, script_stack,
            script_stackp);
        }
        while (script_stackp >= 3) {
          script_stackp -= 3;
          evo.msg[0] = script_stack[script_stackp];
          evo.msg[1] = script_stack[script_stackp + 1];
          evo.msg[2] = script_stack[script_stackp + 2];
          lv2_atom_sequence_append_event(self->out, out_capacity, &evo.event);
        }
        break;
      default:
        lv2_atom_sequence_append_event(self->out, out_capacity, ev);
        break;
      }
    } else if (lv2_atom_forge_is_object_type(&self->forge, ev->body.type)) {
      const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
      if (obj->body.otype == self->uris.patch_Set) {
        const LV2_Atom* property = NULL;
        const LV2_Atom* value = NULL;
        lv2_atom_object_get(obj, self->uris.patch_property, &property,
                            self->uris.patch_value, &value, 0);
        if (!property) continue;
        if (property->type != self->uris.atom_URID) continue;
        const uint32_t key = ((const LV2_Atom_URID*)property)->body;
        if (key == self->uris.midifalse_onoff) {
          const LV2_Atom* property = NULL;
          const LV2_Atom* value = NULL;
          lv2_atom_object_get(obj, self->uris.patch_property, &property,
                              self->uris.patch_value, &value, 0);
          if (value->type == self->uris.atom_String) {
            char* cfg = (char*)((LV2_Atom_String*)value + 1);
            if (self->onoff_handler) free(self->onoff_handler);
            self->onoff_handler = calloc(1, strlen(cfg) + 1);
            strcpy(self->onoff_handler, cfg);
          }
        }
      } else if (obj->body.otype == self->uris.patch_Get) {
        char *cfg = self->onoff_handler;
        if (cfg == NULL) cfg = "";
        LV2_Atom_Forge_Frame frame;
        lv2_atom_forge_frame_time(&self->forge, ev->time.frames);
        lv2_atom_forge_object( &self->forge, &frame, 0, self->uris.patch_Set);
        lv2_atom_forge_key(&self->forge, self->uris.patch_property);
        lv2_atom_forge_urid(&self->forge, self->uris.midifalse_onoff);
        lv2_atom_forge_key(&self->forge, self->uris.patch_value);
        lv2_atom_forge_string(&self->forge, cfg, strlen(cfg));
        lv2_atom_forge_pop(&self->forge, &frame);
      }
    }
  }
}

static void
deactivate(LV2_Handle instance)
{
}

static void
cleanup(LV2_Handle instance)
{
  Midifalse* self = (Midifalse*)instance;
  free(self->onoff_handler);
  free(instance);
}

static LV2_State_Status
save (LV2_Handle instance,
      LV2_State_Store_Function store,
      LV2_State_Handle handle,
      uint32_t flags,
      const LV2_Feature* const* features)
{
  Midifalse* self = (Midifalse*)instance;
  char *cfg = self->onoff_handler;
  if (cfg == NULL) cfg = "";
  store(handle, self->uris.midifalse_onoff,
      cfg, strlen(cfg) + 1,
      self->uris.atom_String,
      LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
  return LV2_STATE_SUCCESS;
}

static LV2_State_Status
restore (LV2_Handle instance,
         LV2_State_Retrieve_Function retrieve,
         LV2_State_Handle handle,
         uint32_t flags,
         const LV2_Feature* const* features)
{
  Midifalse* self = (Midifalse*)instance;
  size_t size;
  uint32_t type;
  uint32_t valflags;
  const void* value;
  free(self->onoff_handler);
  self->onoff_handler = NULL;
  value = retrieve(handle, self->uris.midifalse_onoff, &size, &type, &valflags);
  if (value) {
    const char* cfg = (const char*)value;
    self->onoff_handler = calloc(1, strlen(cfg) + 1);
    strcpy(self->onoff_handler, cfg);
  }
  return LV2_STATE_SUCCESS;
}

static const void*
extension_data(const char* uri)
{
  static const LV2_State_Interface state = { save, restore };
  if (!strcmp(uri, LV2_STATE__interface)) {
    return &state;
  }
  return NULL;
}

static const LV2_Descriptor descriptor = {
  MIDIFALSE_URI,
  instantiate,
  connect_port,
  activate,
  run,
  deactivate,
  cleanup,
  extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptor;
  default:
    return NULL;
  }
}
