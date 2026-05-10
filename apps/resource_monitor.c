int resource_monitor_sample(unsigned *cpu, unsigned *ram) {
    if (!cpu || !ram) return -1;
    *cpu = 3;
    *ram = 42;
    return 0;
}
