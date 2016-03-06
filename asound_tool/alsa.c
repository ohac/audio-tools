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

int connect_midi(const char* sender_name, const char* dest_name)
{
  snd_seq_t* _seq;
  int ret1 = snd_seq_open(&_seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
  if (ret1 < 0) {
    return -1;
  }
  snd_seq_client_info_t* cinfo;
  snd_seq_client_info_alloca(&cinfo);
  snd_seq_client_info_set_client(cinfo, -1);

  snd_seq_port_info_t* pinfo;
  snd_seq_port_info_alloca(&pinfo);
  snd_seq_addr_t sender;
  snd_seq_addr_t dest;
  int found = 0;

  while (snd_seq_query_next_client(_seq, cinfo) >= 0) {
    snd_seq_client_type_t ctype = snd_seq_client_info_get_type(cinfo);
    int cid = snd_seq_client_info_get_client(cinfo);
    const char * cname = snd_seq_client_info_get_name(cinfo);

    snd_seq_port_info_set_client(pinfo, cid);
    snd_seq_port_info_set_port(pinfo, -1);
    while (snd_seq_query_next_port(_seq, pinfo) >= 0) {
      unsigned int ptype = snd_seq_port_info_get_type(pinfo);
      int cap = snd_seq_port_info_get_capability(pinfo);
      const char *pname = snd_seq_port_info_get_name(pinfo);
      int midi_chs = snd_seq_port_info_get_midi_channels(pinfo);
      int midi_vos = snd_seq_port_info_get_midi_voices(pinfo);
      if (ptype & (SND_SEQ_PORT_TYPE_PORT | SND_SEQ_PORT_TYPE_APPLICATION)) {
        if (cap & SND_SEQ_PORT_CAP_SUBS_READ) {
          if (!strcmp(pname, "microKORG XL MIDI 2")) {
            sender = *snd_seq_port_info_get_addr(pinfo);
            found |= 1 << 0;
          }
        }
        if (cap & SND_SEQ_PORT_CAP_SUBS_WRITE) {
          if (!strcmp(pname, "nekobee DSSI plugin")) {
            dest = *snd_seq_port_info_get_addr(pinfo);
            found |= 1 << 1;
          }
        }
      }
    }
  }
  if (found & 3) {
    snd_seq_port_subscribe_t* subs;
    snd_seq_port_subscribe_malloc(&subs);
    snd_seq_port_subscribe_set_sender(subs, &sender);
    snd_seq_port_subscribe_set_dest(subs, &dest);
    snd_seq_port_subscribe_set_exclusive(subs, 0);
    snd_seq_port_subscribe_set_time_update(subs, 0);
    snd_seq_port_subscribe_set_time_real(subs, 0);
    if (!snd_seq_get_port_subscription(_seq, subs)) {
      return -1;
    }
    else {
      int ret2 = snd_seq_subscribe_port(_seq, subs);
      if (ret2 < 0) {
        return -1;
      }
    }
  }
  return 0;
}
