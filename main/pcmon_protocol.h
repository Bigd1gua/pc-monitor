#pragma once

#include <stdbool.h>
#include <stddef.h>

#define PCMON_HOST_MAX 32
#define PCMON_OS_MAX 32
#define PCMON_USER_MAX 24
#define PCMON_CMODEL_MAX 40
#define PCMON_CORES_MAX 8
#define PCMON_LINE_MAX 512

#define PCMON_PROTO_VER 2

typedef struct {
    bool valid;

    bool has_cpu;
    bool has_cores;
    bool has_freq;
    bool has_proc;
    bool has_mem;
    bool has_mem_detail;
    bool has_swap;
    bool has_disk;
    bool has_disk_detail;
    bool has_disk_io;
    bool has_net;
    bool has_net_total;
    bool has_temp;
    bool has_uptime;
    bool has_bat;
    bool has_pc_power;
    bool has_host;
    bool has_os;
    bool has_user;
    bool has_cmodel;

    float cpu;
    int cores;
    float core_pct[PCMON_CORES_MAX];
    float freq_mhz;
    int proc_count;

    float mem;
    float mem_used_gb;
    float mem_total_gb;
    float swap_pct;
    float swap_used_gb;
    float swap_total_gb;

    float disk;
    float disk_used_gb;
    float disk_total_gb;
    float disk_read_mbs;
    float disk_write_mbs;

    float net_down;
    float net_up;
    float net_recv_mb;
    float net_sent_mb;

    float temp;
    long uptime_s;

    float bat;
    bool bat_plugged;
    long bat_time_s;

    char host[PCMON_HOST_MAX];
    char os[PCMON_OS_MAX];
    char user[PCMON_USER_MAX];
    char cmodel[PCMON_CMODEL_MAX];
} pcmon_stats_t;

typedef struct {
    bool valid;
    int ver;
    bool psutil;
} pcmon_agent_t;

void pcmon_stats_reset(pcmon_stats_t *stats);

bool pcmon_parse_status(const char *frame, pcmon_stats_t *out);

bool pcmon_parse_agent(const char *frame, pcmon_agent_t *out);

int pcmon_format_request(char *out, size_t n);

int pcmon_format_status(const pcmon_stats_t *stats, char *out, size_t n);

int pcmon_format_agent(int psutil, char *out, size_t n);

int pcmon_level(float percent);
