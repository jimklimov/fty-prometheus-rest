# fty-prometheus-rest

REST API /metrics implementation of prometheus collector.

## How to build

To build fty-prometheus-rest project run:

```bash
./autogen.sh
./configure
make
```

## How to run

deploy libfty_prometheus_rest.so in /usr/lib/bios folder
copy src/09_prometheus.xml to /etc/tntnet/bios.d 

```bash
systemctl restart tntnet@bios
```

## How to test

```bash
curl --insecure https://<ip_address>/metrics
```

```bash
# HELP ipm_realpower_watt current realpower 
# TYPE ipm_realpower_watt gauge
ipm_realpower_watt{ device="rack-7"} 7
ipm_realpower_watt{ device="rack-6"} 35
# HELP ipm_temperature_celcius current ambiant temperature 
# TYPE ipm_temperature_celcius gauge
ipm_temperature_celcius{ device="room-4"} 36
ipm_temperature_celcius{ device="row-5"} 36
# HELP ipm_humidity_percent current ambiant temperature 
# TYPE ipm_humidity_percent gauge
ipm_humidity_percent{ device="room-4"} 16.89
```

## How to configure prometheus.yml

add this section in "scrape_configs"
```bash
 - job_name: 'ipm'
   scheme: https
   tls_config:
     insecure_skip_verify: true
   static_configs:
     - targets: ['<ip_addr>:443']
```
then restart prometheus


##  what kind of metrics are collected by prometheus ?

for now the published metrics are :
```bash
# HELP ipm_realpower_watt current realpower
# TYPE ipm_realpower_watt gauge
# HELP ipm_temperature_celcius current ambiant temperature
# TYPE ipm_temperature_celcius gauge
# HELP ipm_humidity_percent current ambiant temperature
# TYPE ipm_humidity_percent gauge
```

