#include "pcmon_protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCMON_PREFIX "$PCSTAT,"
#define PCMON_AGENT_PREFIX "$PCAGENT,"

void pcmon_stats_reset(pcmon_stats_t *stats)
{
    if (!stats) {
        return;
    }
    memset(stats, 0, sizeof(*stats));
}

static void copy_field(char *dst, size_t n, const char *src, size_t len)
{
    if (len >= n) {
        len = n - 1;
    }
    memcpy(dst, src, len);
    dst[len] = '\0';
}

static bool key_is(const char *key, size_t klen, const char *name)
{
    return strlen(name) == klen && strncmp(key, name, klen) == 0;
}

static int core_index(const char *key, size_t klen)
{
    if (klen != 2 || key[0] != 'c') {
        return -1;
    }
    if (key[1] < '0' || key[1] > '9') {
        return -1;
    }
    int idx = key[1] - '0';
    return idx < PCMON_CORES_MAX ? idx : -1;
}

bool pcmon_parse_status(const char *frame, pcmon_stats_t *out)
{
    if (!frame || !out) {
        return false;
    }
    const char *start = strstr(frame, PCMON_PREFIX);
    if (!start) {
        return false;
    }
    const char *p = start + strlen(PCMON_PREFIX);
    const char *end = strchr(p, '#');
    if (!end) {
        return false;
    }

    pcmon_stats_t st;
    pcmon_stats_reset(&st);

    while (p < end) {
        const char *comma = memchr(p, ',', (size_t)(end - p));
        const char *tok_end = comma ? comma : end;
        const char *eq = memchr(p, '=', (size_t)(tok_end - p));
        if (eq) {
            const char *key = p;
            size_t klen = (size_t)(eq - p);
            const char *val = eq + 1;
            size_t vlen = (size_t)(tok_end - val);
            if (key_is(key, klen, "cpu")) {
                st.cpu = (float)strtod(val, NULL);
                st.has_cpu = true;
            } else if (key_is(key, klen, "ccnt")) {
                st.cores = (int)strtol(val, NULL, 10);
                st.has_cores = true;
            } else if (key_is(key, klen, "freq")) {
                st.freq_mhz = (float)strtod(val, NULL);
                st.has_freq = true;
            } else if (key_is(key, klen, "proc")) {
                st.proc_count = (int)strtol(val, NULL, 10);
                st.has_proc = true;
            } else if (key_is(key, klen, "mem")) {
                st.mem = (float)strtod(val, NULL);
                st.has_mem = true;
            } else if (key_is(key, klen, "mused")) {
                st.mem_used_gb = (float)strtod(val, NULL);
                st.has_mem_detail = true;
            } else if (key_is(key, klen, "mtotal")) {
                st.mem_total_gb = (float)strtod(val, NULL);
                st.has_mem_detail = true;
            } else if (key_is(key, klen, "swap")) {
                st.swap_pct = (float)strtod(val, NULL);
                st.has_swap = true;
            } else if (key_is(key, klen, "sused")) {
                st.swap_used_gb = (float)strtod(val, NULL);
                st.has_swap = true;
            } else if (key_is(key, klen, "stotal")) {
                st.swap_total_gb = (float)strtod(val, NULL);
                st.has_swap = true;
            } else if (key_is(key, klen, "disk")) {
                st.disk = (float)strtod(val, NULL);
                st.has_disk = true;
            } else if (key_is(key, klen, "dused")) {
                st.disk_used_gb = (float)strtod(val, NULL);
                st.has_disk_detail = true;
            } else if (key_is(key, klen, "dtotal")) {
                st.disk_total_gb = (float)strtod(val, NULL);
                st.has_disk_detail = true;
            } else if (key_is(key, klen, "dread")) {
                st.disk_read_mbs = (float)strtod(val, NULL);
                st.has_disk_io = true;
            } else if (key_is(key, klen, "dwrite")) {
                st.disk_write_mbs = (float)strtod(val, NULL);
                st.has_disk_io = true;
            } else if (key_is(key, klen, "netd")) {
                st.net_down = (float)strtod(val, NULL);
                st.has_net = true;
            } else if (key_is(key, klen, "netu")) {
                st.net_up = (float)strtod(val, NULL);
                st.has_net = true;
            } else if (key_is(key, klen, "nrecv")) {
                st.net_recv_mb = (float)strtod(val, NULL);
                st.has_net_total = true;
            } else if (key_is(key, klen, "nsent")) {
                st.net_sent_mb = (float)strtod(val, NULL);
                st.has_net_total = true;
            } else if (key_is(key, klen, "temp")) {
                st.temp = (float)strtod(val, NULL);
                st.has_temp = true;
            } else if (key_is(key, klen, "up")) {
                st.uptime_s = strtol(val, NULL, 10);
                st.has_uptime = true;
            } else if (key_is(key, klen, "bat")) {
                st.bat = (float)strtod(val, NULL);
                st.has_bat = true;
            } else if (key_is(key, klen, "bplug")) {
                st.bat_plugged = strtol(val, NULL, 10) != 0;
                st.has_pc_power = true;
            } else if (key_is(key, klen, "btime")) {
                st.bat_time_s = strtol(val, NULL, 10);
                st.has_pc_power = true;
            } else if (key_is(key, klen, "host")) {
                copy_field(st.host, sizeof(st.host), val, vlen);
                st.has_host = true;
            } else if (key_is(key, klen, "os")) {
                copy_field(st.os, sizeof(st.os), val, vlen);
                st.has_os = true;
            } else if (key_is(key, klen, "user")) {
                copy_field(st.user, sizeof(st.user), val, vlen);
                st.has_user = true;
            } else if (key_is(key, klen, "cmodel")) {
                copy_field(st.cmodel, sizeof(st.cmodel), val, vlen);
                st.has_cmodel = true;
            } else {
                int idx = core_index(key, klen);
                if (idx >= 0) {
                    st.core_pct[idx] = (float)strtod(val, NULL);
                    if (st.cores < idx + 1) {
                        st.cores = idx + 1;
                    }
                    st.has_cores = true;
                }
            }
        }
        if (!comma) {
            break;
        }
        p = comma + 1;
    }

    st.valid = st.has_cpu && st.has_mem;
    *out = st;
    return st.valid;
}

