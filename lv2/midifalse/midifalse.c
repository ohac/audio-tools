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
#include <stdlib.h>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#define MIDIFALSE_URI "http://sighash.info/lv2/midifalse"

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

  struct {
    LV2_URID midi_Event;
  } uris;
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
  lv2_log_logger_init(&self->logger, map, self->log);
  if (!map) {
    lv2_log_error(&self->logger, "Missing feature\n");
    free(self);
    return NULL;
  }
  self->uris.midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
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
        script_stackp = run_script(note_onoff_handler, script_stack, 3);
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
  free(instance);
}

static const void*
extension_data(const char* uri)
{
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
