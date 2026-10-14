#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "pcmon_protocol.h"

int main(void)
{
    pcmon_stats_t s;
    pcmon_stats_reset(&s);
    assert(!s.valid);

    const char *frame =
        "$PCSTAT,cpu=12.5,ccnt=4,c0=10.0,c1=20.0,c2=30.0,c3=40.0,freq=3200.0,proc=210,"
        "mem=45.0,mused=7.20,mtotal=16.00,swap=5.0,sused=0.50,stotal=8.00,"
        "disk=60.2,dused=120.50,dtotal=476.00,dread=1.25,dwrite=0.75,"
        "netd=1.500,netu=0.250,nrecv=1024.0,nsent=512.0,"
        "temp=52.0,up=12345,bat=88.0,bplug=1,btime=3600,"
        "host=DESKTOP,os=Windows-11,user=alice,cmodel=AMD Ryzen 5#\n";
    assert(pcmon_parse_status(frame, &s));
    assert(s.valid);
    assert(s.has_cpu && s.cpu > 12.4f && s.cpu < 12.6f);
    assert(s.has_cores && s.cores == 4);
    assert(s.core_pct[0] > 9.9f && s.core_pct[3] > 39.9f && s.core_pct[3] < 40.1f);
    assert(s.has_freq && s.freq_mhz > 3199.0f && s.freq_mhz < 3201.0f);
    assert(s.has_proc && s.proc_count == 210);
    assert(s.has_mem && s.mem > 44.9f && s.mem < 45.1f);
    assert(s.has_mem_detail && s.mem_used_gb > 7.1f && s.mem_total_gb > 15.9f);
    assert(s.has_swap && s.swap_pct > 4.9f && s.swap_total_gb > 7.9f);
    assert(s.has_disk && s.disk > 60.1f && s.disk < 60.3f);
    assert(s.has_disk_detail && s.disk_total_gb > 475.0f);
    assert(s.has_disk_io && s.disk_read_mbs > 1.24f && s.disk_write_mbs > 0.74f);
    assert(s.has_net);
    assert(s.has_net_total && s.net_recv_mb > 1023.0f && s.net_sent_mb > 511.0f);
    assert(s.has_temp);
    assert(s.uptime_s == 12345);
    assert(s.has_bat && s.bat > 87.9f && s.bat < 88.1f);
    assert(s.has_pc_power && s.bat_plugged && s.bat_time_s == 3600);
    assert(strcmp(s.host, "DESKTOP") == 0);
    assert(strcmp(s.os, "Windows-11") == 0);
    assert(strcmp(s.user, "alice") == 0);
    assert(strcmp(s.cmodel, "AMD Ryzen 5") == 0);

    pcmon_stats_t p;
    assert(!pcmon_parse_status("plain text, no frame", &p));
    assert(!pcmon_parse_status("$PCSTAT,cpu=1.0#", &p));
    assert(pcmon_parse_status("log line $PCSTAT,cpu=9,mem=9,xyz=7# trailing", &p));
    assert(p.valid && !p.has_disk && !p.has_host && !p.has_cores);

    const char *core_only = "$PCSTAT,cpu=5.0,mem=50.0,c5=77.0#";
    assert(pcmon_parse_status(core_only, &p));
    assert(p.has_cores && p.cores == 6);
    assert(p.core_pct[5] > 76.9f && p.core_pct[5] < 77.1f);
    assert(p.core_pct[0] < 0.1f);

    pcmon_agent_t ag;
    assert(!pcmon_parse_agent("$PCSTAT,cpu=1,mem=1#", &ag));
    assert(pcmon_parse_agent("noise $PCAGENT,ver=2,psutil=1# tail", &ag));
    assert(ag.valid && ag.ver == 2 && ag.psutil);
    assert(pcmon_parse_agent("$PCAGENT,ver=2,psutil=0#", &ag));
    assert(ag.valid && !ag.psutil);

    char req[16];
    assert(pcmon_format_request(req, sizeof(req)) > 0);
    assert(strncmp(req, "$REQ#", 5) == 0);

    char af[64];
    assert(pcmon_format_agent(1, af, sizeof(af)) > 0);
    assert(pcmon_parse_agent(af, &ag));
    assert(ag.ver == PCMON_PROTO_VER && ag.psutil);

    pcmon_stats_t a;
    pcmon_stats_reset(&a);
    a.valid = true;
    a.has_cpu = a.has_mem = a.has_disk = a.has_net = true;
    a.has_temp = a.has_uptime = a.has_bat = a.has_host = a.has_os = true;
    a.has_cores = a.has_freq = a.has_proc = true;
    a.has_mem_detail = a.has_swap = a.has_disk_detail = a.has_disk_io = true;
    a.has_net_total = a.has_pc_power = a.has_user = a.has_cmodel = true;
    a.cpu = 50.0f;
    a.cores = 3;
    a.core_pct[0] = 11.0f;
    a.core_pct[1] = 22.0f;
    a.core_pct[2] = 33.0f;
    a.freq_mhz = 2400.0f;
    a.proc_count = 180;
    a.mem = 60.0f;
    a.mem_used_gb = 9.5f;
    a.mem_total_gb = 16.0f;
    a.swap_pct = 3.0f;
    a.swap_used_gb = 0.25f;
    a.swap_total_gb = 8.0f;
    a.disk = 70.0f;
    a.disk_used_gb = 333.0f;
    a.disk_total_gb = 476.0f;
    a.disk_read_mbs = 2.5f;
    a.disk_write_mbs = 1.5f;
    a.net_down = 2048.0f;
    a.net_up = 512.0f;
    a.net_recv_mb = 4096.0f;
    a.net_sent_mb = 2048.0f;
    a.temp = 61.0f;
    a.uptime_s = 99;
    a.bat = 77.0f;
    a.bat_plugged = true;
    a.bat_time_s = 1200;
    snprintf(a.host, sizeof(a.host), "BOX");
    snprintf(a.os, sizeof(a.os), "linux");
    snprintf(a.user, sizeof(a.user), "bob");
    snprintf(a.cmodel, sizeof(a.cmodel), "Intel i7");

    char buf[PCMON_LINE_MAX];
    assert(pcmon_format_status(&a, buf, sizeof(buf)) > 0);
    pcmon_stats_t b;
    assert(pcmon_parse_status(buf, &b));
    assert(b.valid && b.uptime_s == 99);
    assert(b.has_host && strcmp(b.host, "BOX") == 0);
    assert(b.has_os && strcmp(b.os, "linux") == 0);
    assert(b.has_user && strcmp(b.user, "bob") == 0);
    assert(b.has_cmodel && strcmp(b.cmodel, "Intel i7") == 0);
    assert(b.cpu > 49.9f && b.cpu < 50.1f);
    assert(b.has_cores && b.cores == 3);
    assert(b.core_pct[1] > 21.9f && b.core_pct[2] > 32.9f);
    assert(b.has_freq && b.freq_mhz > 2399.0f);
    assert(b.has_proc && b.proc_count == 180);
    assert(b.has_mem_detail && b.mem_total_gb > 15.9f);
    assert(b.has_swap && b.swap_total_gb > 7.9f);
    assert(b.has_disk_detail && b.disk_total_gb > 475.0f);
    assert(b.has_disk_io && b.disk_read_mbs > 2.4f);
    assert(b.has_net_total && b.net_recv_mb > 4095.0f);
    assert(b.has_pc_power && b.bat_plugged && b.bat_time_s == 1200);

    assert(pcmon_level(10.0f) == 0);
    assert(pcmon_level(75.0f) == 1);
    assert(pcmon_level(95.0f) == 2);

    puts("test_pcmon_protocol: OK");
    return 0;
}