bool pcmon_parse_agent(const char *frame, pcmon_agent_t *out)
{
    if (!frame || !out) {
        return false;
    }
    const char *start = strstr(frame, PCMON_AGENT_PREFIX);
    if (!start) {
        return false;
    }
    const char *p = start + strlen(PCMON_AGENT_PREFIX);
    const char *end = strchr(p, '#');
    if (!end) {
        return false;
    }

    pcmon_agent_t ag;
    memset(&ag, 0, sizeof(ag));

    while (p < end) {
        const char *comma = memchr(p, ',', (size_t)(end - p));
        const char *tok_end = comma ? comma : end;
        const char *eq = memchr(p, '=', (size_t)(tok_end - p));
        if (eq) {
            const char *key = p;
            size_t klen = (size_t)(eq - p);
            const char *val = eq + 1;
            if (key_is(key, klen, "ver")) {
                ag.ver = (int)strtol(val, NULL, 10);
            } else if (key_is(key, klen, "psutil")) {
                ag.psutil = strtol(val, NULL, 10) != 0;
            }
        }
        if (!comma) {
            break;
        }
        p = comma + 1;
    }

    ag.valid = true;
    *out = ag;
    return true;
}

int pcmon_format_request(char *out, size_t n)
{
    if (!out || n == 0) {
        return -1;
    }
    return snprintf(out, n, "$REQ#\n");
}

