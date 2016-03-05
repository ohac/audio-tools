#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <stdio.h>

void set_volume(const char *card, const char *name, long volume)
{
  long min, max;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, card);
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);
  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
  if (elem != NULL) {
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
  }
  snd_mixer_close(handle);
}