int pcmon_format_status(const pcmon_stats_t *stats, char *out, size_t n)
{
    if (!stats || !out || n == 0) {
        return -1;
    }

    int off = 0;
    int r = snprintf(out, n, "%s", PCMON_PREFIX);
    if (r < 0 || (size_t)r >= n) {
        return -1;
    }
    off = r;

#define PCMON_APP(...)                                                      \
    do {                                                                    \
        r = snprintf(out + off, n - (size_t)off, __VA_ARGS__);              \
        if (r < 0 || (size_t)r >= n - (size_t)off) {                        \
            return -1;                                                      \
        }                                                                   \
        off += r;                                                           \
    } while (0)

    PCMON_APP("cpu=%.1f", (double)stats->cpu);
    if (stats->has_cores) {
        PCMON_APP(",ccnt=%d", stats->cores);
        int shown = stats->cores;
        if (shown > PCMON_CORES_MAX) {
            shown = PCMON_CORES_MAX;
        }
        for (int i = 0; i < shown; i++) {
            PCMON_APP(",c%d=%.1f", i, (double)stats->core_pct[i]);
        }
    }
    if (stats->has_freq) {
        PCMON_APP(",freq=%.1f", (double)stats->freq_mhz);
    }
    if (stats->has_proc) {
        PCMON_APP(",proc=%d", stats->proc_count);
    }
    PCMON_APP(",mem=%.1f", (double)stats->mem);
    if (stats->has_mem_detail) {
        PCMON_APP(",mused=%.2f,mtotal=%.2f", (double)stats->mem_used_gb,
                  (double)stats->mem_total_gb);
    }
    if (stats->has_swap) {
        PCMON_APP(",swap=%.1f,sused=%.2f,stotal=%.2f", (double)stats->swap_pct,
                  (double)stats->swap_used_gb, (double)stats->swap_total_gb);
    }
    PCMON_APP(",disk=%.1f", (double)stats->disk);
    if (stats->has_disk_detail) {
        PCMON_APP(",dused=%.2f,dtotal=%.2f", (double)stats->disk_used_gb,
                  (double)stats->disk_total_gb);
    }
    if (stats->has_disk_io) {
        PCMON_APP(",dread=%.2f,dwrite=%.2f", (double)stats->disk_read_mbs,
                  (double)stats->disk_write_mbs);
    }
    PCMON_APP(",netd=%.3f,netu=%.3f", (double)stats->net_down, (double)stats->net_up);
    if (stats->has_net_total) {
        PCMON_APP(",nrecv=%.1f,nsent=%.1f", (double)stats->net_recv_mb,
                  (double)stats->net_sent_mb);
    }
    if (stats->has_temp) {
        PCMON_APP(",temp=%.1f", (double)stats->temp);
    }
    if (stats->has_uptime) {
        PCMON_APP(",up=%ld", stats->uptime_s);
    }
    if (stats->has_bat) {
        PCMON_APP(",bat=%.1f", (double)stats->bat);
    }
    if (stats->has_pc_power) {
        PCMON_APP(",bplug=%d,btime=%ld", stats->bat_plugged ? 1 : 0, stats->bat_time_s);
    }
    if (stats->has_host) {
        PCMON_APP(",host=%s", stats->host);
    }
    if (stats->has_os) {
        PCMON_APP(",os=%s", stats->os);
    }
    if (stats->has_user) {
        PCMON_APP(",user=%s", stats->user);
    }
    if (stats->has_cmodel) {
        PCMON_APP(",cmodel=%s", stats->cmodel);
    }
    PCMON_APP("#\n");

#undef PCMON_APP

    return off;
}

int pcmon_format_agent(int psutil, char *out, size_t n)
{
    if (!out || n == 0) {
        return -1;
    }
    return snprintf(out, n, "%sver=%d,psutil=%d#\n", PCMON_AGENT_PREFIX,
                    PCMON_PROTO_VER, psutil ? 1 : 0);
}

int pcmon_level(float percent)
{
    if (percent >= 90.0f) {
        return 2;
    }
    if (percent >= 70.0f) {
        return 1;
    }
    return 0;
}
